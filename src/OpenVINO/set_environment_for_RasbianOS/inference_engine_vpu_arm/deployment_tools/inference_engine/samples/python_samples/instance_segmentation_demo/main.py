#!/usr/bin/env python
"""
 Copyright (c) 2019 Intel Corporation

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
"""

from __future__ import print_function

import logging as log
import os
import sys
import time
from argparse import ArgumentParser, SUPPRESS

import cv2
import numpy as np
from openvino.inference_engine import IENetwork, IEPlugin

from instance_segmentation_demo.images_capture import ImagesCapture
from instance_segmentation_demo.tracker import StaticIOUTracker
from instance_segmentation_demo.visualizer import Visualizer


def build_argparser():
    parser = ArgumentParser(add_help=False)
    args = parser.add_argument_group('Options')
    args.add_argument('-h', '--help', action='help', default=SUPPRESS,
                      help='Show this help message and exit.')
    args.add_argument('-m', '--model',
                      help='Required. Path to an .xml file with a trained model.',
                      required=True, type=str, metavar='"<path>"')
    args.add_argument('--labels',
                      help='Required. Path to a text file with class labels.',
                      required=True, type=str, metavar='"<path>"')
    args.add_argument('-v', '--video',
                      help='Required for video input. Path to a video file or numeric camera ID.',
                      default=None, type=str, metavar='"<path>"')
    args.add_argument('-i', '--images',
                      help='Required for still images input. Path to an image or a folder with images.',
                      type=str, metavar='"<path>"')
    args.add_argument('-d', '--device',
                      help='Optional. Specify the target device to infer on: CPU, GPU, FPGA, HDDL or MYRIAD. '
                           'The demo will look for a suitable plugin for device specified '
                           '(by default, it is CPU).',
                      default='CPU', type=str, metavar='"<device>"')
    args.add_argument('-l', '--cpu_extension',
                      help='Required for CPU custom layers. '
                           'Absolute path to a shared library with the kernels implementation.',
                      default=None, type=str, metavar='"<absolute_path>"')
    args.add_argument('-pp', '--plugin_dir',
                      help='Optional. Path to a plugin folder.',
                      default=None, type=str, metavar='"<absolute_path>"')
    args.add_argument('-pt', '--prob_threshold',
                      help='Optional. Probability threshold for detections filtering.',
                      default=0.5, type=float, metavar='"<num>"')
    args.add_argument('--show_scores',
                      help='Optional. Show detection scores.',
                      action='store_true')
    args.add_argument('--show_boxes',
                      help='Optional. Show bounding boxes.',
                      action='store_true')
    args.add_argument('-pc', '--perf_counts',
                      help='Optional. Report performance counters',
                      action='store_true')
    return parser


def expand_box(box, scale):
    w_half = (box[2] - box[0]) * .5
    h_half = (box[3] - box[1]) * .5
    x_c = (box[2] + box[0]) * .5
    y_c = (box[3] + box[1]) * .5
    w_half *= scale
    h_half *= scale
    box_exp = np.zeros(box.shape)
    box_exp[0] = x_c - w_half
    box_exp[2] = x_c + w_half
    box_exp[1] = y_c - h_half
    box_exp[3] = y_c + h_half
    return box_exp


def segm_postprocess(box, raw_cls_mask, im_h, im_w):
    # Add zero border to prevent upsampling artifacts on segment borders.
    raw_cls_mask = np.pad(raw_cls_mask, ((1, 1), (1, 1)), 'constant', constant_values=0)
    extended_box = expand_box(box, raw_cls_mask.shape[0] / (raw_cls_mask.shape[0] - 2.0)).astype(int)
    w, h = np.maximum(extended_box[2:] - extended_box[:2] + 1, 1)
    x0, y0 = np.clip(extended_box[:2], a_min=0, a_max=[im_w, im_h])
    x1, y1 = np.clip(extended_box[2:] + 1, a_min=0, a_max=[im_w, im_h])

    raw_cls_mask = cv2.resize(raw_cls_mask, (w, h)) > 0.5
    mask = raw_cls_mask.astype(np.uint8)
    # Put an object mask in an image mask.
    im_mask = np.zeros((im_h, im_w), dtype=np.uint8)
    im_mask[y0:y1, x0:x1] = mask[(y0 - extended_box[1]):(y1 - extended_box[1]),
                            (x0 - extended_box[0]):(x1 - extended_box[0])]
    return im_mask


def main():
    log.basicConfig(format='[ %(levelname)s ] %(message)s', level=log.INFO, stream=sys.stdout)
    args = build_argparser().parse_args()

    model_xml = args.model
    model_bin = os.path.splitext(model_xml)[0] + '.bin'

    # Plugin initialization for specified device and load extensions library if specified.
    log.info('Initializing plugin for {} device...'.format(args.device))
    plugin = IEPlugin(device=args.device, plugin_dirs=args.plugin_dir)
    if args.cpu_extension and 'CPU' in args.device:
        plugin.add_cpu_extension(args.cpu_extension)

    # Read IR
    log.info('Reading IR...')
    net = IENetwork(model=model_xml, weights=model_bin)

    if plugin.device == 'CPU':
        supported_layers = plugin.get_supported_layers(net)
        not_supported_layers = [l for l in net.layers.keys() if l not in supported_layers]
        if len(not_supported_layers) != 0:
            log.error('Following layers are not supported by the plugin for specified device {}:\n\t{}'.
                      format(plugin.device,
                             '\n\t'.join('{} ({} with params {})'.format(layer_id, net.layers[layer_id].type,
                                                                         str(net.layers[layer_id].params))
                                         for layer_id in not_supported_layers)
                             )
                      )
            log.error("Please try to specify cpu extensions library path in demo's command line parameters using -l "
                      "or --cpu_extension command line argument")
            sys.exit(1)

    required_input_keys = {'im_data', 'im_info'}
    assert required_input_keys == set(net.inputs.keys()), \
        'Demo supports only topologies with the following input keys: {}'.format(', '.join(required_input_keys))
    required_output_keys = {'boxes', 'scores', 'classes', 'raw_masks'}
    assert required_output_keys.issubset(net.outputs.keys()), \
        'Demo supports only topologies with the following output keys: {}'.format(', '.join(required_output_keys))

    n, c, h, w = net.inputs['im_data'].shape
    assert n == 1, 'Only batch 1 is supported by the demo application'

    log.info('Loading IR to the plugin...')
    exec_net = plugin.load(network=net, num_requests=2)
    del net

    tracker = None
    assert (args.video is None) != (args.images is None), \
        'Please specify either --video or --images command line argument'
    if args.video is not None:
        try:
            video = int(args.video)
        except ValueError:
            video = args.video
        log.info('Using video "{}" as an image source...'.format(video))
        cap = cv2.VideoCapture(video)
        cap.set(cv2.CAP_PROP_BUFFERSIZE, 1)
        delay = 1
        tracker = StaticIOUTracker()
    if args.images is not None:
        log.info('Using images "{}" as an image source...'.format(args.images))
        cap = ImagesCapture(args.images, skip_non_images=True)
        delay = 0

    with open(args.labels, 'rt') as labels_file:
        class_labels = labels_file.read().splitlines()

    visualizer = Visualizer(class_labels, show_boxes=args.show_boxes, show_scores=args.show_scores)

    log.info('Starting inference...')
    log.info('To stop the demo execution press Esc button')
    render_time = 0

    print("To close the application, press 'CTRL+C' or any key with focus on the output window")
    while cap.isOpened():
        ret, frame = cap.read()
        if not ret:
            break

        # Resize the image to leave the same aspect ratio and to fit it to a window of a target size.
        scale = min(h / frame.shape[0], w / frame.shape[1])
        input_image = cv2.resize(frame, None, fx=scale, fy=scale)
        input_image_size = input_image.shape[:2]
        input_image = np.pad(input_image, ((0, h - input_image_size[0]),
                                           (0, w - input_image_size[1]),
                                           (0, 0)),
                             mode='constant', constant_values=0)
        # Change data layout from HWC to CHW.
        input_image = input_image.transpose((2, 0, 1))
        input_image = input_image.reshape((n, c, h, w)).astype(np.float32)
        input_image_info = np.asarray([[input_image_size[0], input_image_size[1], scale]], dtype=np.float32)

        # Run the net.
        inf_start = time.time()
        outputs = exec_net.infer({'im_data': input_image, 'im_info': input_image_info})
        inf_end = time.time()
        det_time = inf_end - inf_start

        # Parse detection results of the current request
        boxes = outputs['boxes'] / scale
        scores = outputs['scores']
        classes = outputs['classes'].astype(np.uint32)
        masks = []
        for box, cls, raw_mask in zip(boxes, classes, outputs['raw_masks']):
            raw_cls_mask = raw_mask[cls, ...]
            mask = segm_postprocess(box, raw_cls_mask, frame.shape[0], frame.shape[1])
            masks.append(mask)

        # Filter out detections with low confidence.
        detections_filter = scores > args.prob_threshold
        scores = scores[detections_filter]
        classes = classes[detections_filter]
        boxes = boxes[detections_filter]
        masks = list(segm for segm, is_valid in zip(masks, detections_filter) if is_valid)

        render_start = time.time()

        # Get instance track IDs.
        masks_tracks_ids = None
        if tracker is not None:
            masks_tracks_ids = tracker(masks, classes)

        # Visualize masks.
        frame = visualizer(frame, boxes, classes, scores, masks, masks_tracks_ids)

        # Draw performance stats.
        inf_time_message = 'Inference time: {:.3f} ms'.format(det_time * 1000)
        render_time_message = 'OpenCV rendering time: {:.3f} ms'.format(render_time * 1000)
        cv2.putText(frame, inf_time_message, (15, 15), cv2.FONT_HERSHEY_COMPLEX, 0.5, (200, 10, 10), 1)
        cv2.putText(frame, render_time_message, (15, 30), cv2.FONT_HERSHEY_COMPLEX, 0.5, (10, 10, 200), 1)

        # Print performance counters.
        if args.perf_counts:
            perf_counts = exec_net.requests[0].get_perf_counts()
            log.info('Performance counters:')
            print('{:<70} {:<15} {:<15} {:<15} {:<10}'.format('name', 'layer_type', 'exet_type', 'status',
                                                              'real_time, us'))
            for layer, stats in perf_counts.items():
                print('{:<70} {:<15} {:<15} {:<15} {:<10}'.format(layer, stats['layer_type'], stats['exec_type'],
                                                                  stats['status'], stats['real_time']))

        # Show resulting image.
        cv2.imshow('Results', frame)
        render_end = time.time()
        render_time = render_end - render_start

        key = cv2.waitKey(delay)
        esc_code = 27
        if key == esc_code:
            break

    cv2.destroyAllWindows()
    cap.release()
    del exec_net
    del plugin


if __name__ == '__main__':
    sys.exit(main() or 0)

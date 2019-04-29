from collections import deque
from itertools import cycle

import cv2
import numpy as np
from openvino.inference_engine import IENetwork


def center_crop(frame, crop_size):
    img_h, img_w, _ = frame.shape

    x0 = int(round((img_w - crop_size[0]) / 2.))
    y0 = int(round((img_h - crop_size[1]) / 2.))
    x1 = x0 + crop_size[0]
    y1 = y0 + crop_size[1]

    return frame[y0:y1, x0:x1, ...]


def adaptive_resize(frame, dst_size):
    h, w, _ = frame.shape
    scale = dst_size / min(h, w)
    ow, oh = int(w * scale), int(h * scale)

    if ow == w and oh == h:
        return frame
    return cv2.resize(frame, (ow, oh))


def preprocess_frame(frame):
    frame = adaptive_resize(frame, 224)
    frame = center_crop(frame, (224, 224))

    frame = frame.transpose((2, 0, 1))  # HWC -> CHW
    frame = frame[np.newaxis, ...]  # add batch dimension
    return frame


class AsyncWrapper:
    def __init__(self, ie_model, num_requests):
        self.net = ie_model
        self.num_requests = num_requests

        self._result_ready = False
        self._req_ids = cycle(range(num_requests))
        self._result_ids = cycle(range(num_requests))
        self._frames = deque(maxlen=num_requests)

    def infer(self, model_input, frame=None):
        """Schedule current model input to infer, return last result"""
        next_req_id = next(self._req_ids)
        self.net.async_infer(model_input, next_req_id)

        last_frame = self._frames[0] if self._frames else frame

        self._frames.append(frame)
        if next_req_id == self.num_requests - 1:
            self._result_ready = True

        if self._result_ready:
            result_req_id = next(self._result_ids)
            result = self.net.wait_request(result_req_id)
            return result, last_frame
        else:
            return None, None


class IEModel:
    def __init__(self, model_xml, model_bin, ie_plugin, num_requests, batch_size=1):
        # Plugin initialization for specified device and load extensions library if specified
        self.plugin = ie_plugin

        # Read IR
        print("Reading IR...")
        self.net = IENetwork(model_xml, model_bin)
        self.net.batch_size = batch_size
        assert len(self.net.inputs.keys()) == 1, "One input is expected"
        assert len(self.net.outputs) == 1, "One output is expected"

        print("Loading IR to the plugin...")

        self.exec_net = self.plugin.load(self.net, num_requests=num_requests)
        self.input_name = next(iter(self.net.inputs))
        self.output_name = next(iter(self.net.outputs))
        self.input_size = self.net.inputs[self.input_name]
        self.output_size = self.exec_net.requests[0].outputs[self.output_name].shape
        self.num_requests = num_requests

    def infer(self, frame):
        input_data = {self.input_name: frame}
        infer_result = self.exec_net.infer(input_data)
        return infer_result[self.output_name]

    def async_infer(self, frame, req_id):
        input_data = {self.input_name: frame}
        self.exec_net.start_async(request_id=req_id, inputs=input_data)
        pass

    def wait_request(self, req_id):
        self.exec_net.requests[req_id].wait()
        return self.exec_net.requests[req_id].outputs[self.output_name]


class ActionRecognitionSequential:
    def __init__(self, encoder, decoder=None):
        self.encoder = encoder
        self.decoder = decoder

    def infer(self, input):
        if self.decoder is not None:
            embeddigns = self.encoder.infer(input[0])
            decoder_input = embeddigns.reshape(1, 16, 512)
            return self.decoder.infer(decoder_input)

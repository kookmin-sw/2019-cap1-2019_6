Action Recognition Python* Demo
===============================

This is the demo application for Action Recognition algorithm, which classifies actions that are being performed on input video.
The following pre-trained models are delivered with the product:
* `driver-action-recognition-adas-0002-encoder` + `driver-action-recognition-adas-0002-decoder`, which is a model for driver monitoring scenario. It recognizes actions like safe driving, talking to the phone and others
* `action-recognition-0001-encoder` + `action-recognition-0001-decoder`, which is a general-purpose action recognition (400 actions) model for Kinetics-400 dataset.

How It Works
------------
The demo pipeline consists of several frames, namely `Data`, `Encoder`, `Decoder` and `Render`.
Every step implements `PipelineStep` interface by creating a class derived from `PipelineStep` base class. See `steps.py` for implementation details.

- `DataStep` reads frames from the input video.
- `EncoderStep` preprocesses a frame and feeds it to the encoder model to produce a frame embedding.
- `DecoderStep` feeds embeddings produced by the `EncoderStep` to the decoder model and produces predictions.
- `RenderStep` renders prediction results.

Pipeline steps are composed in `AsyncPipeline`. Every step can be run in separate thread by adding it to the pipeline with `parallel=True` option.
When two consequent steps occur in separate threads, they communicate via message queue (for example, deliver step result or stop signal).

To ensure maximum performance, Inference Engine models are wrapped in `AsyncWrapper`
that uses Inference Engine async API by scheduling infer requests in cyclical order
(inference on every new input is started asynchronously, result of the longest working infer request is returned).
You can change the value of `num_requests` in `demo.py` to find an optimal number of parallel working infer requests for your inference accelerators
(Compute Sticks and GPUs benefit from higher number of infer requests).

> **NOTE**: By default, Inference Engine samples and demos expect input with BGR channels order. If you trained your model to work with RGB order, you need to manually rearrange the default channels order in the sample or demo application or reconvert your model using the Model Optimizer tool with `--reverse_input_channels` argument specified. For more information about the argument, refer to **When to Specify Input Shapes** section of [Converting a Model Using General Conversion Parameters](./docs/MO_DG/prepare_model/convert_model/Converting_Model_General.md).

Running
-------
Running the application with the `-h` option yields the following usage message:

```
usage: demo.py [-h] --encoder ENCODER --decoder DECODER [-v VIDEO]
               [-vl VIDEO_LIST] [-e CPU_EXTENSION] [-pp PLUGIN_DIR]
               [-d DEVICE] [--fps FPS] [-l LABELS]

Options:
  -h, --help            show this help message and exit
  --encoder ENCODER     Required. Path to encoder model
  --decoder DECODER     Required. Path to decoder model
  -v VIDEO, --video VIDEO
                        Optional. Path to a video or file. 'cam' for capturing
                        video
  -vl VIDEO_LIST, --video_list VIDEO_LIST
                        Optional. Path to a list with video files (text file,
                        one video per line)
  -e CPU_EXTENSION, --cpu_extension CPU_EXTENSION
                        Optional. For CPU custom layers, if any. Absolute path
                        to a shared library with the kernels implementation.
  -pp PLUGIN_DIR, --plugin_dir PLUGIN_DIR
                        Optional. Path to a plugin folder
  -d DEVICE, --device DEVICE
                        Optional. Specify a target device to infer on. CPU, GPU, FPGA, HDDL or MYRIAD is "
                        acceptable. The demo will look for a suitable plugin for the device specified.
                        Default value is CPU
  --fps FPS             Optional. FPS for renderer
  -l LABELS, --labels LABELS
                        Optional. Path to file with label names

```

Running the application with an empty list of options yields the usage message given above and an error message.

To run the demo, you can use public or pre-trained models. To download the pre-trained models, use the OpenVINO [Model Downloader](https://github.com/opencv/open_model_zoo/tree/2018/model_downloader) or go to [https://download.01.org/opencv/](https://download.01.org/opencv/).

> **NOTE**: Before running the demo with a trained model, make sure the model is converted to the Inference Engine format (\*.xml + \*.bin) using the [Model Optimizer tool](./docs/MO_DG/Deep_Learning_Model_Optimizer_DevGuide.md).

**For example**, to run the demo for in-cabin driver monitoring scenario, please provide a path to the encoder and decoder models, an input video and a file with label names:
```bash
python3 demo.py --encoder models/driver_action_recognition_tsd_0002_encoder.xml \
    --decoder models/driver_action_recognition_tsd_0002_decoder.xml \
    --labels driver_actions.txt
```

Demo Output
------------
The application uses OpenCV to display the real-time results and current inference performance (in FPS).

## See Also
* [Using Inference Engine Samples](./docs/IE_DG/Samples_Overview.md)
* [Model Optimizer](./docs/MO_DG/Deep_Learning_Model_Optimizer_DevGuide.md)
* [Model Downloader](https://github.com/opencv/open_model_zoo/tree/2018/model_downloader)

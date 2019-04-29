# Dynamic Batch Python* Demo

This topic demonstrates how to run the 3D Segmentation Demo, which demostrates how to set batch size dynamicly for certain infer request and check inference time difference.

## How It Works

Upon the start-up, the demo reads command-line parameters and loads a network and images to the Inference Engine plugin.

> **NOTE**: By default, Inference Engine samples and demos expect input with BGR channels order. If you trained your model to work with RGB order, you need to manually rearrange the default channels order in the sample or demo application or reconvert your model using the Model Optimizer tool with `--reverse_input_channels` argument specified. For more information about the argument, refer to **When to Specify Input Shapes** section of [Converting a Model Using General Conversion Parameters](./docs/MO_DG/prepare_model/convert_model/Converting_Model_General.md).

## Running

Run the application with the `-h` or `--help` option to see the usage message:
```
    python3 dynamic_batch_demo.py -h
```
The command yields the following usage message:
```
usage: dynamic_batch_demo.py [-h] -m MODEL -i INPUT [INPUT ...]
                             [-l CPU_EXTENSION] [-pp PLUGIN_DIR] [-d DEVICE]
                             [--labels LABELS] [-mb MAX_BATCH]
                             [-ni NUMBER_ITER] [-pc]

Options:
  -h, --help            Show this help message and exit.
  -m MODEL, --model MODEL
                        Required. Path to an .xml file with a trained model.
  -i INPUT [INPUT ...], --input INPUT [INPUT ...]
                        Required. Path to a folder with images or path to an
                        image files
  -l CPU_EXTENSION, --cpu_extension CPU_EXTENSION
                        Optional. Required for CPU custom layers. Absolute
                        path to a shared library with the kernels
                        implementations.
  -pp PLUGIN_DIR, --plugin_dir PLUGIN_DIR
                        Optional. Path to a plugin folder
  -d DEVICE, --device DEVICE
                        Optional. Specify the target device to infer on; CPU,
                        GPU, FPGA, HDDL or MYRIAD is acceptable.The demo will
                        look for a suitable plugin for device specified.
                        Default value is CPU
  --labels LABELS       Optional. Path to labels mapping file
  -mb MAX_BATCH, --max_batch MAX_BATCH
                        Optional. Set maximum batch size for the network
  -ni NUMBER_ITER, --number_iter NUMBER_ITER
                        Optional. Number of inference iterations
  -pc, --perf_counts    Optional. Report performance counters
```

To run the sample, you can use public or pre-trained models. You can download the pre-trained models with the OpenVINO [Model Downloader](https://github.com/opencv/open_model_zoo/tree/2018/model_downloader) or from [https://download.01.org/opencv/](https://download.01.org/opencv/).

> **NOTE**: Before running the sample with a trained model, make sure the model is converted to the Inference Engine format (\*.xml + \*.bin) using the [Model Optimizer tool](./docs/MO_DG/Deep_Learning_Model_Optimizer_DevGuide.md).


For example, to do inference on images using a trained network with multiple outputs on CPU device with supported dynamic batch setting, run the following command:

```
    python3 dynamic_batch_demo.py -i <path_to_images> -m <path_to_model> -d CPU -mb <max_batch_size>
```
     
## Demo Output
The demo outputs a DOT file with a dumped graph.

## See Also
* [Using Inference Engine Samples](./docs/IE_DG/Samples_Overview.md)
* [Model Optimizer tool](./docs/MO_DG/Deep_Learning_Model_Optimizer_DevGuide.md)
* [Model Downloader](https://github.com/opencv/open_model_zoo/tree/2018/model_downloader)
* [Dynamic Batching](./docs/IE_DG/DynamicBatching.md)

# Affinity Settings Python* Demo

This topic demonstrates how to run the Affinity Settings Demo, which ofloads convolution layers execution to GPU device..

## How It Works

Upon the start-up, the demo reads command-line parameters and loads a network and images to the Inference Engine plugin.

> **NOTE**: By default, Inference Engine samples and demos expect input with BGR channels order. If you trained your model to work with RGB order, you need to manually rearrange the default channels order in the sample or demo application or reconvert your model using the Model Optimizer tool with `--reverse_input_channels` argument specified. For more information about the argument, refer to **When to Specify Input Shapes** section of [Converting a Model Using General Conversion Parameters](./docs/MO_DG/prepare_model/convert_model/Converting_Model_General.md).

## Running

Run the application with the `-h` or `--help` option to see the usage message:
```
    python3 affinity_setting_demo.py -h
```
The command yields the following usage message:
```
usage: affinity_setting_demo.py [-h] -m MODEL -i INPUT [-l CPU_EXTENSION]
                                [-pp PLUGIN_DIR] [-d DEVICE] [-nt NUMBER_TOP]

Options:
  -h, --help            Show this help message and exit.
  -m MODEL, --model MODEL
                        Required. Path to an .xml file with a trained model.
  -i INPUT, --input INPUT
                        Required. Path to a folder with images or path to an
                        image files
  -l CPU_EXTENSION, --cpu_extension CPU_EXTENSION
                        Optional. Required for CPU custom layers. Absolute
                        path to a shared library with the kernels
                        implementations.
  -pp PLUGIN_DIR, --plugin_dir PLUGIN_DIR
                        Optional. Path to a plugin folder
  -d DEVICE, --device DEVICE
                        Optional. Specify HETERO plugin configuration; for
                        example, HETERO:FPGA,CPU
  -nt NUMBER_TOP, --number_top NUMBER_TOP
                        Optional. Number of top results

```

Running the application with the empty list of options yields the usage message given above.

To run the sample, you can use public or pre-trained models. You can download the pre-trained models with the OpenVINO [Model Downloader](https://github.com/opencv/open_model_zoo/tree/2018/model_downloader) or from [https://download.01.org/opencv/](https://download.01.org/opencv/).

> **NOTE**: Before running the sample with a trained model, make sure the model is converted to the Inference Engine format (\*.xml + \*.bin) using the [Model Optimizer tool](./docs/MO_DG/Deep_Learning_Model_Optimizer_DevGuide.md).


For example, to do inference on image using a trained network, run the following command:

```
    python3 affinity_setting_demo.py -i <path_to_image>/inputImage.tiff -m <path_to_model>/multiple-output.xml -d CPU -o <path_to_output>
```
     
## Demo Output
The demo outputs a multipage TIFF image and a NIFTI archive.

## See Also
* [Using Inference Engine Samples](./docs/IE_DG/Samples_Overview.md)
* [Model Optimizer tool](./docs/MO_DG/Deep_Learning_Model_Optimizer_DevGuide.md)
* [Model Downloader](https://github.com/opencv/open_model_zoo/tree/2018/model_downloader)



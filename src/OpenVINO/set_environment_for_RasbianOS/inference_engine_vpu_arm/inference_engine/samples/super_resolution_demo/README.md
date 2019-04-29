# Super Resolution C++ Demo

This topic demonstrates how to run Super Resolution demo application, which
reconstructs the high resolution image from the original low resolution one.
You can use the following pre-trained model with the demo:

* `single-image-super-resolution-1033`, which is the primary and only model that
  performs super resolution 4x upscale on a 200x200 image

For more information about the pre-trained models, refer to the [Open Model Zoo](https://github.com/opencv/open_model_zoo/blob/master/intel_models/index.md) repository on GitHub*.

## How It Works

On the start-up, the application reads command-line parameters and loads the
specified network. After that, the application reads a 200x200 input image and
performs 4x upscale using super resolution.

> **NOTE**: By default, Inference Engine samples and demos expect input with BGR channels order. If you trained your model to work with RGB order, you need to manually rearrange the default channels order in the sample or demo application or reconvert your model using the Model Optimizer tool with `--reverse_input_channels` argument specified. For more information about the argument, refer to **When to Specify Input Shapes** section of [Converting a Model Using General Conversion Parameters](./docs/MO_DG/prepare_model/convert_model/Converting_Model_General.md).

## Running

Running the application with the `-h` option yields the following usage message:
```sh
./super_resolution_demo -h
InferenceEngine:
    API version ............ <version>
    Build .................. <number>

super_resolution_demo [OPTION]
Options:

    -h                      Print a usage message.
    -i "<path>"             Required. Path to an image.
    -m "<path>"             Required. Path to an .xml file with a trained model.
    -pp "<path>"            Optional. Path to a plugin folder.
    -d "<device>"           Optional. Specify the target device to infer on (CPU, GPU, FPGA, HDDL or MYRIAD). The demo will look for a suitable plugin for the specified device.
    -ni "<integer>"         Optional. Number of iterations. Default value is 1
    -pc                     Optional. Enable per-layer performance report

```

Running the application with the empty list of options yields the usage message given above and an error message.

To run the sample, you can use public or pre-trained models. To download the pre-trained models, use the OpenVINO [Model Downloader](https://github.com/opencv/open_model_zoo/tree/2018/model_downloader) or go to [https://download.01.org/opencv/](https://download.01.org/opencv/).

> **NOTE**: Before running the sample with a trained model, make sure the model is converted to the Inference Engine format (\*.xml + \*.bin) using the [Model Optimizer tool](./docs/MO_DG/Deep_Learning_Model_Optimizer_DevGuide.md).

To do inference on CPU using a trained model, run the following command:

```sh
./super_resolution_demo -i <path_to_image>/image.bmp -m <path_to_model>/model.xml
```

## Demo Output

The application outputs a reconstructed high-resolution image and saves it in
the current working directory as `*.bmp` file with `sr` prefix.

## See Also
* [Using Inference Engine Samples](./docs/IE_DG/Samples_Overview.md)
* [Model Optimizer](./docs/MO_DG/Deep_Learning_Model_Optimizer_DevGuide.md)
* [Model Downloader](https://github.com/opencv/open_model_zoo/tree/2018/model_downloader)

# Neural Compute Stick2를 사용하여 Raspberry Pi에서 YOLO 실행하기

 라즈베리파이 환경에서 대표적인 단일 단계 방식의 객체 탐지 모델인 YOLO를 통해 객체 인식 및 추적을 구현한다. 그러나 Darknet 프레임워크는 라즈베리파이에서 실행 시 전력 소모가 크고 요구 사양이 높아 딥러닝 모델을 정상적으로 실행하지 못한다.
    
 따라서 Movidius™ VPU(비주얼 처리 장치)에 의해 저전력, 고성능으로 구동되는 딥러닝용 USB 드라이브인 Intel® Movidius™ NCS2(Neural Compute Stick 2)를 사용한다. 이 USB는 개방형 시각 추론 및 신경망 최적화 도구 모음인 OpenVINO라는 Toolkit을 제공한다.
    
 Raspberry Pi에서 NCS2를 사용하기 위해서는, 다음과 같은 과정이 필요하다. 

***
### Dependency : Raspberry Pi 3 B+, Neural Compute Stick2
***

## 1. Raspberry Pi에서 OpenVINO tool 설치 및 USB 규칙 설정

Raspberry Pi에서 NCS2를 사용하기 위해서는 OpenVINO 도구를 설치한 후 환경 변수 및 USB 규칙을 설정해주어야 한다. 아래 링크를 참고하여 OpenVINO tool **2019 R1** 버전을 설치한다.

참고 :
- [Install OpenVINO™ toolkit for Raspbian* OS](https://docs.openvinotoolkit.org/latest/_docs_install_guides_installing_openvino_raspbian.html)
***

## 2. Ubuntu 16.04(LTS)에서 OpenVINO tool 설치

우분투에서 OpenVINO tool를 추가적으로 설치하는 이유는 3번 과정인 Model Optimizer 작업을 수행하기 위해서이다. 아래 링크를 참고하여 OpenVINO tool **2018 R5** 버전을 설치한다.

참고 :
- [Install Intel® Distribution of OpenVINO™ toolkit for Linux*](https://docs.openvinotoolkit.org/latest/_docs_install_guides_installing_openvino_linux.html)
***

## 3. Model Optimizer(모델 최적화)

Darknet에서 제공하는 학습 도구 Yolo Mark를 사용하여 학습 모델 파일(.weight)을 생성한다. 여기서 학습된 모델 파일은 NCS2 USB를 사용할 수 있는 OpenVINO 도구에서 지원하지 않기 때문에 실행되지 않는다. 따라서 OpenVINO에서 사용할 수 있도록 최적화된 학습 모델로 변환하는 작업이 필요하다. 

Model Optimizer 작업은 Darknet 모델을 Tensorflow 모델로 변환한 다음에 IR(중간표현)로 다시 변환하는 과정을 거쳐야 한다.

1. .weight 파일 -> .pb 파일 변환

아래 주소를 clone하여 로컬에 다운받는다.
```markdown
git clone https://github.com/mystic123/tensorflow-yolo-v3
```

학습시킨 클래스의 coco.names 파일에 이름을 적어준다.
```markdown
extinguisher
bollard
stair
```

names파일과 weights파일의 경로를 아래와 같이 {} 안에 넣고 convert_weights_pb.py 파일을 실행한다.
```markdown
python3 convert_weights_pb.py --class_names {coco.names} --data_format NHWC --weights_file {yolov3-tiny.weights} --tiny
```

2. .pb 파일 -> .xml, .bin, .mapping 파일 변환

Tiny-Yolo v3를 사용하기 때문에 <OPENVINO_INSTALL_DIR>/deployment_tools/model_optimizer/extensions/front/tf/ 경로에서 yolo_v3_tiny.json을 만들고 class의 갯수를 적절히 수정해준다.
```markdown
[
  {
    "id": "TFYOLOV3",
    "match_kind": "general",
    "custom_attributes": {
      "classes": 4,
      "coords": 4,
      "num": 6,
      "mask": [0,1,2],
      "entry_points": ["detector/yolo-v3-tiny/Reshape","detector/yolo-v3-tiny/Reshape_4"]
    }
  }
]
```

<OPENVINO_INSTALL_DIR>/deployment_tools/model_optimizer/ 경로에서 1번 과정을 통해 만든 pb 파일의 경로를 아래와 같이 {} 안에 넣고 mo_tf.py 파일을 실행한다. 
```markdown
python mo_tf.py --input_model {frozen_darknet_yolov3_model.pb} --tensorflow_use_custom_operations_config {yolo_v3_tiny.json} --data_type=FP16 --input_shape=[1,416,416,3] 
```
또는
```markdown
python mo_tf.py --input_model {frozen_darknet_yolov3_model.pb} --tensorflow_use_custom_operations_config {yolo_v3_tiny.json} --data_type=FP16 --batch 1
```

이 작업을 정상적으로 수행하게 되면 .xml, .bin, .mapping 총 3개의 파일이 만들어진다. 

참고 : 
- [OpenVINO-ModelOptimizer](https://software.intel.com/en-us/articles/OpenVINO-ModelOptimizer)
- [OpenVINO-Using-TensorFlow](https://software.intel.com/en-us/articles/OpenVINO-Using-TensorFlow)
- [Converting YOLO* Models to the Intermediate Representation (IR)](https://docs.openvinotoolkit.org/latest/_docs_MO_DG_prepare_model_convert_model_tf_specific_Convert_YOLO_From_Tensorflow.html)
***

## 4. OpenVINO에서 제공하는 YOLO 알고리즘 적용

Raspberry Pi 를 실행하여 아래 주소를 clone하여 다운받는다.
```markdown
git clone https://github.com/PINTO0309/OpenVINO-YoloV3
```

openvino_tiny-yolov3_MultiStick_test.py 파일을 열어서 우리가 학습시킨 모델에 맞춰서 코드를 수정한다.
```markdown
classes = 4
coords = 4
num = 3
anchors = [10,14, 23,27, 37,58, 81,82, 135,169, 344,319]

LABELS = ("extinguister", "bollard", "stair", "finger")
```

3번 과정을 통해 만든 xml, bin 파일의 경로를 아래와 같이 넣어준다. (xml, bin, mapping 총 3개의 파일은 같은 경로에 있어야한다.)
```markdown
class NcsWorker(object):

    def __init__(self, devid, frameBuffer, results, camera_width, camera_height, number_of_ncs, vidfps):
      ...
        self.model_xml = "/home/pi/Downloads/OpenVINO-YoloV3/frozen_tiny_yolo_v3.xml"
        self.model_bin = "/home/pi/Downloads/OpenVINO-YoloV3/frozen_tiny_yolo_v3.bin"
      ...
```

라즈베리파이에 NCS2 USB를 연결하고 그 갯수만큼 옵션에 넣어서 openvino_tiny-yolov3_MultiStick_test.py 파일을 실행한다.
```markdown
python3 openvino_tiny-yolov3_MultiStick_test.py -numncs 2 
(2개를 연결했을 경우이다. Default는 1이므로 1개를 연결했을 때는 옵션 없이 실행해도 된다.)
```


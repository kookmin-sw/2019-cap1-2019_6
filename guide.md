# YOLO 모델에 데이터 학습시키기

사용자가 데이터를 추가적으로 학습시키기 원한다면, 다음과 같은 과정이 필요하다. (학습을 진행한 환경은 Ubuntu 16.04 LTS이다.)
***

## 1. 데이터 수집 

학습시키기 원하는 object를 카메라를 이용하여 여러 장 촬영한다. 이때, **배경, 각도**를 다양하게 하여 촬영을 하면 더 높은 인식률을 기대할 수 있다.
수집해야 할 이미지의 양은 object의 종류에 따라서 달라지는데, 보통 한 object당 100장에서 1500장까지 수집한다.
***

## 2. Yolo mark를 통한 데이터 전처리 

Yolo mark는 촬영된 이미지 데이터에서 인식하고자 하는 object의 영역에 대한 boundary의 정보를 추출해주는 프로그램이다. 
해당 명령어를 terminal에서 실행하면, Yolo mark가 설치된다. 
```markdown
git clone https://github.com/AlexeyAB/Yolo_mark
```
설치가 완료되면 yolo-mark 디렉토리로 이동 후 make한다.
다음 명령어를 순서대로 terminal에 입력한다.
```markdown
cmake .
make
```
이제 이미지 데이터의 object 정보를 추출하는 라벨링 작업을 진행한다.
```markdown
yolo_mark/x64/Release/data/img
```
다음 경로에 기존에 있던 이미지 파일들을 삭제한다. 그리고 해당 폴더에 새로 수집한 이미지 파일(JPG)을 넣어준다.
그 뒤, yolo_mark/x64/Release/data/ 경로에 있는 obj.data 파일을 수정한다.
```markdown
yolo_mark/x64/Release/data
vi obj.data
```
obj.data 파일을 열게 되면, 다음과 같은 정보가 나오게 된다.
```markdown
classes= 2
train = data/train.txt
valid = data/train.txt
names = data/obj.names
backup = backup/
```
여기서, classes는 분류하고자 하는 object의 종류의 수를 의미한다. 본 프로젝트에서는 볼라드, 계단, 소화기, 손가락을 학습시켰으므로, classes= 4로 설정하였다. 나머지 부분은 그대로 두고 수정한 내용을 저장한다.
***
다음에는, obj.names 파일을 수정한다.
```markdown
yolo_mark/x64/Release/data
vi obj.names
```
obj.names 파일을 열게 되면, 다음과 같은 정보가 나온다.
```markdown
airplane
bird
```
해당 파일은 분류할 object에 대해 각각 이름을 지정해준다. object id는 위부터 0, 1, 2 ... 순으로 매핑된다. 해당 폴더의 내용을 수정한 후 저장한다. 
그 다음, 다시 yolo_mark 디렉토리로 이동하여 yolo mark를 실행시킨다.
```markdown
./linux_mark.sh
```
이제부터 bounding box를 직접 마우스로 그려넣어 라벨링을 진행한다.
<라벨링 사진>
라벨링을 하면, jpg 파일 옆에 txt 파일이 생성된다. 해당 txt 파일에는 object id와 bounding box의 중심 좌표값과 너비, 높이가 기록된다.

## 3. 데이터 학습 진행



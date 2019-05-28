# OpenCV의 HarrCascade를 이용한 손가락 학습 

특징점을 이용한 학습을 통해 오브젝트 디텍션이 가능하다. 

*** 
## 1. 샘플 수집하기
1. 긍정이미지(본 프로젝트에서는 손가락)와 부정이미지(손가락 외)를 넣을 폴더를 각각 만든다.
2. positive 폴더에 검출하고 싶은 샘플 이미지들을 넣는다.
***

## 2. 샘플링
1. objectmarker.exe 실행한다.
2. 샘플 이미지가 뜨면 마우스로 네모를 그려 대상을 선택한다.
3. 스페이스바를 누르면 대상이 기록된다.
4. 현재 이미지에서 모든 대상을 기록 하였으면 엔터를 눌러 다음 이미지로 넘어간다.
5. 모든 샘플 이미지에 위의 과정을 반복한다. 
***

## 3. vec 파일 생성
1. opencv_createsamples.bat 파일을 실행한다.
2. data 폴더가 생성되고 positives.vec 파일이 생성된다.
3. opencv_createsamples.bat 파일을 실행하면 샘플들을 미리 볼 수 있다. 
***

## 4. Haar 학습 
1. negative 폴더에 학습에 사용할 오답 이미지들을 넣는다. (샘플링 이미지와 1대3 비율이 가장 적합)
2. create_list.bat 파일을 실행하여 infofile.txt 파일을 생성한다.
3. opencv_haartraining.bat 파일을 메모장으로 열어 옵션을 수정한다.
		-npos [positive 이미지 갯수] -nneg [negative 이미지 갯수]
4. opencv_haartraining.bat 파일을 실행한다.
5. data\cascade 폴더가 생성되어 학습 결과가 저장된다.
***

## 5. XML 파일 생성
1. haarconv.bat 파일을 실행한다.
2. output.xml 파일이 생성된다.
***

참고 :
- [Tutorial: OpenCV haar training](http://note.sonots.com/SciSoftware/haartraining.html)
- [Tutorial: Opencv Haar/cascade training](https://darkpgmr.tistory.com/70)
***

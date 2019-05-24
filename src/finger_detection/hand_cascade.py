import cv2
import numpy as np

#웹캠에서 영상을 읽어온다
cap = cv2.VideoCapture(0)
cap.set(3, 640) #WIDTH
cap.set(4, 480) #HEIGHT

#손 인식 캐스케이드 파일 읽는다
hand_cascade = cv2.CascadeClassifier('hand.xml')

#잘라서 빈이미지랑 붙인 후 저장
def im_trim(img, y):
    zero = np.zeros((768-y, 1280, 3), np.uint8)
    img_trim = img[0:y]
    addv = cv2.vconcat([img_trim, zero])
    resize = cv2.resize(addv, (1024, 768))
    cv2.imwrite('img_to_read.jpg', resize)

while(True):
    # frame 별로 capture 한다
    ret, frame = cap.read()

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    hands = hand_cascade.detectMultiScale(gray, 1.3, 5)

    #인식된 손 갯수를 출력
    print(len(hands))

    # 인식된 손에 사각형을 출력한다
    for (x,y,w,h) in hands:
         cv2.rectangle(frame,(x,y),(x+w,y+h),(255,0,0),2)
         im_trim(frame, y)

    #화면에 출력한다
    cv2.imshow('frame',frame)
    if cv2.waitKey(1) & 0xFF == ord('q'): break

cap.release()
cv2.destroyAllWindows()
#!/usr/bin/etc python
# 특징점 - hog
# 분류기 - svm, knn

import cv2
import numpy as np
#import pytesseract
from  PIL import Image
import os
from os.path import *

class Recognition:
     def ExtractNumber(self):
          Number='testimg3.jpg'
          img=cv2.imread(Number,cv2.IMREAD_COLOR)
          copy_img=img.copy()
          img2=cv2.cvtColor(img,cv2.COLOR_BGR2GRAY) # gray 이미지로 변환

          #cv2.imwrite('gray.jpg',img2) # img2를 gray.jpg 라는 이미지로 저장
          blur = cv2.GaussianBlur(img2,(3,3),0)
          #cv2.imwrite('blur.jpg',blur) # blur이미지로 저장
          canny=cv2.Canny(blur,100,200) # 엣지 검출
          #cv2.imwrite('canny.jpg',canny)
          contours,hierarchy  = cv2.findContours(canny, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

          box1=[]
          f_count=0
          select=0
          plate_width=0

          for i in range(len(contours)):
               cnt=contours[i]
               area = cv2.contourArea(cnt)
               x,y,w,h = cv2.boundingRect(cnt)
               rect_area=w*h  #area size
               aspect_ratio = float(w)/h
               if  (rect_area>=100) and (rect_area<=700): # 여기 수정해야됌
                    cv2.rectangle(img,(x,y),(x+w,y+h),(0,255,0),1)
                    box1.append(cv2.boundingRect(cnt))
                    #print(rect_area)
          #print(len(box1))
          #cv2.imshow('snake',img)
          #cv2.waitKey(0)

          # 특징점 검출
          # 분류기
          samples = []
          labels = []
          positive_path = '/home/byunghoonpark/finger_detection/positive/'
          negative_path = '/home/byunghoonpark/finger_detection/negative/'
          positive_list = os.listdir(positive_path)
          negative_list = os.listdir(negative_path)
          positive_list.sort()
          negative_list.sort()
          # Get positive samples
          for filename in positive_list:
              img = cv2.imread(filename, 1)
              #hist = hog(img)
              samples.append(hist)
              labels.append(1)

          # Get negative samples
          for filename in negative_list:
              img = cv2.imread(filename, 1)
              #hist = hog(img)
              samples.append(hist)
              labels.append(0)

          # Convert objects to Numpy Objects
          samples = np.float32(samples)
          labels = np.array(labels)

          # Shuffle Samples
          rand = np.random.RandomState(321)
          shuffle = rand.permutation(len(samples))
          samples = samples[shuffle]
          labels = labels[shuffle]

          # Create SVM classifier
          svm = cv2.ml.SVM_create()
          svm.setType(cv2.ml.SVM_C_SVC)
          svm.setKernel(cv2.ml.SVM_RBF) # cv2.ml.SVM_LINEAR
          # svm.setDegree(0.0)
          svm.setGamma(5.383)
          # svm.setCoef0(0.0)
          svm.setC(2.67)

          # Train
          svm.train(samples, cv2.ml.ROW_SAMPLE, labels)
          svm.save('svm_data.dat')


          '''
          for i in range(len(box1)): ##Buble Sort on python
               for j in range(len(box1)-(i+1)):
                    if box1[j][0]>box1[j+1][0]:
                         temp=box1[j]
                         box1[j]=box1[j+1]
                         box1[j+1]=temp

         for m in range(len(box1)):
               count=0
               for n in range(m+1,(len(box1)-1)):
                    delta_x=abs(box1[n+1][0]-box1[m][0])
                    if delta_x > 150:
                         break
                    delta_y=abs(box1[n+1][1]-box1[m][1])
                    if delta_x ==0:
                         delta_x=1
                    if delta_y ==0:
                         delta_y=1
                    gradient =float(delta_y) /float(delta_x)
                    if gradient<0.25:
                        count=count+1
               #measure number plate size
               if count > f_count:
                    select = m
                    f_count = count;
                    plate_width=delta_x

          cv2.imwrite('snake.jpg',img)
          #cv2.imshow('snake',img)
          #cv2.waitKey(0)
          '''

          '''
          number_plate=copy_img[box1[select][1]-10:box1[select][3]+box1[select][1]+20,box1[select][0]-10:140+box1[select][0]]
          resize_plate=cv2.resize(number_plate,None,fx=1.8,fy=1.8,interpolation=cv2.INTER_CUBIC+cv2.INTER_LINEAR)
          plate_gray=cv2.cvtColor(resize_plate,cv2.COLOR_BGR2GRAY)
          ret,th_plate = cv2.threshold(plate_gray,150,255,cv2.THRESH_BINARY)
          cv2.imwrite('plate_th.jpg',th_plate)
          kernel = np.ones((3,3),np.uint8)
          er_plate = cv2.erode(th_plate,kernel,iterations=1)
          er_invplate = er_plate
          cv2.imwrite('er_plate.jpg',er_invplate)
          result = pytesseract.image_to_string(Image.open('er_plate.jpg'), lang='kor')
          return(result.replace(" ",""))
          '''

recogtest=Recognition()
result=recogtest.ExtractNumber()
#print(result)

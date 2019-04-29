import cv2 as cv
import numpy as np

img_color = cv.imread('fingerpoint.png')
img_gray = cv.cvtColor(img_color, cv.COLOR_BGR2GRAY)
ret, img_binary = cv.threshold(img_gray, 127, 255, 0)
_, contours, hierarchy = cv.findContours(img_binary, cv.RETR_LIST, cv.CHAIN_APPROX_SIMPLE)

cnt = contours[9]
cv.drawContours(img_color, [cnt], 0, (255, 0, 0), 3)  # blue

x, y, w, h = cv.boundingRect(cnt)
cv.rectangle(img_color, (x, y), (x+w, h+y), (255, 0, 0), 2)

print('x:', x,'y:', y)
cv.circle(img_color, (x, y), 10, (0,0,255), -1)

cv.imshow("result", img_color)
cv.waitKey(0)

cv.imwrite('savedimage.jpg',img_binary)


#for cnt in contours:
#    hull = cv.convexHull(cnt)
#    cv.drawContours(img_color, [hull], 0, (255, 0, 255), 5)

#cv.imshow("result", img_color)
#cv.waitKey(0)


#for cnt in contours:
#    M = cv.moments(cnt)
#    if M['m00'] != 0:
#        cx = int(M['m10']/M['m00'])
#        cy = int(M['m01']/M['m00'])

#    cv.circle(img_color, (cx, cy), 10, (0,0,255), -1)
#    print (cx,cy)

#cv.imshow("result", img_color)
#cv.waitKey(0)
import numpy as np
import cv2

#잘라서 빈이미지랑 붙인 후 저장
def im_trim(img, y):
    zero = np.zeros((768-y, 1280, 3), np.uint8)
    img_trim = img[0:y]
    addv = cv2.vconcat([img_trim, zero])
    resize = cv2.resize(addv, (1024, 768))
    cv2.imwrite('img_to_read.jpg', resize)
#    resize = cv2.resize(img_trim,(1024,768))
#    cv2.imwrite('boundary!!.jpg', resize)

def nothing(x):
    pass

cam = cv2.VideoCapture(0)
cam.set(cv2.CAP_PROP_FRAME_WIDTH, 1024)
cam.set(cv2.CAP_PROP_FRAME_HEIGHT, 768)

cv2.namedWindow("hand")
cv2.createTrackbar("hue_lower", "hand", 0, 255, nothing)  # 1) Creating trackbar for lower hue value so as to find the desired colored object in frame.
cv2.createTrackbar("hue_upper", "hand", 30, 255, nothing)  # Creating trackbar for upper hue value for same reason as above.
cv2.createTrackbar("saturation_lower", "hand", 41, 255, nothing)  # Creating trackbar for lower saturation value for same reason as above.
cv2.createTrackbar("saturation_upper", "hand", 152, 255, nothing)  # Creating trackbar for upper saturation value for same reason as above.
cv2.createTrackbar("value_lower", "hand", 69, 255, nothing)  # Creating trackbar for lower value for same reason as above.
cv2.createTrackbar("value_upper", "hand", 220, 255, nothing)  # Creating trackbar for upper value for same reason as above.
#out = cv2.VideoWriter('video2.avi', -1, 20.0, (640, 480))

while (1):

    ret, frame = cam.read()

    # cv2.imshow("frame",frame)
    frame_hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    hl = cv2.getTrackbarPos("hue_lower", "hand")
    hu = cv2.getTrackbarPos("hue_upper", "hand")
    sl = cv2.getTrackbarPos("saturation_lower", "hand")
    su = cv2.getTrackbarPos("saturation_upper", "hand")
    vl = cv2.getTrackbarPos("value_lower", "hand")
    vu = cv2.getTrackbarPos("value_upper", "hand")
    hand_lower = np.array([hl, sl, vl])
    hand_upper = np.array([hu, su, vu])
    mask = cv2.inRange(frame_hsv, hand_lower, hand_upper)
    ret, mask = cv2.threshold(mask, 127, 255, 0)  # Region lying in HSV range of hand_lower and hand_upper has intensity : 255, rest 0
    kernel = np.ones((7, 7), np.uint8)
    mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel)  # Performing Open operation (Increasing the white portion) to remove the noise from image
    mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel)  # Performing the Close operation (Decreasing the white portion)
    mask = cv2.bilateralFilter(mask, 5, 75, 75)  # Applying bilateral filter to further remove noises in image and keeping the boundary of hands sharp.
    #cv2.imshow("image2",mask)
    contours, _ = cv2.findContours(mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)  # finding the approximate contours of all closed objects in image
    drawing = np.zeros(frame.shape, np.uint8)
    max = 0
    ci = 0
    for i in range(len(contours)):
        cnt = contours[i]  # Finding the contour with maximum size. (hand when kept considerably closer to webcam in comparison to face.
        area = cv2.contourArea(cnt)
        if area > max:
            max = area
            ci = i

    if len(contours) == 0:
        continue
    cnt = contours[ci]  # cnt is the largest contour
    epsilon = 0.5 * cv2.arcLength(cnt, True)  # Further trying to better approximate the contour by making edges sharper and using lesser number of points to approximate contour cnt.
    approx = cv2.approxPolyDP(cnt, epsilon, True)
    hull = cv2.convexHull(cnt, returnPoints=True)  # Finding the convex hull of largest contour
    #cv2.drawContours(frame, [cnt], 0, (255, 0, 0), 3)  # storing the hull points and contours in "frame" image variable(matrix).
    x, y, w, h = cv2.boundingRect(cnt)
    cv2.rectangle(frame, (x, y), (x + w, h + y), (255, 0, 0), 2)
    #cv2.drawContours(frame, [hull], 0, (0, 255, 0), 3)  # (")
    im_trim(frame, y)
    hull = cv2.convexHull(cnt, returnPoints=False)
    defects = cv2.convexityDefects(cnt, hull)  # Finding the defects between cnt contour and convex hull of hand.
   # far = ()
    end = ()

    for i in range(defects.shape[0]):
        s, e, f, d = defects[i, 0]
        if d > 14000 and d < 28000:  # If normal distance between farthest point(defect) and contour is > 14000 and < 28000, it is the desired defect point.
            start = tuple(cnt[s][0])
            end = tuple(cnt[e][0])
            far = tuple(cnt[f][0])
            #cv2.circle(frame, end, 5, [0, 0, 255], -1)  # draw a circle at the defect point.

    font = cv2.FONT_HERSHEY_COMPLEX
    cv2.putText(frame, str(end), (100, 100), font, 1, (0, 0, 255), 1)

    #if len(end) > 0 :
    #    im_trim(frame, end[1])

    #out.write(frame)  # To save the video
    #cv2.namedWindow('frame', cv2.WINDOW_AUTOSIZE)
    cv2.imshow('frame', frame)
    if cv2.waitKey(1) & 0xFF == ord('q'): break

cam.release()
#out.release()
cv2.destroyAllWindows()
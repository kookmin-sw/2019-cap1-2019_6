// #include <opencv2/opencv.hpp>
// #include <iostream>
//
// using namespace cv;
// using namespace std;
//
// int main(){
//   VideoCapture cap(0);
//   Mat img;
//   if(!cap.isOPend())
//       cout << "카메라를 열 수 없습니다." << '\n';
//
//   while(1){
//       cap >> img;
//       imshow("image",img);
//       resize(img)
//   }
//
// }

#include "opencv2/opencv.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>

using namespace cv;
using namespace std;

Mat getHandMask(const Mat& frame, int minCr=128, int maxCr=170, int minCb=73, int maxCb=158){

    Mat YCrCb; //컬러 공간 변환 BGR->YCrCb
    cvtColor(frame, YCrCb, CV_BGR2YCrCb);

    //각 채널별로 분리
    vector<Mat> planes;
    split(YCrCb, planes);

    //각 채널별로 화소마다 비교
    Mat mask(frame.size(), CV_8U, Scalar(0));   //결과 마스크를 저장할 영상
    int nr = frame.rows;    //전체 행의 수
    int nc = frame.cols;

    for(int i=0; i<nr; i++){
        uchar* CrPlane=planes[1].ptr<uchar>(i);   //Cr채널의 i번째 행 주소
        uchar* CbPlane=planes[2].ptr<uchar>(i);   //Cb채널의 i번째 행 주소
        for(int j=0; j<nc; j++){
            if( (minCr < CrPlane[j]) && (CrPlane[j] <maxCr) && (minCb < CbPlane[j]) && (CbPlane[j] < maxCb) )
              mask.at<uchar>(i, j)=255;
        }
    }
    return mask;
}

Point getHandCenter(const Mat& mask, double& radius){
    Mat dst;   //거리 변환 행렬을 저장할 변수
    distanceTransform(mask, dst, CV_DIST_L2, 5);  //결과는 CV_32SC1 타입
    //거리 변환 행렬에서 값(거리)이 가장 큰 픽셀의 좌표와, 값을 얻어온다.
    int maxIdx[2];    //좌표 값을 얻어올 배열(행, 열 순으로 저장됨)
    minMaxIdx(dst, NULL, &radius, NULL, maxIdx, mask);   //최소값은 사용 X
    return Point(maxIdx[1], maxIdx[0]);
}

int getFingerCount(const Mat& mask, Point center, double radius, double scale=2.0){

    Mat cImg(mask.size(), CV_8U, Scalar(0)); //손가락 개수를 세기 위한 원 그리기
    circle(cImg, center, radius*scale, Scalar(255));
    vector< vector<Point> > contours; //원의 외곽선을 저장할 벡터
    findContours(cImg, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    if(contours.size()==0)   //외곽선이 없을 때 == 손 검출 X
      return -1;

    int fingerCount=0; //외곽선을 따라 돌며 mask의 값이 0에서 1로 바뀌는 지점 확인
    for(int i=1; i<contours[0].size(); i++){
        Point p1=contours[0][i-1];
        Point p2=contours[0][i];
        if(mask.at<uchar>(p1.y, p1.x)==0 && mask.at<uchar>(p2.y, p2.x)>1)
            fingerCount++;
    }
    return fingerCount-1; //손목과 만나는 개수 1개 제외
}

int main(int, char**){
    VideoCapture cap(0);
    Mat frame;
    if (!cap.isOpened())
         cout << "첫번째 카메라를 열 수 없습니다." << endl;
    namedWindow("camera", 1);

    while (1) {
        double radius;
        int finger_count = 0;
        cap >> frame; //웹캡으로부터 한 프레임을 읽어옴
        imshow("camera", frame);
        Mat mask = getHandMask(frame);
        //Mat image = imread("/home/byunghoonpark/finger_detection/test3.jpg");

        erode(mask, mask, Mat(3, 3, CV_8U, Scalar(1)), Point(-1, -1), 2);

        Point center = getHandCenter(mask,radius);
        finger_count = getFingerCount(mask, center, radius);
        cout << "손가락 개수: " << finger_count << endl;
        if(finger_count == -1)
          continue;

        if( 1<=finger_count && finger_count<=5 ){
          cout << "손바닥 중심점 좌표: " << center << " " << "반지름: " << radius << endl;
          resize(frame, frame, Size(100, 100), 0, 0, INTER_CUBIC);
          imwrite("test.jpg",frame);
        }
        //circle(image, center, 2, Scalar(0, 255, 0), -1); //손바닥 중심점 그리기
        //circle(image, center, (int)(radius+0.5), Scalar(255, 0, 0), 2); //손바닥 영역 그리기
        //imshow("Original Image With Hand Center", image);
        if (waitKey(1) == 27) break; // q키를 누르면 종료
    }

    return 0;
}

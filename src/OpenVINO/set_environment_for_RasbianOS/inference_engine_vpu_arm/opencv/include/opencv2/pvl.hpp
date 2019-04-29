/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2019, Intel Corporation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the OpenCV Foundation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/


#ifndef __OPENCV_PVL_HPP__
#define __OPENCV_PVL_HPP__


#include <vector>
#include "opencv2/core.hpp"


/**
@defgroup pvl Photography Vision Library

Photography Vision Library
--------------------------
Photography Vision Library (PVL) is set of state-of-the-art computer vision and imaging algorithms.
Each component in the PVL is optimized on Intel architecture both in performance and power consumption.

### %Face Detection
PVL provides fast and light-weight face detection with industry-leading accuracy.
A person's face can be automatically detected and tracked, and it supports wide range of rotation and long distance.
It also shows top-tier face detection rates in FDDB benchmark.

### Blink Detection
Blink of a person is detected and indexed in real-time with PVL.
Blink detection can be utilized while taking a photo to trigger the capture (in wink) or to get rid of the photo
where the subject has blinked. It can also be applied to the Driving Safety Management System for alerting a drowsy driver.

### Smile Detection
A person's smile intensity is able to be detected through analyzing the changes and movements of the face with PVL.
Smile detection can be applied to digital still cameras and camera phones as a mechanism for the smile auto shutter,
which automatically triggers the capture function when the subject is smiling.

### %Face Recognition
PVL provides efficient face recognition is real-time with industry-leading accuracy.
It shows top-tier face recognition rates in FRGC benchmark.
You can sort or browse photos by person using the PVL face recognition algorithm.
It can also be utilized in biometric authentication.
*/


/// @cond
typedef struct pvl_image                       pvl_image;
typedef struct pvl_face_detection              pvl_face_detection;
typedef struct pvl_face_detection_parameters   pvl_face_detection_parameters;
typedef struct pvl_eye_detection               pvl_eye_detection;
typedef struct pvl_mouth_detection             pvl_mouth_detection;
typedef struct pvl_blink_detection             pvl_blink_detection;
typedef struct pvl_blink_detection_parameters  pvl_blink_detection_parameters;
typedef struct pvl_smile_detection             pvl_smile_detection;
typedef struct pvl_smile_detection_parameters  pvl_smile_detection_parameters;
typedef struct pvl_face_recognition            pvl_face_recognition;
typedef struct pvl_face_recognition_result     pvl_face_recognition_result;
typedef struct pvl_face_recognition_parameters pvl_face_recognition_parameters;
/// @endcond


namespace cv
{


//! @addtogroup pvl
//! @{


/**
 * @brief
 * Contains classes, functions, and enumerations of PVL.
 */
namespace pvl
{
enum { MAX_SUPPORTED_FACES = 32,                                              /**< Maximum number of faces when detecting or recognizing faces */
       MAX_SUPPORTED_FACES_FOR_RECOGNITION_IN_TRACKING = 8,                   /**< Maximum number of faces that can be recognized in one frame in tracking mode */
       MAX_SUPPORTED_FACE_DATA_PER_PERSON = 10                                /**< Maximum number of faces for a person when registering faces */
     };


enum { BLINK_MIN_THRESHOLD = 1,                                               /**< Minimum threshold value for the blink detection */
       BLINK_MAX_THRESHOLD = 99,                                              /**< Maximum threshold value for the blink detection */
       SMILE_MIN_THRESHOLD = 1,                                               /**< Minimum threshold value for the smile detection */
       SMILE_MAX_THRESHOLD = 99,                                              /**< Maximum threshold value for the smile detection */
     };


enum { FACE_RECOGNIZER_MIN_CONFIDENCE = -USHRT_MAX,                           /**< Minimum confidence value for the result of face recognition */
       FACE_RECOGNIZER_MAX_CONFIDENCE = USHRT_MAX,                            /**< Maximum confidence value for the result of face recognition */
     };


enum { FACE_RECOGNIZER_UNKNOWN_PERSON_ID = -10000,                            /**< ID for unknown person. If there is no matched person when recognizing face, result would have this ID. */
       FACE_RECOGNIZER_START_OF_VALID_ID = 1,                                 /**< All the valid IDs used for FaceRecognizer must be equal or greater than this constant. */
     };





/**
 * @class Face
 * @brief Represents a face.
 *
 * This class contains information on face landmarks and face classifications.
 * There are face rectangle, eye, and mouth information for face landmarks.
 * In addition, there are blink and smile information for face classifications.
 * All of the information can be acquired through the FaceDetector's detection methods.
 * It is also possible to set the information manually by yourself.
 * So, if you had your own algorithm for any of these face landmarks and classifications,
 * you can use your algorithm along with FaceDetector and FaceRecognizer.
 */
class CV_EXPORTS Face
{
public:
    enum Element
    {
        FACE_RECT            = 0,     /**< Rectangle of the face. There's no default value. (type: cv::Rect) */
        RIP_ANGLE            = 1,     /**< Rotation-in-plane angle of the face. Default value is 0. (type: int) */
        ROP_ANGLE            = 2,     /**< Rotation-out-of-plane(yawing) angle of the face. Default value is 0. (type: int) */
        FACE_RECT_CONFIDENCE = 3,     /**< Value indicating how much the face is close to the typical face. Default value is 100. (type: int) */
        TRACKING_ID          = 4,     /**< Tracking ID of the face. Default value is -1. (type: int) */
        LEFT_EYE_POS         = 5,     /**< Left eye position. There's no default value. (type: cv::Point) */
        RIGHT_EYE_POS        = 6,     /**< Right eye position. There's no default value. (type: cv::Point) */
        EYE_POS_CONFIDENCE   = 7,     /**< Value indicating how much the eye position is close to the typical eye position. Default value is 100. (type: int) */
        MOUTH_POS            = 8,     /**< Mouth position. There's no default value. (type: cv::Point) */
        MOUTH_POS_CONFIDENCE = 9,     /**< Value indicating how much the mouth position is close to the typical mouth position. Default value is 100. (type: int) */
        CLOSING_LEFT_EYE     = 10,    /**< Indicates whether the person is closing the left eye. There's no default value. (type: bool) */
        CLOSING_RIGHT_EYE    = 11,    /**< Indicates whether the person is closing the right eye. There's no default value. (type: bool) */
        LEFT_BLINK_SCORE     = 12,    /**< Left eye score. The value has range from 0 to 100. 0 means wide opened eye, while 100 means fully closed eye. There's no default value. (type: int) */
        RIGHT_BLINK_SCORE    = 13,    /**< Right eye score. The value has range from 0 to 100. 0 means wide opened eye, while 100 means fully closed eye. There's no default value. (type: int) */
        SMILING              = 14,    /**< Indicates whether the person is smiling. There's no default value. (type: bool) */
        SMILE_SCORE          = 15     /**< Smile score between 0 and 100. 0 means non-smile, while 100 means full smile. There's no default value. (type: int) */
    };


public:
    /**
     * @brief Default constructor.
     */
    Face();


    /**
     * @brief Destructor.
     */
    ~Face();


    /**
     * @brief Sets face rectangle information.
     *
     * @param[in] rect       %Face rectangle
     * @param[in] ripAngle   %Face RIP angle
     * @param[in] ropAngle   %Face ROP angle
     * @param[in] confidence %Face rectangle confidence
     * @param[in] trackingID Tracking ID
     */
    void setFaceRectInfo(const cv::Rect& rect, int ripAngle = 0, int ropAngle = 0, int confidence = 100, int trackingID = -1);


    /**
     * @brief Sets eye information.
     *
     * @param[in] left       Left eye position
     * @param[in] right      Right eye position
     * @param[in] confidence Eye confidence
     */
    void setEyeInfo(const cv::Point& left, const cv::Point& right, int confidence = 100);


    /**
     * @brief Sets mouth information.
     *
     * @param[in] mouth      Mouth position
     * @param[in] confidence Mouth confidence
     */
    void setMouthInfo(const cv::Point& mouth, int confidence = 100);


    /**
     * @brief Sets blink information.
     *
     * @param[in] closingLeft  Indicates whether left eye is closing
     * @param[in] closingRight Indicates whether right eye is closing
     * @param[in] leftScore    Left blink score
     * @param[in] rightScore   Right blink score
     */
    void setBlinkInfo(bool closingLeft, bool closingRight, int leftScore, int rightScore);


    /**
     * @brief Sets smile information.
     *
     * @param[in] smiling Indicates whether face is smiling
     * @param[in] score   Smile score
     */
    void setSmileInfo(bool smiling, int score);


    /**
     * @brief Gets value of the given element.
     *
     * @param[in] element %Face element
     * @return See the method description.
     * @exception cv::Error::StsBadArg   If you specify undefined element
     * @exception cv::Error::StsInternal If the element's value was never set before
     */
    template<typename T> T get(Element element) const
    {
        T value = T();
        this->getImpl(element, &value);
        return value;
    }


    /**
     * @brief Sets value of the given element.
     *
     * @param[in] element %Face element
     * @param[in] value   Value of the element
     * @exception cv::Error::StsBadArg If you specify undefined element
     */
    template<typename T> void set(Element element, const T& value)
    {
        this->setImpl(element, &value);
    }


private:
    void getImpl(Element element, void* value) const;
    void setImpl(Element element, const void* value);


private:
    cv::Rect  faceRect;
    int       faceRipAngle;
    int       faceRopAngle;
    int       faceRectConfidence;
    int       faceTrackingID;
    cv::Point leftEyePos;
    cv::Point rightEyePos;
    int       eyePosConfidence;
    cv::Point mouthPos;
    int       mouthPosConfidence;
    bool      isClosingLeftEye;
    bool      isClosingRightEye;
    int       leftBlinkScore;
    int       rightBlinkScore;
    bool      isSmiling;
    int       smileScore;
    uint      elementSetFlag;
};





/**
 * @class FaceDetector
 * @brief Detects face landmarks and performs face classification.
 *
 * This class detects face features including face rectangle, eye position, mouth position, blink status, and smile status.
 * The face features can be detected selectively. You may detect face rectangle only, or detect face rectangle, eye position, and blink status.
 * There are dependencies among face features. In order to detect eye position or mouth position, face rectangle is needed.
 * Also, in order to detect smile status or blink status, face rectangle and eye position are required.
 * Required face features would be detected even though they are not specified for detection.
 * There are a lot of parameters that affect face detection results. Depending on the parameters you set, number of detected faces can vary.
 * As you try to detect more and more faces, detection would take a longer time.
 * Detection can be done in two different modes, which are normal mode and tracking mode.
 * With normal mode, you can get all the detected faces at one time. With tracking mode, you need to run detection several times
 * in order to detect all the faces. By doing so, face detection is relatively very fast in tracking mode.
 * If you want to detect faces with a still image, normal mode should be used. However, if you want to detect faces in a video stream,
 * usually tracking mode is recommended.
 * @n
 * There are many parameters influencing the behavior of FaceDetector. You can set your own preferred values or use default values.
 * Check for the method description.
 * @n
 * See the sample code below.
 * @code
    cv::VideoCapture cam(0);
    cv::Mat frame, grayedFrame;
    cv::Ptr<cv::pvl::FaceDetector> fd = cv::pvl::FaceDetector::create();
    std::vector<cv::pvl::Face> faces;
    cv::Scalar white(255, 255, 255);
    cv::Scalar black(0, 0, 0);

    fd->setMaxDetectableFaces(15);
    fd->setRIPAngleRange(45);
    fd->setROPAngleRange(90);
    fd->setBlinkThreshold(80);
    fd->setSmileThreshold(70);
    fd->setMinFaceSize(50);
    fd->setTrackingModeEnabled(true);

    cv::namedWindow("Sample");

    while (true)
    {
        cam >> frame;
        cvtColor(frame, grayedFrame, cv::COLOR_BGR2GRAY);

        fd->detectFaceRect(grayedFrame, faces);

        for (uint i = 0; i < faces.size(); ++i)
        {
            fd->detectEye(grayedFrame, faces[i]);
            fd->detectMouth(grayedFrame, faces[i]);
            fd->detectBlink(grayedFrame, faces[i]);
            fd->detectSmile(grayedFrame, faces[i]);
        }

        for (std::vector<cv::pvl::Face>::const_iterator iter = faces.begin(); iter != faces.end(); ++iter)
        {
            const cv::pvl::Face& face = *iter;

            cv::rectangle(frame, face.get<cv::Rect>(cv::pvl::Face::FACE_RECT), white);
            cv::circle(frame, face.get<cv::Point>(cv::pvl::Face::LEFT_EYE_POS), 5, face.get<bool>(cv::pvl::Face::CLOSING_LEFT_EYE) ? black : white);
            cv::circle(frame, face.get<cv::Point>(cv::pvl::Face::RIGHT_EYE_POS), 5, face.get<bool>(cv::pvl::Face::CLOSING_RIGHT_EYE) ? black : white);
            cv::circle(frame, face.get<cv::Point>(cv::pvl::Face::MOUTH_POS), 10, face.get<bool>(cv::pvl::Face::SMILING) ? black : white);
        }

        cv::imshow("Sample", frame);

        if (cv::waitKey(5) > 0)
            break;
    }
 * @endcode
 */
class CV_EXPORTS FaceDetector : public cv::Algorithm
{
public:
    /**
     * @brief Creates FaceDetector instance.
     *
     * @return See the method description.
     * @exception cv::Error::StsNoMem    If there's little available memory
     * @exception cv::Error::StsInternal If the internal objects fail to initialize
     */
    static cv::Ptr<FaceDetector> create();


    /**
     * @brief Destructor.
     */
    virtual ~FaceDetector();


protected:
    /// @cond
    FaceDetector();
    /// @endcond


private:
    FaceDetector(const FaceDetector& fd);
    FaceDetector& operator=(const FaceDetector& fd);


public:
    /**
     * @brief Returns RIP(Rotation-In-Plane) angle range for face detection.
     *
     * @return See the method description.
     */
    virtual int getRIPAngleRange() const = 0;


    /**
     * @brief Sets RIP(Rotation-In-Plane) angle range for face detection.
     *
     * Rotated faces within this angle range can be detected when detect method is invoked.
     * If you specify small value for the range, Detection takes lesser time since it doesn't need to find
     * much rotated faces. Default value is 135.
     *
     * @param[in] range RIP angle range
     * @exception cv::Error::StsOutOfRange If range < 0 or range > 180
     */
    virtual void setRIPAngleRange(int range) = 0;


    /**
     * @brief Returns ROP(Rotation-Out-Of-Plane) angle range for face detection.
     *
     * @return See the method description.
     */
    virtual int getROPAngleRange() const = 0;


    /**
     * @brief Sets ROP(Rotation-Out-Of-Plane) angle range for face detection.
     *
     * Rotated faces within this angle range can be detected when detect method is invoked.
     * If you specify small value for the range, Detection takes lesser time since it doesn't need to find
     * much rotated faces. Default value is 90.
     *
     * @param[in] range ROP angle range
     * @exception cv::Error::StsOutOfRange If range < 0 or range > 90
     */
    virtual void setROPAngleRange(int range) = 0;


    /**
     * @brief Returns the maximum number of detected faces.
     *
     * @return See the method description.
     */
    virtual int getMaxDetectableFaces() const = 0;


    /**
     * @brief Sets the maximum number of detected faces.
     *
     * The maximum number of detected faces cannot exceed @link MAX_SUPPORTED_FACES @endlink.
     * You can get face detection results up to this value. Default value is @link MAX_SUPPORTED_FACES @endlink.
     *
     * @param[in] numFaces Maximum number of detected faces
     * @exception cv::Error::StsOutOfRange If numFaces < 1 or numFaces > @link MAX_SUPPORTED_FACES @endlink
     */
    virtual void setMaxDetectableFaces(int numFaces) = 0;


    /**
     * @brief Gets the minimum face size in pixel.
     *
     * @return See the method description.
     */
    virtual int getMinFaceSize() const = 0;


    /**
     * @brief Sets the minimum face size in pixel.
     *
     * This value can affect performance of face detection. If you specify small value for the face size,
     * detection takes more time in order to find small faces. Default value is 64.
     *
     * @param[in] widthHeight Minimum size of faces to be detected
     * @exception cv::Error::StsOutOfRange If size < 32
     */
    virtual void setMinFaceSize(int widthHeight) = 0;


    /**
     * @brief Gets the threshold value used for evaluating blink.
     *
     * @return See the method description.
     */
    virtual int getBlinkThreshold() const = 0;


    /**
     * @brief Sets the threshold value used for evaluating blink.
     *
     * When the blink score is equal or greater than this threshold, the eye is considered closing.
     * Default value is 50.
     *
     * @param[in] threshold  Blink threshold value
     * @exception cv::Error::StsOutOfRange If threshold < @link BLINK_MIN_THRESHOLD @endlink or threshold > @link BLINK_MAX_THRESHOLD @endlink
     */
    virtual void setBlinkThreshold(int threshold) = 0;


    /**
     * @brief Gets the threshold value used for evaluating smile.
     *
     * @return See the method description.
     */
    virtual int getSmileThreshold() const = 0;


    /**
     * @brief Sets the threshold value used for evaluating smile.
     *
     * When the smile score is greater than this threshold, the face is considered smiling.
     * Default value is 48.
     *
     * @param[in] threshold  Smile threshold value
     * @exception cv::Error::StsOutOfRange If threshold < @link SMILE_MIN_THRESHOLD @endlink or threshold > @link SMILE_MIN_THRESHOLD @endlink
     */
    virtual void setSmileThreshold(int threshold) = 0;


    /**
     * @brief Tells whether the tracking mode is enabled or not.
     *
     * @return See the method description.
     */
    virtual bool isTrackingModeEnabled() const = 0;


    /**
     * @brief Sets the tracking mode as enabled or disabled.
     *
     * It is applicable only for face rectangle detection.
     *
     * @param[in] enabled  Indicates whether tracking mode is enabled or not
     */
    virtual void setTrackingModeEnabled(bool enabled) = 0;


    /**
     * @brief Detects face rectangle with the given image.
     *
     * This method runs as tracking mode or normal mode.
     * In the normal mode, this method scans the whole input image to detect face rectangles.
     * On the other hand, in the tracking mode, this method takes care of a part of the input image.
     * Internally, it divides up the entire computation into a certain number of equal parts.
     * Therefore, it might not detect all the faces at one time in the tracking mode.
     * Tracking mode can be effective for continuous frames such as video clips and camera preview.
     * Because usually there's a small variation between two consecutive frames, this method can detect
     * all the faces in the frames with a few frames. The performance of tracking mode is much better than
     * the performance of normal mode, but tracking mode might not be as accurate as normal mode.
     * Once a face is detected in tracking mode, it keeps information of the detected faces and tracks the face
     * until the face disappears out of the frame.
     * @n
     * Refer to the sample code.
     * @code
        cv::Ptr<cv::pvl::FaceDetector> fd = cv::pvl::FaceDetector::create();
        std::vector<cv::pvl::Face> faces;
        cv::Mat image = cv::imread("face_sample.jpg", cv::IMREAD_GRAYSCALE);

        fd->detectFaceRect(image, faces);

        for (uint i = 0; i < faces.size(); ++i)
        {
            cv::Rect rect = faces[i].get<cv::Rect>(cv::pvl::Face::FACE_RECT);
            int ripAngle = faces[i].get<int>(cv::pvl::Face::RIP_ANGLE);
            int ropAngle = faces[i].get<int>(cv::pvl::Face::ROP_ANGLE);
            int confidence = faces[i].get<int>(cv::pvl::Face::FACE_RECT_CONFIDENCE);

            std::cout << "rect: " << rect << std::endl;
            std::cout << "rip angle: " << ripAngle << ", rop angle: " << ropAngle << ", confidence: " << confidence << std::endl;
        }
     * @endcode
     *
     * @param[in]  image Input image. It must be grayed image that has only one channel and 8-bit depth.
     * @param[out] faces Detected faces. Number of faces is equal or less than min(@link MAX_SUPPORTED_FACES @endlink, number of detected faces).
     * @exception  cv::Error::StsUnsupportedFormat If the given image is not grayscale format, which means the image must have only one channel and the channel must have unsigned 8-bit depth
     * @exception  cv::Error::StsBadSize           If the given image's size is abnormal
     * @exception  cv::Error::StsInternal          If the internal algorithm fails to run
     */
    virtual void detectFaceRect(cv::InputArray image, std::vector<Face>& faces) = 0;


    /**
     * @brief Detects eyes position with the given image and face rectangle information.
     *
     * The face instance must have face rectangle information.
     * You can get the face rectangle information from detectFaceRect method.
     * Also, you might be able to set the face rectangle information by yourself if you got it
     * from your own algorithm.
     * @n
     * Refer to the sample code.
     * @code
        cv::Ptr<cv::pvl::FaceDetector> fd = cv::pvl::FaceDetector::create();
        std::vector<cv::pvl::Face> faces;
        cv::Mat image = cv::imread("face_sample.jpg", cv::IMREAD_GRAYSCALE);

        fd->detectFaceRect(image, faces);

        for (uint i = 0; i < faces.size(); ++i)
        {
            fd->detectEye(image, faces[i]);

            cv::Point leftEye = faces[i].get<cv::Point>(cv::pvl::Face::LEFT_EYE_POS);
            cv::Point rightEye = faces[i].get<cv::Point>(cv::pvl::Face::RIGHT_EYE_POS);
            int confidence = faces[i].get<int>(cv::pvl::Face::EYE_POS_CONFIDENCE);

            std::cout << "left: " << leftEye << ", right: " << rightEye << std::endl;
            std::cout << "confidence: " << confidence << std::endl;
        }
     * @endcode
     *
     * @param[in]    image Input image. It must be grayed image that has only one channel and 8-bit depth.
     * @param[inout] face  It must contain face rectangle information. Eye information will be set.
     * @exception    cv::Error::StsUnsupportedFormat If the given image is not grayscale format, which means the image must have only one channel and the channel must have unsigned 8-bit depth
     * @exception    cv::Error::StsBadSize           If the given image's size is abnormal
     * @exception    cv::Error::StsInternal          If the internal algorithm fails to run
     */
    virtual void detectEye(cv::InputArray image, Face& face) = 0;


    /**
     * @brief Detects mouth position with the given image and face rectangle information.
     *
     * The face instance must have face rectangle information.
     * You can get the face rectangle information from detectFaceRect method.
     * Also, you might be able to set the face rectangle information by yourself if you got it
     * from your own algorithm.
     * @n
     * Refer to the sample code.
     * @code
        cv::Ptr<cv::pvl::FaceDetector> fd = cv::pvl::FaceDetector::create();
        std::vector<cv::pvl::Face> faces;
        cv::Mat image = cv::imread("face_sample.jpg", cv::IMREAD_GRAYSCALE);

        fd->detectFaceRect(image, faces);

        for (uint i = 0; i < faces.size(); ++i)
        {
            fd->detectMouth(image, faces[i]);

            cv::Point mouth = faces[i].get<cv::Point>(cv::pvl::Face::MOUTH_POS);
            int confidence = faces[i].get<int>(cv::pvl::Face::MOUTH_POS_CONFIDENCE);

            std::cout << "mouth: " << mouth << std::endl;
            std::cout << "confidence: " << confidence << std::endl;
        }
     * @endcode
     *
     * @param[in]    image Input image. It must be grayed image that has only one channel and 8-bit depth.
     * @param[inout] face  It must contain face rectangle information. Mouth information will be set.
     * @exception    cv::Error::StsUnsupportedFormat If the given image is not grayscale format, which means the image must have only one channel and the channel must have unsigned 8-bit depth
     * @exception    cv::Error::StsBadSize           If the given image's size is abnormal
     * @exception    cv::Error::StsInternal          If the internal algorithm fails to run
     */
    virtual void detectMouth(cv::InputArray image, Face& face) = 0;


    /**
     * @brief Detects blink with the given image, face rectangle information, and eye information.
     *
     * The face instance must have face rectangle and eye information.
     * You can get the face rectangle information from detectFaceRect method and the eye information
     * from detectEye method. Also, you might be able to set the face rectangle and eye information
     * by yourself if you got them from your own algorithms.
     * @n
     * Refer to the sample code.
     * @code
        cv::Ptr<cv::pvl::FaceDetector> fd = cv::pvl::FaceDetector::create();
        std::vector<cv::pvl::Face> faces;
        cv::Mat image = cv::imread("face_sample.jpg", cv::IMREAD_GRAYSCALE);

        fd->detectFaceRect(image, faces);

        for (uint i = 0; i < faces.size(); ++i)
        {
            fd->detectEye(image, faces[i]);
            fd->detectBlink(image, faces[i]);

            bool closingLeft = faces[i].get<bool>(cv::pvl::Face::CLOSING_LEFT_EYE);
            bool closingRight = faces[i].get<bool>(cv::pvl::Face::CLOSING_RIGHT_EYE);
            int leftScore = faces[i].get<int>(cv::pvl::Face::LEFT_BLINK_SCORE);
            int rightScore = faces[i].get<int>(cv::pvl::Face::RIGHT_BLINK_SCORE);

            std::cout << "closing left eye: " << closingLeft << ", closing right eye: " << closingRight << std::endl;
            std::cout << "left blink score: " << leftScore << ", right blink score: " << rightScore << std::endl;
        }
     * @endcode
     * The following sample code shows how you can set face rectangle and eye information manually
     * in order to do blink detection.
     * @code
        cv::Ptr<cv::pvl::FaceDetector> fd = cv::pvl::FaceDetector::create();
        cv::pvl::Face face;
        cv::Mat image = cv::imread("face_sample.jpg", cv::IMREAD_GRAYSCALE);
        cv::Rect faceRect;
        cv::Point leftEye, rightEye;

        // Assume that you can get face rectangle and eye information from your own algorithms.
        // faceRect = cv::Rect(x, y, width, height);
        // leftEye = cv::Point(x, y);
        // rightEye = cv::Point(x, y);
        face.setFaceRectInfo(faceRect);
        face.setEyeInfo(leftEye, rightEye);

        fd->detectBlink(image, face);

        bool closingLeft = face.get<bool>(cv::pvl::Face::CLOSING_LEFT_EYE);
        bool closingRight = face.get<bool>(cv::pvl::Face::CLOSING_RIGHT_EYE);
        int leftScore = face.get<int>(cv::pvl::Face::LEFT_BLINK_SCORE);
        int rightScore = face.get<int>(cv::pvl::Face::RIGHT_BLINK_SCORE);

        std::cout << "closing left eye: " << closingLeft << ", closing right eye: " << closingRight << std::endl;
        std::cout << "left blink score: " << leftScore << ", right blink score: " << rightScore << std::endl;
     * @endcode
     *
     * @param[in]    image Input image. It must be grayed image that has only one channel and 8-bit depth.
     * @param[inout] face  It must contain face rectangle and eye information. Blink information will be set.
     * @exception    cv::Error::StsUnsupportedFormat If the given image is not grayscale format, which means the image must have only one channel and the channel must have unsigned 8-bit depth
     * @exception    cv::Error::StsBadSize           If the given image's size is abnormal
     * @exception    cv::Error::StsInternal          If the internal algorithm fails to run
     */
    virtual void detectBlink(cv::InputArray image, Face& face) = 0;


    /**
     * @brief Detects smile with the given image, face rectangle information, and eye information.
     *
     * The face instance must have face rectangle and eye information.
     * You can get the face rectangle information from detectFaceRect method and the eye information
     * from detectEye method. Also, you might be able to set the face rectangle and eye information
     * by yourself if you got them from your own algorithms.
     * @n
     * Refer to the sample code.
     * @code
        cv::Ptr<cv::pvl::FaceDetector> fd = cv::pvl::FaceDetector::create();
        std::vector<cv::pvl::Face> faces;
        cv::Mat image = cv::imread("face_sample.jpg", cv::IMREAD_GRAYSCALE);

        fd->detectFaceRect(image, faces);

        for (uint i = 0; i < faces.size(); ++i)
        {
            fd->detectEye(image, faces[i]);
            fd->detectSmile(image, faces[i]);

            bool smiling = faces[i].get<bool>(cv::pvl::Face::SMILING);
            int smileScore = faces[i].get<int>(cv::pvl::Face::SMILE_SCORE);

            std::cout << "smiling: " << smiling << ", smile score: " << smileScore << std::endl;
        }
     * @endcode
     * The following sample code shows how you can set face rectangle and eye information manually
     * in order to do smile detection.
     * @code
        cv::Ptr<cv::pvl::FaceDetector> fd = cv::pvl::FaceDetector::create();
        cv::pvl::Face face;
        cv::Mat image = cv::imread("face_sample.jpg", cv::IMREAD_GRAYSCALE);
        cv::Rect faceRect;
        cv::Point leftEye, rightEye;

        // Assume that you can get face rectangle and eye information from your own algorithms.
        // faceRect = cv::Rect(x, y, width, height);
        // leftEye = cv::Point(x, y);
        // rightEye = cv::Point(x, y);
        face.setFaceRectInfo(faceRect);
        face.setEyeInfo(leftEye, rightEye);

        fd->detectBlink(image, face);

        bool smiling = face.get<bool>(cv::pvl::Face::SMILING);
        int smileScore = face.get<int>(cv::pvl::Face::SMILE_SCORE);

        std::cout << "smiling: " << smiling << ", smile score: " << smileScore << std::endl;
     * @endcode
     *
     * @param[in]    image Input image. It must be grayed image that has only one channel and 8-bit depth.
     * @param[inout] face  It must contain face rectangle and eye information. Smile information will be set.
     * @exception    cv::Error::StsUnsupportedFormat If the given image is not grayscale format, which means the image must have only one channel and the channel must have unsigned 8-bit depth
     * @exception    cv::Error::StsBadSize           If the given image's size is abnormal
     * @exception    cv::Error::StsInternal          If the internal algorithm fails to run
     */
    virtual void detectSmile(cv::InputArray image, Face& face) = 0;


    /**
     * @copydoc Algorithm::clear()
     *
     * @exception cv::Error::StsInternal If the internal algorithm fails to reset state of the objects
     */
    virtual void clear() CV_OVERRIDE;
};





/**
 * @class FaceRecognizer
 * @brief Recognizes the given faces in the input image.
 *
 * In order to recognize faces, face database should be constructed first.
 * You can register faces to the internal database of this class' instance.
 * You can also unregister the registered faces from the internal database.
 * Registered faces can be stored in a file, and you can load the stored file when you create an instance.
 * Unless you explicitly store registered faces into a file, the internal database is removed when
 * the instance's destructor is invoked.
 * %Face recognition runs in two different mode, which are normal mode and tracking mode.
 * In the normal mode, it always tries to handle the whole input image to recognize faces.
 * In the tracking mode, if the given faces are the same as ones of previous frames and the faces are recognized,
 * it will not recognize faces again. If performance is an important factor, it is recommended to use the tracking mode.
 * @n
 * Please see the sample code below.
 * @code
    cv::VideoCapture cam(0);
    cv::Mat frame, grayedFrame;
    cv::Ptr<cv::pvl::FaceDetector> fd = cv::pvl::FaceDetector::create();
    cv::Ptr<cv::pvl::FaceRecognizer> fr = cv::pvl::FaceRecognizer::create();
    std::vector<cv::pvl::Face> faces;
    cv::Scalar defaultColor(255, 255, 255), regPersonColor(255, 0, 0);
    cv::Mat image = cv::imread("my_face.jpg", cv::IMREAD_GRAYSCALE);
    std::vector<int> personIDs;

    // Register a face
    fd->detectFaceRect(image, faces);
    fr->registerFace(image, faces[0], fr->createNewPersonID(), true);

    fd->setTrackingModeEnabled(true);
    fr->setTrackingModeEnabled(true);

    cv::namedWindow("Sample");

    while (true)
    {
        cam >> frame;
        cv::cvtColor(frame, grayedFrame, cv::COLOR_BGR2GRAY);

        // Detect faces first and then try to recognize the faces
        fd->detectFaceRect(grayedFrame, faces);

        if (!faces.empty())
        {
            fr->recognize(grayedFrame, faces, personIDs);

            for (int i = 0; i < personIDs.size(); ++i)
            {
                cv::rectangle(frame, faces[i].get<cv::Rect>(cv::pvl::Face::FACE_RECT), (personIDs[i] == cv::pvl::FACE_RECOGNIZER_UNKNOWN_PERSON_ID) ? defaultColor : regPersonColor);
            }
        }

        cv::imshow("Sample", frame);

        if (cv::waitKey(5) > 0)
            break;
    }

    // Store registered faces in the file.
    fr->save("frdb.xml");
 * @endcode
 * @n
 * Below is another sample code that creates FaceRecognizer instance from the file.
 * @code
    cv::VideoCapture cam(0);
    cv::Mat frame, grayedFrame;

    // cv::Algorithm::load method loads the given file and creates FaceRecognizer instance with content of the file.
    // FaceRecognizer reads information of faces and register it.
    cv::Ptr<cv::pvl::FaceRecognizer> fr = cv::Algorithm::load<cv::pvl::FaceRecognizer>("frdb.xml");
    cv::Ptr<cv::pvl::FaceDetector> fd = cv::pvl::FaceDetector::create();
    cv::Mat image = cv::imread("my_faces.jpg", cv::IMREAD_GRAYSCALE);
    std::vector<cv::pvl::Face> faces;
    cv::Scalar defaultColor(255, 255, 255), regPersonColor(255, 0, 0);
    std::vector<int> personIDs;

    fd->setTrackingModeEnabled(true);
    fr->setTrackingModeEnabled(true);

    while (true)
    {
        cam >> frame;
        cv::cvtColor(frame, grayedFrame, cv::COLOR_BGR2GRAY);

        // Detect faces first and then try to recognize the faces
        fd->detectFaceRect(grayedFrame, faces);

        if (!faces.empty())
        {
            fr->recognize(grayedFrame, faces, personIDs);

            for (int i = 0; i < personIDs.size(); ++i)
            {
                cv::rectangle(frame, faces[i].get<cv::Rect>(cv::pvl::Face::FACE_RECT), (personIDs[i] == cv::pvl::FACE_RECOGNIZER_UNKNOWN_PERSON_ID) ? defaultColor : regPersonColor);
            }
        }

        cv::imshow("Sample", frame);

        if (cv::waitKey(5) > 0)
            break;
    }
 * @endcode
 */
class CV_EXPORTS FaceRecognizer : public cv::Algorithm
{
public:
    /**
     * @brief Creates an instance of FaceRecognizer class.
     *
     * @return See the method description.
     * @exception cv::Error::StsNoMem    If there's little available memory
     * @exception cv::Error::StsInternal If the internal objects fail to initialize
     */
    static cv::Ptr<FaceRecognizer> create();


protected:
    /// @cond
    FaceRecognizer();
    /// @endcond


public:
    /**
     * @brief Destructor.
     */
    virtual ~FaceRecognizer();


private:
    FaceRecognizer(const FaceRecognizer& fr);
    FaceRecognizer& operator=(const FaceRecognizer& rhs);


public:
    /**
    * @brief Gets the maximum number of faces that can be set as parameter for tracking mode.
    *
    * @return See the method description.
    */
    virtual int getMaxSupportedFacesInTracking() const = 0;


    /**
     * @brief Gets the maximum number of faces that can be recognized in one frame in tracking mode.
     *
     * @return See the method description.
     */
    virtual int getMaxFacesInTracking() const = 0;


    /**
     * @brief Sets the maximum number of faces that can be recognized in one frame in tracking mode.
     *
     * This value may not exceed @link MAX_SUPPORTED_FACES_FOR_RECOGNITION_IN_TRACKING @endlink.
     * Default value is @link MAX_SUPPORTED_FACES_FOR_RECOGNITION_IN_TRACKING @endlink.
     *
     * @param[in] numFaces Number of faces
     * @exception cv::Error::StsBadArg If numFaces < 1 or numFaces > @link MAX_SUPPORTED_FACES_FOR_RECOGNITION_IN_TRACKING @endlink
     */
    virtual void setMaxFacesInTracking(int numFaces) = 0;


    /**
     * @brief Tells whether the tracking mode is enabled or not.
     *
     * @return See the method description.
     */
    virtual bool isTrackingModeEnabled() const = 0;


    /**
     * @brief Sets the tracking mode as enabled or disabled.
     *
     * @param[in] enabled  Indicates whether the tracking mode is enabled
     */
    virtual void setTrackingModeEnabled(bool enabled) = 0;


    /**
     * @brief Recognizes faces with the given image and face information.
     *
     * This method recognizes the given faces with registered faces.
     * If the face is recognized successfully, this method returns person ID and confidence.
     * Otherwise, you will get person ID as @link FACE_RECOGNIZER_UNKNOWN_PERSON_ID @endlink .
     * In normal mode, it always scans the given image entirely with the given faces to recognize faces.
     * On the other hand, in tracking mode it does not process the given faces again
     * if the faces is recognized in previous method calls. It makes performance of face recognition better.
     * In the case where you want to do face recognition with video stream, it is recommended to use tracking mode.
     * Like face detection, the number of faces that can be processed at the same time cannot exceed the @link MAX_SUPPORTED_FACES @endlink .
     * In tracking mode, the maximum number of recognizable faces is limited to the number set by setMaxFacesInTracking method.
     * @n
     * Refer to the sample code.
     * @code
        cv::VideoCapture cam(0);
        cv::Mat frame, grayedFrame;
        cv::Ptr<cv::pvl::FaceDetector> fd = cv::pvl::FaceDetector::create();
        cv::Ptr<cv::pvl::FaceRecognizer> fr = cv::pvl::FaceRecognizer::create();
        std::vector<cv::pvl::Face> faces;
        std::vector<int> personIDs;
        std::vector<int> confidence;

        // Assume that faces have been already registered.

        fd->setTrackingModeEnabled(true);
        fr->setTrackingModeEnabled(true);

        while (true)
        {
            cam >> frame;
            cv::cvtColor(frame, grayedFrame, cv::COLOR_BGR2GRAY);

            // Detect faces first and then try to recognize the faces
            fd->detectFaceRect(grayedFrame, faces);

            if (!faces.empty())
            {
                fr->recognize(grayedFrame, faces, personIDs, confidence);

                for (uint i = 0; i < personIDs.size(); ++i)
                    std::cout << "person: " << personIDs[i] << ", confidence: " << confidence[i] << std::endl;
            }

            if (cv::waitKey(5) > 0)
                break;
        }
     * @endcode
     *
     * @param[in]  image      Image that contains the faces. It should be the same image as the one used for face detection
     * @param[in]  faces      Faces that will be recognized with registered faces
     * @param[out] personIDs  It contains person IDs of the given faces. If there's no matched face in the internal database, the ID is @link FACE_RECOGNIZER_UNKNOWN_PERSON_ID @endlink
     * @param[out] confidence It contains confidence values of the given faces. If there's no matched face in the internal database, the value is the minimum.
     * @exception  cv::Error::StsOutOfRange        If faces is empty or the number of faces is greater than @link MAX_SUPPORTED_FACES @endlink
     * @exception  cv::Error::StsOutOfRange        If the number of faces is greater than the maximum number of faces that can be recognized in one frame in tracking mode
     * @exception  cv::Error::StsUnsupportedFormat If the given image is not grayscale format, which means the image must have only one channel and the channel must have unsigned 8-bit depth
     * @exception  cv::Error::StsBadSize           If the given image's size is abnormal
     * @exception  cv::Error::StsNoMem             If there's little available memory
     * @exception  cv::Error::StsInternal          If the internal algorithm fails to run
     */
    virtual void recognize(cv::InputArray image, const std::vector<Face>& faces, std::vector<int>& personIDs, std::vector<int>& confidence) = 0;


    /**
     * @overload
     */
    virtual void recognize(cv::InputArray image, const std::vector<Face>& faces, std::vector<int>& personIDs) = 0;


    /**
     * @brief Registers face information into the internal database.
     *
     * This method tries to register the given face. The face must have face rectangle information.
     * The face rectangle information can be acquired from FaceDetector::detectFaceRect method.
     * Also, you could set the information manually if you had your own algorithm to detect face rectangle.
     * In order to enhance recognition accuracy, you should register multiple faces for the same person.
     * You can check the maximum number of faces for a person with getMaxRegistrableFacesPerPerson method.
     * In order to prevent unexpected behavior, it is recommended not to register rotated-out-of-plance faces.
     * PVL face recognizer can cover face ROP angle range from -22.5 degree to 22.5 degree.
     * @n
     * Refer to the sample code.
     * @code
        cv::Ptr<cv::pvl::FaceDetector> fd = cv::pvl::FaceDetector::create();
        cv::Ptr<cv::pvl::FaceRecognizer> fr = cv::pvl::FaceRecognizer::create();
        std::vector<cv::pvl::Face> faces;
        cv::Mat image;
        int personID = -1;

        for (int i = 0; i < NUM_PERSONS; ++i)
        {
            personID = fr->createNewPersonID();

            for (int j = 0; j < NUM_FACES_PER_PERSON; ++j)
            {
                std::stringstream imagePathStream;
                imagePathStream << "person" << i << "face" << j << ".jpg";
                image = cv::imread(imagePathStream.str(), cv::IMREAD_GRAYSCALE);

                fd->detectFaceRect(image, faces);

                // Assume that the image has only one face.
                fr->registerFace(image, faces[0], personID, true);
            }
        }

        fr->save("facedb.xml");
     * @endcode
     * The following sample code shows how to register faces when you already know face rectangle information.
     * @code
        cv::Ptr<cv::pvl::FaceRecognizer> fr = cv::pvl::FaceRecognizer::create();
        cv::pvl::Face face;
        cv::Rect faceRect;
        cv::Mat image;
        int personID = -1;

        for (int i = 0; i < NUM_PERSONS; ++i)
        {
            std::stringstream imagePathStream;
            imagePathStream << "person" << i << ".jpg";
            image = cv::imread(imagePathStream.str(), cv::IMREAD_GRAYSCALE);
            personID = fr->createNewPersonID();

            for (int j = 0; j < NUM_FACES_PER_PERSON; ++j)
            {
                // Assume that you already have face rectangle information.
                // faceRect = cv::Rect(x, y, width, height);
                face.setFaceRectInfo(faceRect);
                fr->registerFace(image, face, personID, true);
            }
        }

        fr->save("facedb.xml");
     * @endcode
     *
     * @param[in] image      Image that contains faces
     * @param[in] face       %Face information
     * @param[in] personID   Person ID for the face
     * @param[in] saveToFile If true, registered face can be stored in a file
     * @return    It returns a face ID as the result of registration. The face ID can be used when you want to deregister the face.
     * @exception cv::Error::StsUnsupportedFormat If the given image is not grayscale format, which means the image must have only one channel and the channel must have unsigned 8-bit depth
     * @exception cv::Error::StsBadSize           If the given image's size is abnormal
     * @exception cv::Error::StsBadArg            If person ID is not valid
     * @exception cv::Error::StsNoMem             If there's little available memory
     * @exception cv::Error::StsInternal          If the internal database is full
     * @exception cv::Error::StsInternal          If the internal algorithm fails to register the face
     */
    virtual int64 registerFace(cv::InputArray image, const Face& face, int personID, bool saveToFile = false) = 0;


    /**
     * @brief Deregisters the previously registered face from the internal database.
     *
     * @param[in] faceID ID of the face to be deregistered.
     * @exception cv::Error::StsOutOfRange     If the faceID is invalid
     * @exception cv::Error::StsObjectNotFound If the face was not regitered previously
     * @exception cv::Error::StsInternal       If the internal algorithm fails to unregister the face
     */
    virtual void deregisterFace(int64 faceID) = 0;


    /**
     * @brief Deregisters all the previously registered faces of the given person.
     *
     * @param[in] personID ID of the person whose faces are going to be deregistered.
     * @exception cv::Error::StsOutOfRange     If the personID is invalid
     * @exception cv::Error::StsObjectNotFound If there's no face regitered previously for the person
     * @exception cv::Error::StsInternal       If the internal algorithm fails to unregister the faces
     */
    virtual void deregisterPerson(int personID) = 0;


    /**
     * @brief Gets the number of currently registered faces.
     *
     * @return See the method description.
     */
    virtual int getNumRegisteredFaces() const = 0;


    /**
     * @brief Gets the number of currently registered persons.
     *
     * @return See the method description.
     */
    virtual int getNumRegisteredPersons() const = 0;


    /**
     * @brief Gets the number of currently registered faces for the given person.
     *
     * @return See the method description.
     * @exception cv::Error::StsOutOfRange If the given personID is invalid
     */
    virtual int getNumRegisteredFaces(int personID) const = 0;


    /**
     * @brief Gets the maximum number of faces that can be registered.
     *
     * @return See the method description.
     */
    virtual int getMaxRegistrableFaces() const = 0;


    /**
     * @brief Gets the maximum number of persons that can be registered.
     *
     * @return See the method description.
     */
    virtual int getMaxRegistrablePersons() const = 0;


    /**
     * @brief Gets the maximum number of faces for a person that can be registered.
     *
     * @return See the method description.
     */
    virtual int getMaxRegistrableFacesPerPerson() const = 0;


    /**
     * @brief Creates a person ID.
     *
     * This method creates a new person ID that is not conflicted with existing IDs.
     * It is recommended not to use user-assigned person IDs in order to prevent registration failure.
     *
     * @return See the method description.
     */
    virtual int createNewPersonID() = 0;


    /**
     * @copydoc Algorithm::clear()
     *
     * @exception cv::Error::StsInternal If the internal algorithm fails to reset state of the object
     */
    virtual void clear() CV_OVERRIDE;


    // inherited from Algorithm
    virtual bool empty() const CV_OVERRIDE;


    // inherited from Algorithm
    virtual void write(cv::FileStorage& fs) const CV_OVERRIDE;


    // inherited from Algorithm
    virtual void read(const cv::FileNode& fn) CV_OVERRIDE;


    // inherited from Algorithm
    virtual void save(const cv::String& filename) const CV_OVERRIDE;
};


}; // namespace pvl


//! @} pvl


}; // namespace cv


#endif

// Copyright (C) 2019 Intel Corporation
// SPDX-License-Identifier: BSD-3-Clause

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/pvl.hpp"
#include <iostream>
#include <sstream>


void displayCommandInfo(cv::Mat& image);
void displayFDInfo(cv::Mat& image, const std::vector<cv::pvl::Face>& faces);
void displayFRInfo(cv::Mat& image, const std::vector<cv::pvl::Face>& faces, const std::vector<int>& personIDs, const std::vector<int>& confidence);


const int FONT = cv::FONT_HERSHEY_PLAIN;
const cv::Scalar GREEN(0, 255, 0);
const cv::Scalar BLUE(255, 0, 0);
const cv::Scalar WHITE(255, 255, 255);


int main()
{
    const char* WINDOW_NAME = "PVL Face Detection/Recognition Simple Demo";

    cv::Ptr<cv::pvl::FaceDetector> pvlFD = cv::pvl::FaceDetector::create();
    cv::Ptr<cv::pvl::FaceRecognizer> pvlFR = cv::pvl::FaceRecognizer::create();
    cv::Mat imgIn, imgGray;
    cv::VideoCapture capture(0);
    std::vector<cv::pvl::Face> faces;
    std::vector<int>  personIDs;
    std::vector<int>  confidence;

    if (!capture.isOpened())
    {
        std::cerr << "Error: fail to open camera." << std::endl;
        return -1;
    }

    pvlFD->setTrackingModeEnabled(true);
    pvlFR->setTrackingModeEnabled(true);

    cv::namedWindow(WINDOW_NAME, cv::WINDOW_AUTOSIZE);

    while (true)
    {
        capture >> imgIn;
        cv::cvtColor(imgIn, imgGray, cv::COLOR_BGR2GRAY);

        displayCommandInfo(imgIn);

        //do face detection
        pvlFD->detectFaceRect(imgGray, faces);

        for (uint i = 0; i < faces.size(); ++i)
        {
            pvlFD->detectEye(imgGray, faces[i]);
            pvlFD->detectMouth(imgGray, faces[i]);
            pvlFD->detectBlink(imgGray, faces[i]);
            pvlFD->detectSmile(imgGray, faces[i]);
        }

        displayFDInfo(imgIn, faces);

        //do face recognition
        if (faces.size() > 0)
        {
            // Keep as much as the maximum number of faces in FR's tracking mode
            if (faces.size() > static_cast<uint>(pvlFR->getMaxFacesInTracking()))
            {
                for (uint i = pvlFR->getMaxFacesInTracking(); i < faces.size(); ++i)
                    faces.pop_back();
            }

            pvlFR->recognize(imgGray, faces, personIDs, confidence);
        }

        //display FD & FR results on top of input image
        displayFRInfo(imgIn, faces, personIDs, confidence);

        cv::imshow(WINDOW_NAME, imgIn);

        //handle key input
        char key = static_cast<char>(cv::waitKey(5));

        if (key == 'q' || key == 'Q')       //escape
        {
            break;
        }
        else if (key == 'r' || key == 'R')  //train current unknown faces
        {
            for (uint i = 0; i < personIDs.size(); i++)
            {
                // If the face is unknown and its ROP angle is less than 23
                if (personIDs[i] == cv::pvl::FACE_RECOGNIZER_UNKNOWN_PERSON_ID && faces[i].get<int>(cv::pvl::Face::ROP_ANGLE) < 23)
                {
                    int personID = pvlFR->createNewPersonID();
                    pvlFR->registerFace(imgGray, faces[i], personID, true);
                }
            }
        }
    }

    cv::destroyWindow(WINDOW_NAME);

    return 0;
}


void displayCommandInfo(cv::Mat& image)
{
    putText(image, "(Q)uit", cv::Point(2, 16), FONT, 1, WHITE, 1, 8);
    putText(image, "(R)egister unknown faces", cv::Point(2, 32), FONT, 1, WHITE, 1, 8);
}


void displayFDInfo(cv::Mat& image, const std::vector<cv::pvl::Face>& faces)
{
    for (uint i = 0; i < faces.size(); ++i)
    {
        const cv::pvl::Face& face = faces[i];
        cv::Rect faceRect = face.get<cv::Rect>(cv::pvl::Face::FACE_RECT);

        // Draw face rect
        cv::rectangle(image, faceRect, WHITE, 2);

        // Draw eyes
        cv::circle(image, face.get<cv::Point>(cv::pvl::Face::LEFT_EYE_POS), 3, BLUE, 2);
        cv::circle(image, face.get<cv::Point>(cv::pvl::Face::RIGHT_EYE_POS), 3, BLUE, 2);

        // Draw mouth
        cv::circle(image, face.get<cv::Point>(cv::pvl::Face::MOUTH_POS), 5, BLUE, 2);

        // Draw blink
#define MARGIN 5
        int left_x = faceRect.x - MARGIN;
        int left_height = faceRect.height / 100 * face.get<int>(cv::pvl::Face::LEFT_BLINK_SCORE);

        int right_x = faceRect.x + faceRect.width + MARGIN;
        int right_height = faceRect.height / 100 * face.get<int>(cv::pvl::Face::RIGHT_BLINK_SCORE);

        int y = faceRect.y + faceRect.height;

        cv::line(image, cv::Point(left_x, y), cv::Point(left_x, y - left_height), BLUE, 4);
        std::stringstream left(std::ios_base::app | std::ios_base::out);
        left << face.get<int>(cv::pvl::Face::LEFT_BLINK_SCORE);
        cv::putText(image, left.str(), cv::Point(left_x - 25, y), FONT, 1, BLUE, 2);

        cv::line(image, cv::Point(right_x, y), cv::Point(right_x, y - right_height), BLUE, 4);
        std::stringstream right(std::ios_base::app | std::ios_base::out);
        right << face.get<int>(cv::pvl::Face::RIGHT_BLINK_SCORE);
        cv::putText(image, right.str(), cv::Point(right_x + 5, y), FONT, 1, BLUE, 2);

        // Draw smile
        int x = faceRect.x + 5;
        y = faceRect.y + faceRect.height + 15;

        std::stringstream smile(std::ios_base::app | std::ios_base::out);
        smile << face.get<int>(cv::pvl::Face::SMILE_SCORE);
        cv::putText(image, "smile score", cv::Point(x, y), FONT, 1, BLUE);
        cv::putText(image, smile.str(), cv::Point(x, y + 20), FONT, 1, BLUE);
    }
}


void displayFRInfo(cv::Mat& image, const std::vector<cv::pvl::Face>& faces, const std::vector<int>& personIDs, const std::vector<int>& confidence)
{
    cv::String str;

    for (uint i = 0; i < faces.size(); i++)
    {
        const cv::pvl::Face& face = faces[i];
        cv::Rect faceRect = face.get<cv::Rect>(cv::pvl::Face::FACE_RECT);

        //draw FR info
        str = (personIDs[i] > 0) ? cv::format("Person: %d(%d)", personIDs[i], confidence[i]) : "UNKNOWN";

        cv::Size strSize = cv::getTextSize(str, FONT, 1.2, 2, NULL);
        cv::Point strPos(faceRect.x + (faceRect.width / 2) - (strSize.width / 2), faceRect.y - 2);
        cv::putText(image, str, strPos, FONT, 1.2, GREEN, 2);
    }
}

// Copyright (C) 2019 Intel Corporation
// SPDX-License-Identifier: BSD-3-Clause

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"

#include "opencv2/pvl.hpp"

#include <iostream>
#include <fstream>
#include <sstream>


using namespace cv;
using namespace std;
using namespace cv::pvl;


//--------------------------------------------------------------------------------
// define: editable
//--------------------------------------------------------------------------------
//Maximum number of db member for faces
//Unlimited, but recommend less than 1000 for better performance
#define MAX_FACES_IN_DATABASE       320


//--------------------------------------------------------------------------------
// user-defined enum
//--------------------------------------------------------------------------------
enum eResMode
{
    E_RES_CAMERA = 0,
    E_RES_VIDEO_FILE = 1,
    E_RES_IMAGE_FILE = 2,
};


//--------------------------------------------------------------------------------
// function definition
//--------------------------------------------------------------------------------
void execFDFR(eResMode resMode, const string& res_file_path, const string& db_path);

static void help();
bool parseArg(int argc, char **argv, eResMode& resMode, string& resource, string& dbpath);
bool displayInfo(Mat& img, const vector<Face>& faces, const vector<int>& personIDs, const vector<int>& confidence, eResMode resMode, double elapsed);


//--------------------------------------------------------------------------------
// functions
//--------------------------------------------------------------------------------
int main(int argc, char **argv)
{
    eResMode resMode = E_RES_CAMERA;
    string resPath;
    string dbPath;

    if (parseArg(argc, argv, resMode, resPath, dbPath) != true)
    {
        help();
        return 0;
    }

    execFDFR(resMode, resPath, dbPath);

    return 1;
}


static void help()
{
    cout << "------------------------------------------------------------" << endl;
    cout << "Usage  : exec -mode={camera|video|image}" << endl;
    cout << "              -resource=<resPath> -db=<dbPath>" << endl;
    cout << "example:" << endl;
    cout << "    exec (camera mode & default frdb path in running folder)" << endl;
    cout << "    exec -help" << endl;
    cout << "    exec -mode=video -resource=video.mp4 -db=facedb.xml" << endl;
    cout << "" << endl;
    cout << "-mode:" << endl;
    cout << "    camera     : Default. Use camera frame." << endl;
    cout << "    video      : Use video file frame from 'resource'." << endl;
    cout << "    image      : Use a image file from 'resource'." << endl;
    cout << "-resource: resource path." << endl;
    cout << "-db: face recognition database xml file path." << endl;
    cout << "     If not exist, it's created." << endl;
    cout << "------------------------------------------------------------" << endl;
}


bool parseArg(int argc, char **argv, eResMode& resMode, string& resource, string& dbPath)
{
    string mode;

    //parse arg
    const string keys =
        "{help h usage ?    |                   |   }"
        "{mode              |camera             |   }"
        "{resource          |                   |   }"
        "{db                |defaultdb.xml      |   }"
        ;

    CommandLineParser parser(argc, argv, keys);
    if (parser.has("help"))
    {
        return false;
    }

    mode = parser.get<string>("mode");
    resource = parser.get<string>("resource");
    dbPath = parser.get<string>("db");

    if (dbPath.length() == 0)
    {
        return false;
    }

    //set mode of resource
    resMode = E_RES_CAMERA;
    if (mode.compare("video") == 0)
    {
        resMode = E_RES_VIDEO_FILE;
        if (resource.length() == 0)
        {
            return false;
        }
    }
    else if (mode.compare("image") == 0)
    {
        resMode = E_RES_IMAGE_FILE;
        if (resource.length() == 0)
        {
            return false;
        }
    }
    else if (mode.compare("camera") == 0)
    {
        resMode = E_RES_CAMERA;
    }
    else
    {
        return false;
    }

    return true;
}


//--------------------------------------------------------------------------------
// main feature
// execute FD & FR with given resource
//--------------------------------------------------------------------------------
void execFDFR(eResMode resMode, const string& resFilePath, const string& dbPath)
{
    const char* WINDOW_NAME = "Face Recognition";

    Ptr<FaceDetector> pvlFD;
    Ptr<FaceRecognizer> pvlFR;

    Mat imgIn;
    Mat imgGray;

    //get resource
    VideoCapture capture;

    if (resMode == E_RES_CAMERA)
    {
        capture.open(0);
    }
    else
    {
        capture.open(resFilePath);
    }

    if (!capture.isOpened())
    {
        cerr << "Error: fail to capture video." << endl;
        return;
    }

    //init fr
    pvlFR = FaceRecognizer::create();
    if (pvlFR.empty())
    {
        cerr << "Error: fail to create PVL face recognizer" << endl;
        return;
    }

    //init fd
    pvlFD = FaceDetector::create();
    if (pvlFD.empty())
    {
        cerr << "Error: fail to create PVL face detector" << endl;
        return;
    }

    //prepare image
    capture >> imgIn;
    if (imgIn.empty())
    {
        cerr << "Error: no input image" << endl;
        return;
    }

    //prepare grayimage
    cvtColor(imgIn, imgGray, COLOR_BGR2GRAY);
    if (imgGray.empty())
    {
        cerr << "Error: get gray image()" << endl;
        return;
    }

    //init loop
    bool bRegist = false;
    int keyDelay = 1;
    if (resMode == E_RES_IMAGE_FILE)
    {
        keyDelay = 0;
    }

    namedWindow(WINDOW_NAME, WINDOW_AUTOSIZE);

    //tracking mode
    bool bTracking = false;
    if (resMode == E_RES_IMAGE_FILE)
    {
        bTracking = false;
    }
    else
    {
        bTracking = true;
    }

    pvlFD->setTrackingModeEnabled(bTracking);
    pvlFR->setTrackingModeEnabled(bTracking);

    vector<Face> faces;
    vector<Face> validFaces;
    vector<int>  personIDs;
    vector<int>  confidence;

    int64 startTick = getTickCount();
    //pvlFR->setMaxFacesInTracking(MAX_SUPPORTED_FACES_FOR_RECOGNITION_IN_TRACKING);

    //main loop
    for (;;)
    {
        faces.clear();
        personIDs.clear();
        confidence.clear();

        //do face detection
        pvlFD->detectFaceRect(imgGray, faces);

        //do face recognition
        if (faces.size() > 0)
        {
            //maxmum FR count in a frame
            validFaces.clear();
            int validFaceCount = 0;

            if (bTracking)
            {
                validFaceCount = MIN(static_cast<int>(faces.size()), pvlFR->getMaxFacesInTracking());
            }
            else
            {
                validFaceCount = faces.size();
            }

            for (int i = 0; i < validFaceCount; i++)
            {
                validFaces.push_back(faces[i]);
            }

            //predict by pvl
            pvlFR->recognize(imgGray, validFaces, personIDs, confidence);

            //do face registration
            if (bRegist)
            {
                bRegist = false;
                //train pvl face
                for (uint i = 0; i < personIDs.size(); i++)
                {
                    if (personIDs[i] == FACE_RECOGNIZER_UNKNOWN_PERSON_ID)
                    {
                        int personID = pvlFR->createNewPersonID();
                        pvlFR->registerFace(imgGray, validFaces[i], personID, true);
                    }
                }

                if (!bTracking)
                {
                    pvlFR->recognize(imgGray, validFaces, personIDs, confidence); //to update screen using registered infomation
                }
            }
        }

        double elasped = static_cast<double>(getTickCount() - startTick) / getTickFrequency();
        startTick = getTickCount();

        //display FD & FR results on top of input image
        displayInfo(imgIn, faces, personIDs, confidence, resMode, elasped);
        imshow(WINDOW_NAME, imgIn);

        //handle key input
        char key = static_cast<char>(waitKey(keyDelay));
        if (key == 'q' || key == 'Q')       //escape
        {
            cout << "Quit." << endl;
            break;
        }
        else if (key == 'r' || key == 'R')  //train current unknown faces
        {
            cout << "Register." << endl;
            bRegist = true;
        }
        else if (key == 's' || key == 'S')  //save db xml file
        {
            cout << "Save. " << dbPath << endl;
            pvlFR->save(dbPath);
        }
        else if (key == 'l' || key == 'L')  //load db xml file
        {
            cout << "Load. " << dbPath << endl;
            bTracking = pvlFR->isTrackingModeEnabled();
            pvlFR = Algorithm::load<FaceRecognizer>(dbPath);
            pvlFR->setTrackingModeEnabled(bTracking);
        }

        if (resMode == E_RES_IMAGE_FILE)
        {
            //reload current image
            capture = VideoCapture(resFilePath);
        }

        //get next frame
        capture >> imgIn;
        if (imgIn.empty()) { break; }
        cvtColor(imgIn, imgGray, COLOR_BGR2GRAY);

    }

    destroyWindow(WINDOW_NAME);
}


static void roundedRectangle(Mat& src, const Point& topLeft, const Point& bottomRight,
    const Scalar lineColor, const int thickness, const int lineType, const int cornerRadius)
{
    /* corners:
    * p1 - p2
    * |     |
    * p4 - p3
    */
    Point p1 = topLeft;
    Point p2 = Point(bottomRight.x, topLeft.y);
    Point p3 = bottomRight;
    Point p4 = Point(topLeft.x, bottomRight.y);

    // draw straight lines
    line(src, Point(p1.x + cornerRadius, p1.y), Point(p2.x - cornerRadius, p2.y), lineColor, thickness, lineType);
    line(src, Point(p2.x, p2.y + cornerRadius), Point(p3.x, p3.y - cornerRadius), lineColor, thickness, lineType);
    line(src, Point(p4.x + cornerRadius, p4.y), Point(p3.x - cornerRadius, p3.y), lineColor, thickness, lineType);
    line(src, Point(p1.x, p1.y + cornerRadius), Point(p4.x, p4.y - cornerRadius), lineColor, thickness, lineType);

    // draw arcs
    ellipse(src, p1 + Point(cornerRadius, cornerRadius), Size(cornerRadius, cornerRadius), 180.0, 0, 90, lineColor, thickness, lineType);
    ellipse(src, p2 + Point(-cornerRadius, cornerRadius), Size(cornerRadius, cornerRadius), 270.0, 0, 90, lineColor, thickness, lineType);
    ellipse(src, p3 + Point(-cornerRadius, -cornerRadius), Size(cornerRadius, cornerRadius), 0.0, 0, 90, lineColor, thickness, lineType);
    ellipse(src, p4 + Point(cornerRadius, -cornerRadius), Size(cornerRadius, cornerRadius), 90.0, 0, 90, lineColor, thickness, lineType);
}


static void putTextCenter(InputOutputArray img, const String& text, Point& point, int fontFace,
    double fontScale, Scalar color, int thickness = 1, int lineType = LINE_8)
{
    int baseline;
    Size textSize = getTextSize(text, fontFace, fontScale, thickness, &baseline);

    Point center = point;
    center.x -= (textSize.width / 2);
    putText(img, text, center, fontFace, fontScale, color, thickness, lineType);
}


bool displayInfo(Mat& img, const vector<Face>& faces, const vector<int>& personIDs, const vector<int>& confidence, eResMode resMode, double elapsed)
{
    const int FONT = FONT_HERSHEY_PLAIN;
    const Scalar GREEN = Scalar(0, 255, 0, 255);
    const Scalar BLUE = Scalar(255, 0, 0, 255);

    Point pointLT, pointRB;
    String string;

    for (uint i = 0; i < faces.size(); i++)
    {
        const Face& face = faces[i];
        Rect faceRect = face.get<Rect>(Face::FACE_RECT);

        // Draw a face rectangle
        pointLT.x = faceRect.x;
        pointLT.y = faceRect.y;
        pointRB.x = faceRect.x + faceRect.width;
        pointRB.y = faceRect.y + faceRect.height;

        roundedRectangle(img, pointLT, pointRB, GREEN, 1, 8, 5);

        //draw FR info
        if (i < personIDs.size())
        {
            string.clear();

            if (personIDs[i] > 0)
            {
                string = format("Person:%d(%d)", personIDs[i], confidence[i]);
            }
            else if (personIDs[i] == FACE_RECOGNIZER_UNKNOWN_PERSON_ID)
            {
                string = "UNKNOWN";
            }
            else
            {
                //do nothing
            }

            pointLT.x += faceRect.width / 2;
            pointLT.y -= 2;
            putTextCenter(img, string, pointLT, FONT, 1.2, GREEN, 2);
        }
    }

    //print command
    pointLT.x = 2;
    pointLT.y = 16;
    string = "(Q)Quit";
    putText(img, string, pointLT, FONT, 1, BLUE, 1, 8);

    pointLT.y += 16;
    string = "(R)Register current unknown faces";
    putText(img, string, pointLT, FONT, 1, BLUE, 1, 8);

    pointLT.y += 16;
    string = "(L)load faces from xml";
    putText(img, string, pointLT, FONT, 1, BLUE, 1, 8);

    pointLT.y += 16;
    string = "(S)save faces into xml";
    putText(img, string, pointLT, FONT, 1, BLUE, 1, 8);

    //print fps
    if (resMode != E_RES_IMAGE_FILE)
    {
        const int FPS_MEASURE_INTERVAL = 30;
        static int fpsInterval = 0;
        static double fpsSum = 0;
        static double fps = 0;

        fpsSum += elapsed;

        if (fpsInterval++ == FPS_MEASURE_INTERVAL)
        {
            fps = 1.f / fpsSum * FPS_MEASURE_INTERVAL;

            fpsInterval = 0;
            fpsSum = 0;
        }

        pointLT.y = img.size().height - 7;
        string = format("fps:%.1f", fps);
        putText(img, string, pointLT, FONT, 1.2, BLUE, 2, 8);
    }

    return true;
}

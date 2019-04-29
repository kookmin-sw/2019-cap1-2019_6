// Copyright (C) 2019 Intel Corporation
// SPDX-License-Identifier: BSD-3-Clause

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/pvl.hpp"


using namespace cv;
using namespace std;


#define FACE_RECTANGLE (1 << 0)
#define EYE            (1 << 1)
#define MOUTH          (1 << 2)
#define BLINK          (1 << 3)
#define SMILE          (1 << 4)
#define ALL_FEATURES   (FACE_RECTANGLE | EYE | MOUTH | BLINK | SMILE)


class HaarFaceDetector
{
protected:
    class HaarFace
    {
    public:
        HaarFace(){};
        Rect faceRect;
        vector<Rect> eyeRects;
        vector<Rect> smileRects;
    };

public:
    HaarFaceDetector(const string& faceScript, const string& eyeScript, const string& smileScript) :
    faceRectColor(Scalar(0.0, 255.0, 0.0)),
    eyeRectColor(Scalar(255.0, 255.0, 0.0)),
    smileRectColor(Scalar(255.0, 0.0, 255.0))
    {
        faceDetector = makePtr<CascadeClassifier>(faceScript);
        eyeDetector = makePtr<CascadeClassifier>(eyeScript);
        smileDetector = makePtr<CascadeClassifier>(smileScript);

        CV_Assert(faceDetector->empty() == false);
        CV_Assert(eyeDetector->empty() == false);
        CV_Assert(smileDetector->empty() == false);
    }

    void detectAndDraw(Mat &frame, const string& windowName)
    {
        Mat original, gray;
        vector<HaarFace> results;

        original = frame.clone();

        // convert to grayscale image
        cvtColor(original, gray, COLOR_BGR2GRAY);
        equalizeHist(gray, gray);

        vector<Rect> faceRects;

        int64 start = getTickCount();

        // detect the faceRects of image
        faceDetector->detectMultiScale(gray, faceRects, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

        // detect the eyes of face
        for (uint i = 0; i < faceRects.size(); i++)
        {
            HaarFace result;
            Mat smallImgROI;

            const Rect& fr = faceRects[i];
            result.faceRect = fr;

            smallImgROI = original(fr);

            vector<Rect> eyes;

            eyeDetector->detectMultiScale(smallImgROI, eyes, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

            for (uint j = 0; j < eyes.size(); j++)
            {
                Rect& er = eyes[j];
                er.x += fr.x;
                er.y += fr.y;
                result.eyeRects.push_back(er);
            }

            vector<Rect> smiles;

            smileDetector->detectMultiScale(smallImgROI, smiles, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

            for (uint k = 0; k < smiles.size(); k++)
            {
                Rect& sm = smiles[k];
                sm.x += fr.x;
                sm.y += fr.y;
                result.smileRects.push_back(sm);
            }

            results.push_back(result);
        }

        double t = static_cast<double>(getTickCount() - start) / getTickFrequency() * 1000.0;

        drawRect(original, results);
        drawProcessTime(original, t);

        imshow(windowName, original);
    }

    virtual ~HaarFaceDetector()
    {}

private:
    void drawRect(Mat& image, vector<HaarFace>& results)
    {
        for (uint i = 0; i < results.size(); i++)
        {
            const HaarFace& r = results[i];
            rectangle(image, r.faceRect, faceRectColor, 2);

            for (uint j = 0; j < r.eyeRects.size(); j++)
            {
                rectangle(image, r.eyeRects[j], eyeRectColor, 2);
            }

            for (uint k = 0; k < r.smileRects.size(); k++)
            {
                rectangle(image, r.smileRects[k], smileRectColor, 2);
            }
        }
    }

    void drawProcessTime(Mat& image, double t)
    {
        int height = image.rows;

        putText(image, "Haarcascade", Point(0, height - 25), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 255), 2);

        stringstream ss(ios_base::app | ios_base::out);
        ss << fixed << setprecision(3) << t << " ms";
        putText(image, ss.str(), Point(0, height - 5), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 255), 2);
    }

private:
    Ptr<CascadeClassifier> faceDetector;
    Ptr<CascadeClassifier> eyeDetector;
    Ptr<CascadeClassifier> smileDetector;

    vector<Rect> faces;

    Scalar faceRectColor;
    Scalar eyeRectColor;
    Scalar smileRectColor;
};






class PvlFaceDetectorAdapter
{
public:
    PvlFaceDetectorAdapter(Ptr<pvl::FaceDetector> faceDetector, bool tracking = false)
        : faceRectColor(Scalar(0, 255.0, 0))
        , eyesColor(Scalar(0, 0, 255.0))
        , mouthColor(Scalar(255.0, 0, 0))
        , textColor(Scalar(255.0, 0, 255.0))
        , barColor(Scalar(0, 255.0, 255.0))
        , features(FACE_RECTANGLE)
    {
        CV_Assert(faceDetector);
        fd = faceDetector;
        fd->setTrackingModeEnabled(tracking);
    }

    virtual ~PvlFaceDetectorAdapter()
    {}

    void setTracking(bool tracking)
    {
        fd->setTrackingModeEnabled(tracking);
    }

    void setParameters(CommandLineParser& parser)
    {
        if (parser.has("rip"))
        {
            int rip = parser.get<int>("rip");

            if (rip > 180)
            {
                cout << "The rip value " << rip << " is changed by 180." << endl;
                rip = 180;
            }

            if (rip < 0)
            {
                cout << "The rip value " << rip << " is changed by 0." << endl;
                rip = 0;
            }

            fd->setRIPAngleRange(rip);
        }

        if (parser.has("rop"))
        {
            int rop = parser.get<int>("rop");

            if (rop > 90)
            {
                cout << "The rop value " << rop << " is changed by 90." << endl;
                rop = 90;
            }

            if (rop < 0)
            {
                cout << "The rop value " << rop << " is changed by 0." << endl;
                rop = 0;
            }

            fd->setROPAngleRange(rop);
        }

        if (parser.has("max-face"))
        {
            int numFaces = parser.get<int>("max-face");

            if (numFaces > pvl::MAX_SUPPORTED_FACES)
            {
                cout << "The max face value " << numFaces << " is changed by " << static_cast<int>(pvl::MAX_SUPPORTED_FACES) << "." << endl;
                numFaces = pvl::MAX_SUPPORTED_FACES;
            }

            if (numFaces < 1)
            {
                cout << "The max face value " << numFaces << " is changed by 1." << endl;
                numFaces = 1;
            }

            fd->setMaxDetectableFaces(numFaces);
        }
    }

    void detectAndDraw(Mat &frame, const string& windowName)
    {
        Mat original, gray;
        original = frame.clone();

        // convert to grayscale image
        cvtColor(original, gray, COLOR_BGR2GRAY);
        equalizeHist(gray, gray);

        vector<pvl::Face> faces;

        int64 start = getTickCount();

        if ((features & FACE_RECTANGLE) > 0)
        {
            fd->detectFaceRect(gray, faces);

            if ((features & EYE) > 0)
            {
                for (uint i = 0; i < faces.size(); ++i)
                    fd->detectEye(gray, faces[i]);

                if ((features & BLINK) > 0)
                {
                    for (uint i = 0; i < faces.size(); ++i)
                        fd->detectBlink(gray, faces[i]);
                }

                if ((features & SMILE) > 0)
                {
                    for (uint i = 0; i < faces.size(); ++i)
                        fd->detectSmile(gray, faces[i]);
                }
            }

            if ((features & MOUTH) > 0)
            {
                for (uint i = 0; i < faces.size(); ++i)
                    fd->detectMouth(gray, faces[i]);
            }
        }

        double t = static_cast<double>(getTickCount() - start) / getTickFrequency() * 1000.0;

        drawRect(original, faces);
        drawProcessTime(original, t);

        imshow(windowName, original);
    }

    void processKey(char c)
    {
        // enable/disable all features
        if (c == 'a' || c == 'A')
        {
            if (features == ALL_FEATURES)
                features = FACE_RECTANGLE;
            else
                features = ALL_FEATURES;
            return;
        }

        // enable/disable EYE_POS
        if (c == 'e' || c == 'E')
        {
            if (features & EYE)
                features ^= EYE;
            else
                features |= EYE;
            return;
        }

        // enable/disable MOUTH_POS
        if (c == 'm' || c == 'M')
        {
            if (features & MOUTH)
                features ^= MOUTH;
            else
                features |= MOUTH;
            return;
        }

        // enable/disable BLINK
        if (c == 'b' || c == 'B')
        {
            if (features & BLINK)
                features ^= BLINK;
            else
                features |= BLINK;
            return;
        }

        // enable/disable SMILE
        if (c == 's' || c == 'S')
        {
            if (features & SMILE)
                features ^= SMILE;
            else
                features |= SMILE;
            return;
        }
    }

    uint getFeatures()
    {
        return features;
    }

    void setFeatures(uint new_features)
    {
        features = new_features;
    }

private:
    void drawRect(Mat& image, vector<pvl::Face>& results)
    {
        for (uint i = 0; i < results.size(); i++)
        {
            pvl::Face& r = results[i];
            Rect faceRect = r.get<Rect>(pvl::Face::FACE_RECT);

            rectangle(image, faceRect, faceRectColor, 2);

            if (features & EYE)
            {
                circle(image, r.get<Point>(pvl::Face::LEFT_EYE_POS), 3, eyesColor, 2);
                circle(image, r.get<Point>(pvl::Face::RIGHT_EYE_POS), 3, eyesColor, 2);

                if (features & BLINK)
                {
#define MARGIN 5
                    int left_x = faceRect.x - MARGIN;
                    int left_height = faceRect.height / 100 * r.get<int>(pvl::Face::LEFT_BLINK_SCORE);

                    int right_x = faceRect.x + faceRect.width + MARGIN;
                    int right_height = faceRect.height / 100 * r.get<int>(pvl::Face::RIGHT_BLINK_SCORE);

                    int y = faceRect.y + faceRect.height;

                    line(image, Point(left_x, y), Point(left_x, y - left_height), barColor, 4);
                    stringstream left(ios_base::app | ios_base::out);
                    left << r.get<int>(pvl::Face::LEFT_BLINK_SCORE);
                    putText(image, left.str(), Point(left_x - 25, y), FONT_HERSHEY_PLAIN, 1, textColor, 2);

                    line(image, Point(right_x, y), Point(right_x, y - right_height), barColor, 4);
                    stringstream right(ios_base::app | ios_base::out);
                    right << r.get<int>(pvl::Face::RIGHT_BLINK_SCORE);
                    putText(image, right.str(), Point(right_x + 5, y), FONT_HERSHEY_PLAIN, 1, textColor, 2);
                }

                if (features & SMILE)
                {
                    int x = faceRect.x + 5;
                    int y = faceRect.y + faceRect.height + 15;

                    stringstream smile(ios_base::app | ios_base::out);
                    smile << r.get<int>(pvl::Face::SMILE_SCORE);
                    putText(image, "smile score", Point(x, y), FONT_HERSHEY_PLAIN, 1, textColor);
                    putText(image, smile.str(), Point(x, y + 20), FONT_HERSHEY_PLAIN, 1, textColor);
                }


            }

            if (features & MOUTH)
            {
                circle(image, r.get<Point>(pvl::Face::MOUTH_POS), 5, mouthColor, 2);
            }

        }
    }


    void drawProcessTime(Mat& image, double t)
    {
        int height = image.rows;

        putText(image, "pvl::FaceDetector", Point(0, height - 25), FONT_HERSHEY_PLAIN, 1, Scalar(0, 128, 255), 2);

        stringstream ss(ios_base::app | ios_base::out);
        ss << fixed << setprecision(3) << t << " ms";
        putText(image, ss.str(), Point(0, height - 5), FONT_HERSHEY_PLAIN, 1, Scalar(0, 128, 255), 2);
    }

private:
    Ptr<pvl::FaceDetector> fd;
    Scalar faceRectColor;
    Scalar eyesColor;
    Scalar mouthColor;
    Scalar textColor;
    Scalar barColor;
    uint features;
};


int main(int argc, char* argv[])
{
    const String keys =
        "{help h | | show help}"
        "{image | | this is target image to detect faces (non-tracking mode)}" // if image is exist, detects the faces of the single image.
        "{cascade | | this is the primary trained classifier such as frontal face (e.g. haarcascade_frontalface_alt2.xml)}"
        "{nested-cascade | | nested_cascade_path this an optional secondary classifier such as eyes (e.g. haarcascade_eye.xml)}"
        "{nested-cascade2 | | this an optional third classifier such as mouth (e.g. haarcascade_smile.xml)}"
        "{rip | | RIP(Rotation-In-Plane) angle range for face detection. the value is between 0 and 180}"
        "{rop | | ROP(Rotation-Out-Of-Plane) angle range for face detection. the value is between 0 and 90}"
        "{max-face | | the maximum number of detected faces. Max is MAX_SUPPORTED_FACES}"
    ;

    CommandLineParser parser(argc, argv, keys);
    parser.about(
        "This program demonstrates the pvl::FaceDetector. And you can compare with face detection using Haar cascades classifier.\n"
        "pvl::FaceDetector can detect faces from an image or image sequence of video capture device.\n"
        "This detects eyes, mouth, blink and smile. You can change serveral parameters when this program is running.");

    if (parser.has("help"))
    {
        parser.printMessage();
        return 0;
    }

    bool hasImage = parser.has("image");
    Ptr<HaarFaceDetector>  haarFaceDetector;
    if (parser.has("cascade"))
    {
        string face_script = parser.get<string>("cascade");
        string eyes_script = parser.get<string>("nested-cascade");
        string smile_script = parser.get<string>("nested-cascade2");
        haarFaceDetector = makePtr<HaarFaceDetector>(face_script, eyes_script, smile_script);
    }

    Ptr<pvl::FaceDetector> pvlFaceDetector = pvl::FaceDetector::create();
    PvlFaceDetectorAdapter faceDetectorAdapter(pvlFaceDetector, true);
    faceDetectorAdapter.setParameters(parser);

    if (hasImage)
    {
        string fileName = parser.get<string>("image");
        Mat frame = imread(fileName);

        imshow("original", frame);

        faceDetectorAdapter.setTracking(false);
        faceDetectorAdapter.setFeatures(ALL_FEATURES);
        faceDetectorAdapter.detectAndDraw(frame, "pvl::FaceDetector");

        if (haarFaceDetector)
            haarFaceDetector->detectAndDraw(frame, "Haar face detection");
    }
    else
    {
        VideoCapture cap;

        // open default video device.
        if (cap.open(0) == false)
        {
            cerr << "cannot open camera" << endl;
            return -1;
        }

        Mat frame, gray;
        Scalar face_rect_color(255.0, 0, 0);

        bool usePVL = true;

        cout << "During execution:\n";
        cout << "\tHit 'Q' key to quit.\n";
        cout << "\tHit 'A' key to enable/disable to detect all face landmarks.\n";
        cout << "\tHit 'E' key to enable/disable to detect eye position.\n";
        cout << "\tHit 'M' key to enable/disable to detect mouth position.\n";
        cout << "\tHit 'B' key to enable/disable to detect blink status.\n";
        cout << "\tHit 'S' key to enable/disable to detect smile status.\n";
        if (haarFaceDetector)
            cout << "\tHit 'P' key to switch between pvl::FaceDetector and Haar face detection.\n";
        cout << "\tUsing OpenCV version " << CV_VERSION << "\n" << endl;

        for (;;)
        {
            cap >> frame;

            if (frame.empty())
                continue;

            if (usePVL)
            {
                faceDetectorAdapter.detectAndDraw(frame, "result");
            }
            else if (haarFaceDetector)
            {
                haarFaceDetector->detectAndDraw(frame, "result");
            }

            char c = (char)waitKey(1);

            if (c == 27 || c == 'q' || c == 'Q')
                return false;

            if (haarFaceDetector && (c == 'p' || c == 'P'))
            {
                usePVL = !usePVL;
            }

            faceDetectorAdapter.processKey(c);

        }
    }

    destroyAllWindows();

    return 0;
}

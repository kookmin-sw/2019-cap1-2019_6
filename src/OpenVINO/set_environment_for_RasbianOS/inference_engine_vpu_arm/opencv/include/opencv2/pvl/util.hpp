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


#ifndef __OPENCV_PVL_UTIL_HPP__
#define __OPENCV_PVL_UTIL_HPP__


#include "opencv2/core.hpp"
#include "pvl_types.h"


namespace cv
{
namespace pvl
{


/// @cond
class Util
{
public:
    Util();
    ~Util();


public:
    static void matToPvlImage(const cv::Mat& mat, pvl_image& image);
    static void pvlPointToCvPoint(const pvl_point& pvlPoint, cv::Point& cvPoint);
    static void cvPointToPvlPoint(const cv::Point& cvPoint, pvl_point& pvlPoint);
    static void pvlRectToCvRect(const pvl_rect& pvlRect, cv::Rect& cvRect);
    static void cvRectToPvlRect(const cv::Rect& cvRect, pvl_rect& pvlRect);
};


template<typename _Tp>
class ArrayDeleter
{
public:
    void operator()(_Tp* p) const
    {
        delete[] p;
    }
};


template<typename _Tp>
cv::Ptr<_Tp> makeArrayPtr(unsigned int size)
{
    return cv::Ptr<_Tp>(new _Tp[size], ArrayDeleter<_Tp>());
}


template<typename _Tp>
cv::Ptr<_Tp> wrapArrayPtr(_Tp* p)
{
    return cv::Ptr<_Tp>(p, ArrayDeleter<_Tp>());
}
/// @endcond


#define PVL_ERROR(COND, CODE, MSG)                                  \
    {                                                               \
        if (!!(COND));                                              \
        else                                                        \
            CV_Error((CODE), "Assertion: " #COND " failed. " #MSG); \
    }


};
};


#endif

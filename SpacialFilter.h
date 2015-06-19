//
//  SpacialFilter.h
//  EVM
//
//  Created by DarkTango on 6/1/15.
//  Copyright (c) 2015 DarkTango. All rights reserved.
//

#ifndef __EVM__SpacialFilter__
#define __EVM__SpacialFilter__
#include "utility.h"


class SpacialFilter{
public:
    //build Gaussian pyramid for color magnify.
    static void buildGuassianPyramid(const Mat& img, const int level, vector<Mat>& pyramid);
    
    //build Laplacian pyramid for motion magnify.
    static void buildLaplacianPyramid(const Mat& img, const int level, vector<Mat>& pyramid);
    
    static void reconImgFromLaplacianPyramid(const std::vector<cv::Mat> &pyramid,const int levels,cv::Mat &dst);
    
    static void upsamplingFromGaussianPyramid(const cv::Mat &src, const int levels, cv::Mat &dst);
    
};







#endif /* defined(__EVM__SpacialFilter__) */

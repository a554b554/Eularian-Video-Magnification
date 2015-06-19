//
//  SpacialFilter.cpp
//  EVM
//
//  Created by DarkTango on 6/1/15.
//  Copyright (c) 2015 DarkTango. All rights reserved.
//

#include "SpacialFilter.h"
void SpacialFilter::buildGuassianPyramid(const Mat& img, const int level, vector<Mat>& pyramid){
    assert(level>1);
    pyramid.clear();
    Mat curr = img;
    for (int i = 0; i < level; i++) {
        Mat down;
        pyrDown(curr, down);
        pyramid.push_back(down);
        curr = down;
    }
    //pyramid.push_back(curr);
}

void SpacialFilter::buildLaplacianPyramid(const cv::Mat &img, const int level, vector<cv::Mat> &pyramid){
    assert(level>=1);
    pyramid.clear();
    cv::Mat currentImg = img;
    for (int l=0; l<level; l++) {
        cv::Mat down,up;
        pyrDown(currentImg, down);
        pyrUp(down, up, currentImg.size());
        cv::Mat lap = currentImg - up;
        
        //waitKey(0);
        pyramid.push_back(lap);
        currentImg = down;
    }
    pyramid.push_back(currentImg);
}

void SpacialFilter::reconImgFromLaplacianPyramid(const std::vector<cv::Mat> &pyramid, const int levels, cv::Mat &dst){
    cv::Mat currentImg = pyramid[levels];
    for (int i = levels - 1; i >= 0; i--) {
        Mat up;
        pyrUp(currentImg, up, pyramid[i].size());
        //imshow("before", up);
        currentImg = up + pyramid[i];

        //imshow("after", currentImg);
        //waitKey(0);
    }
    dst = currentImg.clone();
}

void SpacialFilter::upsamplingFromGaussianPyramid(const cv::Mat &src, const int levels, cv::Mat &dst){
    cv::Mat currentLevel = src.clone();
    for (int i = 0; i < levels; ++i) {
        cv::Mat up;
        cv::pyrUp(currentLevel, up);
        currentLevel = up;
        
    }
    currentLevel.copyTo(dst);
}
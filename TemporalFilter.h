//
//  TemporalFilter.h
//  EVM
//
//  Created by DarkTango on 6/6/15.
//  Copyright (c) 2015 DarkTango. All rights reserved.
//

#ifndef __EVM__TemporalFilter__
#define __EVM__TemporalFilter__

#include "utility.h"



class TemporalFilter{
public:
    void idealFilter(const Mat& src, const double wl, const double wh, const double fps, Mat& dst);
    void IIRFilter(const Mat& src, const double wl, const double wh, Mat& lowpass1, Mat& lowpass2, Mat& dst);
    void concatenateImg(const vector<Mat>& imgs, Mat& dst);
    void decomposeImg(const Mat& src, const int num, const Size& frameSize,vector<Mat>& dst);
    void amplify(const Mat& src, Mat& dst, double alpha);
   
    
private:
    void createIdealFilter(Mat& filter, const double wl, const double wh, const double fps);
    void mulFilter(const Mat& filter, Mat& img);

};

#endif /* defined(__EVM__TemporalFilter__) */
//void VideoProcessor::temporalIIRFilter(const cv::Mat &src,
//                                       cv::Mat &dst)
//{
//    cv::Mat temp1 = (1-fh)*lowpass1[curLevel] + fh*src;
//    cv::Mat temp2 = (1-fl)*lowpass2[curLevel] + fl*src;
//    lowpass1[curLevel] = temp1;
//    lowpass2[curLevel] = temp2;
//    dst = lowpass1[curLevel] - lowpass2[curLevel];
//}

//if (fnumber == 0){      // is first frame
//    lowpass1 = pyramid;
//    lowpass2 = pyramid;
//    filtered = pyramid;
//} else {
//    for (int i=0; i<levels; ++i) {
//        curLevel = i;
//        temporalFilter(pyramid.at(i), filtered.at(i));
//    }
//
//    // amplify each spatial frequency bands
//    // according to Figure 6 of paper
//    cv::Size filterSize = filtered.at(0).size();
//    int w = filterSize.width;
//    int h = filterSize.height;
//
//    delta = lambda_c/8.0/(1.0+alpha);
//    // the factor to boost alpha above the bound
//    // (for better visualization)
//    exaggeration_factor = 2.0;
//
//    // compute the representative wavelength lambda
//    // for the lowest spatial frequency band of Laplacian pyramid
//    lambda = sqrt(w*w + h*h)/3;  // 3 is experimental constant
//
//    for (int i=levels; i>=0; i--) {
//        curLevel = i;
//
//        amplify(filtered.at(i), filtered.at(i));
//
//        // go one level down on pyramid
//        // representative lambda will reduce by factor of 2
//        lambda /= 2.0;
//    }
//}
//
//// 4. reconstruct motion image from filtered pyramid
//reconImgFromLaplacianPyramid(filtered, levels, motion);
//
//// 5. attenuate I, Q channels
//attenuate(motion, motion);


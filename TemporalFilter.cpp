//
//  TemporalFilter.cpp
//  EVM
//
//  Created by DarkTango on 6/6/15.
//  Copyright (c) 2015 DarkTango. All rights reserved.
//

#include "TemporalFilter.h"
#include "VideoProcessor.h"
void TemporalFilter::idealFilter(const cv::Mat &src, const double wl, const double wh, const double fps, cv::Mat &dst){
    Mat rgb[3];
    split(src, rgb);
    //split the src image into 3 channels.
    Mat before;
    normalize(src, before, 0, 1, CV_MINMAX);
//    imshow("before", before);
    for (int i = 0 ; i < 3; i++) {
        
        Mat curr = rgb[i];
        Mat tmp;
        int width = getOptimalDFTSize(curr.cols);
        int height = getOptimalDFTSize(curr.rows);
        
        copyMakeBorder(curr, tmp, 0, height-curr.rows, 0, width-curr.cols, BORDER_CONSTANT, Scalar::all(0));
        tmp = Mat_<float>(tmp);
        //imshow("tmp", tmp);
        // apply dft
//        EVM::showFrame(tmp, "before dft");
        
        dft(tmp, tmp, DFT_ROWS|DFT_SCALE);
        
//        EVM::showFrame(tmp, "after dft");
        
        Mat filter = tmp.clone();
        createIdealFilter(filter, wl, wh, fps);
//        mulSpectrums(tmp, filter, tmp, DFT_ROWS);
//        cout<<tmp;
        mulFilter(filter, tmp);
        //cout<<tmp;
//        EVM::showFrame(tmp, "after filtering");
        idft(tmp, tmp, DFT_ROWS|DFT_SCALE);
//        EVM::showFrame(tmp, "after idft");
//        cout<<tmp;
        
        tmp(Rect(0,0,curr.cols,curr.rows)).copyTo(rgb[i]);
        
    }
    merge(rgb, 3, dst);
    normalize(dst, dst, 0, 1, CV_MINMAX);

    //imshow("after", dst);

}

void TemporalFilter::IIRFilter(const cv::Mat &src, const double wl, const double wh, Mat &lowpass1, Mat& lowpass2, cv::Mat &dst){
    assert(src.type()==CV_32FC3);
    assert(lowpass1.type()==CV_32FC3);
    assert(lowpass2.type()==CV_32FC3);
//    EVM::showFrame(src, "src");
//    EVM::showFrame(lowpass1, "lowpass1");
//    EVM::showFrame(lowpass2, "lowpass2");
    Mat tmp1 = (1-wh)*lowpass1 + wh*src;
    Mat tmp2 = (1-wl)*lowpass1 + wl*src;
    lowpass1 = tmp1;
    lowpass2 = tmp2;
    dst = lowpass1 - lowpass2;
//    EVM::showFrame(dst, "Filtered");
//    waitKey(0);
}




void TemporalFilter::createIdealFilter(cv::Mat &filter, const double wl, const double wh, const double fps){
    int timeline = filter.cols;
    int pixel = filter.rows;
    // wk = 2*pi*k/(timeline*samplingRate), k = wk*timeline*samplingRate/(2*pi)
    
    double fl = 2 * wl * timeline / fps;
    double fh = 2 * wh * timeline / fps;
    double response;
    cout<<"low: "<<fl<<" high: "<<fh<<endl;
    for (int i = 0; i < pixel; i++) {
        for (int j = 0; j < timeline; j++) {
            if (j <= fh && j >= fl) {
                response = 1.0f;
            }
            else{
                response = 0.0f;
            }
            filter.at<float>(i,j) = response;
        }
    }
}


// compress image into one frame in order to apply dft.
void TemporalFilter::concatenateImg(const vector<cv::Mat> &imgs, cv::Mat &dst){
    Size framesize = imgs[0].size();
   // imshow("test", imgs[0]);
    Mat tmp(framesize.width*framesize.height, imgs.size(), CV_32FC3);
    for (int i = 0; i < imgs.size(); i++) {
        Mat input = imgs[i];
        Mat reshaped =  input.reshape(3, input.cols*input.rows).clone();
        //cout<<reshaped;
        reshaped.copyTo(tmp.col(i));
    }
    tmp.copyTo(dst);
}


// decompost image from one frame.

void TemporalFilter::decomposeImg(const cv::Mat &src, const int num, const Size& frameSize ,vector<cv::Mat> &dst){
    dst.clear();
    for (int i = 0; i < num; i++) {
        Mat line = src.col(i).clone();
        Mat reshaped = line.reshape(3, frameSize.height).clone();
        dst.push_back(reshaped);
        
    }
}


void TemporalFilter::amplify(const Mat& src, Mat& dst, double alpha){
    dst = src * alpha;
}


void TemporalFilter::mulFilter(const cv::Mat &filter, cv::Mat &img){
    assert(filter.size==img.size&&filter.type()==CV_32FC1&&img.type()==CV_32FC1);
    Point p;
//    EVM::showFrame(filter, "FILTER");
    for (p.y = 0; p.y < img.rows; p.y++) {
        for (p.x = 0; p.x < img.cols; p.x++) {
           // cout<<"before"<<img.at<float>(p)<<" filter:"<<filter.at<float>(p);
            img.at<float>(p) = filter.at<float>(p)*img.at<float>(p);
            //cout<<" final:"<<img.at<float>(p)<<endl;
        }
    }
//    EVM::showFrame(img, "filtered");
}




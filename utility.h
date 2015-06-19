//
//  utility.h
//  EVM
//
//  Created by DarkTango on 6/1/15.
//  Copyright (c) 2015 DarkTango. All rights reserved.
//

#pragma once
#ifndef EVM_utility_h
#define EVM_utility_h
#include <stdio.h>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/photo/photo.hpp>
#include <string>
#include <fstream>
using namespace cv;
using namespace std;
enum{
    SPATIAL_LAPLACIAN,
    SPATIAL_GAUSSIAN,
};

enum{
    TEMPORAL_IIR,
    TEMPORAL_IDEAL
};

enum{
    MOTION,
    COLOR
};



namespace EVM {
    static void rbg2yiq(const Mat& src, Mat& dst){
        Mat ret(src.rows,src.cols,CV_32FC3);
        for (int j=0; j<src.rows; j++) {
            for (int i=0; i<src.cols; i++) {
                Vec3f bgr = src.at<Vec3b>(j, i);
                Vec3f yiq;
                yiq[0] = 0.114*bgr[0] + 0.587*bgr[1] + 0.299*bgr[2];
                yiq[1] = -0.321*bgr[0] - 0.275*bgr[1] + 0.596*bgr[2];
                yiq[2] = 0.311*bgr[0] - 0.523*bgr[1] + 0.212*bgr[2];
                ret.at<Vec3f>(j,i) = yiq;
            }
        }
        dst = ret;
        //cout<<dst;
    }
    
    static void yiq2rgb(const Mat& src, Mat& dst)
    {
        Mat ret(src.rows,src.cols,CV_8UC3);
        for (int j=0; j<src.rows; j++) {
            for (int i=0; i<src.cols; i++) {
                Vec3f yiq = src.at<Vec3f>(j,i);
                Vec3b bgr;
                bgr[0] =  yiq[0] - 1.108*yiq[1] + 1.705*yiq[2];
                bgr[1] = yiq[0] - 0.272*yiq[1] - 0.647*yiq[2];
                bgr[2] = yiq[0] + 0.956*yiq[1] + 0.621*yiq[2];
                ret.at<Vec3b>(j,i) = bgr;
            }
        }
        dst = ret;
    }

    static string toString(int str){
        stringstream ss;
        string ans;
        ss<<str;
        ss>>ans;
        return ans;
    }
    
    static void showFrame(const Mat& frame, const string winName){
        assert(frame.type()==CV_8UC3||frame.type()==CV_32FC3||frame.type()==CV_32FC1||frame.type()==CV_8UC1);
        if (frame.type()==CV_8UC1||frame.type()==CV_8UC3) {
            imshow(winName, frame);
        }
        else{
            Mat tmp;
            normalize(frame, tmp,0 ,1, CV_MINMAX);
            imshow(winName, tmp);
        }
    }
    
    static void showPyramid(const vector<Mat>& pyramid){
        for (int i = 0; i < pyramid.size(); i++) {
//            imshow("Pyramid"+toString(i), pyramid[i]);
            showFrame(pyramid[i],"Pyramid"+toString(i));
        }
    }
}

static void loadimglist(const string listname, vector<Mat>& imglist){
    fstream f;
    f.open(listname);
    string a;
    imglist.clear();
    string base = listname.substr(0,listname.find('/',2)+1);
    cout<<base;
    while (f>>a) {
        Mat tmp = imread(base+a);
        imglist.push_back(tmp);
    }
}

#endif

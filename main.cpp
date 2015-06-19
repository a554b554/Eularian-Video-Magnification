//
//  main.cpp
//  EVM
//
//  Created by DarkTango on 5/30/15.
//  Copyright (c) 2015 DarkTango. All rights reserved.
//
// this is an application for demostrating Eularian Video Magnification


#include "SpacialFilter.h"
#include "TemporalFilter.h"
#include "utility.h"
#include "VideoProcessor.h"
#include "App.h"
#include "possion/PoissonBlending.h"

App app;

static void on_mouse(int event, int x, int y, int flag, void* param){
    
    app.mouseClick(event, x, y, flag, param);
}

int main(int argc, const char * argv[]){

  //  Mat A =  Mat::eye(50000, 50000, CV_64FC1);
    const string winName = "APP";
    namedWindow(winName, CV_WINDOW_AUTOSIZE);
    setMouseCallback(winName, on_mouse, 0);
    app.setWinName(winName);
    app.start();
    
   

    return 0;
}


//unit test
int mainpois(int argc, const char * argv[]) {
    Mat src = imread("src.jpg");
    Mat tag = imread("tag.jpg");
    Mat mask(src.rows, src.cols, CV_8UC1);
    mask.setTo(0);
    Rect roi(449,77,365,420);
    mask(roi).setTo(255);
    imshow("mask", mask);
    imshow("src", src);
    imshow("tag", tag);
    Mat dst;

    Blend::PoissonBlender b(src, tag, mask);
    b.seamlessClone(dst, 0, 0);
    imshow("dst", dst);
    waitKey(0);
    
    
    return 0;
}




int main1(int argc, const char * argv[]){
    // insert code here...
    Mat I = imread("4.jpg");
    if( I.empty())
        return -1;
    
    Mat padded;                            //expand input image to optimal size
    int m = getOptimalDFTSize( I.rows );
    int n = getOptimalDFTSize( I.cols ); // on the border add zero values
    copyMakeBorder(I, padded, 0, m - I.rows, 0, n - I.cols, BORDER_CONSTANT, Scalar::all(0));
    
    Mat planes[] = {Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F)};
    Mat complexI;
    merge(planes, 2, complexI);         // Add to the expanded another plane with zeros
    imshow("sdf", padded);
    dft(complexI, complexI, DFT_ROWS|DFT_SCALE);            // this way the result may fit in the source matrix
    
    // compute the magnitude and switch to logarithmic scale
    // => log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))
    split(complexI, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
    magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude
    Mat magI = planes[0];
    
    magI += Scalar::all(1);                    // switch to logarithmic scale
    log(magI, magI);
    
    // crop the spectrum, if it has an odd number of rows or columns
    magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));
    
    // rearrange the quadrants of Fourier image  so that the origin is at the image center
    int cx = magI.cols/2;
    int cy = magI.rows/2;
    
    Mat q0(magI, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
    Mat q1(magI, Rect(cx, 0, cx, cy));  // Top-Right
    Mat q2(magI, Rect(0, cy, cx, cy));  // Bottom-Left
    Mat q3(magI, Rect(cx, cy, cx, cy)); // Bottom-Right
    
    Mat tmp;                           // swap quadrants (Top-Left with Bottom-Right)
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);
    
    q1.copyTo(tmp);                    // swap quadrant (Top-Right with Bottom-Left)
    q2.copyTo(q1);
    tmp.copyTo(q2);
    
    normalize(magI, magI, 0, 1, CV_MINMAX); // Transform the matrix with float values into a
    // viewable image form (float between values 0 and 1).
    
    imshow("Input Image"       , I   );    // Show the result
    imshow("spectrum magnitude", magI);
    waitKey();
    
    return 0;
}
//
//  VideoProcessor.h
//  EVM
//
//  Created by DarkTango on 6/15/15.
//  Copyright (c) 2015 DarkTango. All rights reserved.
//

#ifndef __EVM__VideoProcessor__
#define __EVM__VideoProcessor__

#include "SpacialFilter.h"
#include "TemporalFilter.h"
#include "utility.h"
#include "PoissonImageEditing.h"

class Video{
public:
    Video(vector<Mat>& frames, double framerate);
    Video(const string listname, double framerate);
    Video();
    Size framesize;
    int framelength;
    double framerate;
    vector<Mat> frames;
    void addframe(const Mat& frame);
    void play();
    void writeVideo();
    void writeVideoAsImg();
    void clear();
    void combineVideo(const Video& origin, const Video& roi, const Rect& rect, bool usePoisson=false);
    string videoname;
private:
    int writeCount = 0;
};

class VideoProcessor{
public:
    int pyramidlevel = 4;
    VideoProcessor();
    VideoProcessor(Video& inputvideo);
    VideoProcessor(const string listname);
    void processVideo(int type);
    void processVideo(Video& inputvideo, int type);
    void splitVideo();
    void magnify();
    void motionMagnify();
    void colorMagnify();
    void showoutput();
    void writeOutput();
    void writeInput();
    void rgb2yiq();
    void yiq2rgb();
    void attenuate(const Mat& src, Mat& dst);
    void copyOutputTo(Video& output);
    void clearOutput();
    static void showframe(const Mat& frame, const string winName);
    
    ~VideoProcessor();
private:
    int processType;
    Video* video;
    vector<Video*> mid;
    Video* outputvideo;
    double framerate = 16;
    double wl = 0.5;
    double wh = 1;
    double lambda_c = 16;
    double alpha = 50;
    double chromAttenuation = 0.1;
};





#endif /* defined(__EVM__VideoProcessor__) */

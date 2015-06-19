//
//  VideoProcessor.cpp
//  EVM
//
//  Created by DarkTango on 6/15/15.
//  Copyright (c) 2015 DarkTango. All rights reserved.
//

#include "VideoProcessor.h"

Video::Video(const string listname, double framerate){
    Video();
    loadimglist(listname, frames);
    framesize = frames[0].size();
    framelength = (int)frames.size();
    this->framerate = framerate;
}

Video::Video(vector<Mat>& frames, double framerate){
    Video();
    this->frames = frames;
    this->framerate = framerate;
    this->framelength = (int)frames.size();
    this->framesize = frames[0].size();
}

Video::Video(){
    framelength = 0;
    videoname = EVM::toString(arc4random()/300);
}

void Video::addframe(const cv::Mat &frame){
    if (framelength == 0) {
        framesize = frame.size();
    }
    else{
        assert(framesize == frame.size());
    }
    framelength++;
    frames.push_back(frame);
}

void Video::play(){
    Mat frame;
    double maxV,minV;
    for (int i = 0; i < frames.size(); i++) {
        if(frames[i].type()!=CV_8UC3){
            minMaxLoc(frames[i], &minV, &maxV);
            frames[i].convertTo(frame, CV_8UC3, 255.0/(maxV-minV), -minV*255.0/(maxV-minV));
        }
        else{
            frame = frames[i];
        }
        imshow("play", frame);
        waitKey(1);
    }
}

void Video::writeVideo(){
    string command = "./ffmpeg -framerate "+EVM::toString(framerate)+" -i ./tmp/"+videoname+"_%d.jpg -c:v libx264 -vf "+"\"fps="+EVM::toString(framerate)+",format=yuv420p\" "+videoname+".mp4";
    cout<<command<<endl;
    system(command.c_str());
}

void Video::writeVideoAsImg(){
    for (int i = 0; i < framelength; i++) {
        imwrite("tmp/"+videoname+"_"+EVM::toString(writeCount)+".jpg", frames[i]);
        writeCount++;
    }
}

void Video::clear(){
    frames.clear();
    framelength = 0;    
}

void Video::combineVideo(const Video &origin, const Video &roi, const Rect &rect, bool usePoisson){
    assert(rect.width==roi.framesize.width);
    assert(rect.height==roi.framesize.height);
    for (int i = 0; i < roi.framelength; i++) {
        Mat out = origin.frames[i].clone();
//        cvtColor(roi.frames[i], roi.frames[i], CV_Lab2BGR);
//        roi.frames[i].convertTo(roi.frames[i], CV_8UC3, 255.0, 1.0/255.0);
//        EVM::showFrame(roi.frames[i], "roi");
        Mat in = roi.frames[i].clone();
        if(usePoisson){
//            Mat tmp;
//            Rect a(0,0,in.rows,in.cols);
//            tmp = poisson_blending(in, out, a, rect.x, rect.y);
//            out = tmp;
        }
        else{
            roi.frames[i].copyTo(out(rect));
        }
        
        EVM::showFrame(out, "finalresult");
        
        this->addframe(out);
    }
}


/////////////////////////////////////////////////////////////

VideoProcessor::VideoProcessor(){
    outputvideo = new Video();
}


VideoProcessor::VideoProcessor(Video& inputvideo){
    this->video = &inputvideo;
    this->framerate = this->video->framerate;
    outputvideo = new Video();
    outputvideo->framerate = framerate;
    outputvideo->videoname = inputvideo.videoname;
}

void VideoProcessor::processVideo(Video& inputvideo, int type){
    video = &inputvideo;
    this->framerate = this->video->framerate;
    mid.clear();
    outputvideo->videoname = inputvideo.videoname;
    int64 tick = getTickCount();
    processVideo(type);
    cout<<"processing time: "<<(getTickCount()-tick)/getTickFrequency()<<endl;
}

void VideoProcessor::processVideo(int type){
    processType = type;
    switch (type) {
        case COLOR:
            //rgb2yiq();
            splitVideo();
            magnify();
            // yiq2rgb();
            //outputvideo->writeVideoAsImg();
            //outputvideo->clear();
            break;
            
        case MOTION:
            splitVideo();
            magnify();
            //outputvideo->writeVideoAsImg();
            //outputvideo->clear();
            break;
    }
}


void VideoProcessor::magnify(){
    if (processType==COLOR) {
        colorMagnify();
    }
    else if (processType==MOTION){
        motionMagnify();
    }
}

void VideoProcessor::colorMagnify(){
    vector<Mat> out;
    TemporalFilter tem;
    Mat concatened,filtered;
    tem.concatenateImg(mid[pyramidlevel-1]->frames, concatened);
    tem.idealFilter(concatened, wl, wh, video->framerate, filtered);
    tem.amplify(filtered, filtered, alpha);
    tem.decomposeImg(filtered, video->framelength, mid[pyramidlevel-1]->framesize, out);
    Mat tmp, motion,outputframe;
    
    for (int i = 0; i < out.size(); i++) {
        SpacialFilter::upsamplingFromGaussianPyramid(out[i], pyramidlevel, motion);
        double minV,maxV;
        
        resize(motion, motion, video->frames[i].size());
        //            if (video->frames[i].type()==CV_8UC3) {
        //                minMaxLoc(motion, &minV, &maxV);
        //                motion.convertTo(motion, CV_8UC3, 255.0/(maxV-minV), -minV*255.0/(maxV-minV));
        //            }
        
        tmp = video->frames[i] + motion;
        outputframe = tmp.clone();
        minMaxLoc(outputframe, &minV, &maxV);
        outputframe.convertTo(outputframe, CV_8UC3, 255.0/(maxV-minV), -minV*255.0/(maxV-minV));
        outputvideo->addframe(outputframe);
    }

}

void VideoProcessor::motionMagnify(){
    Mat motion,out;
    TemporalFilter tem;
    vector<Mat> filtered;
    vector<Mat> py;
    vector<Mat> lowpass1,lowpass2;
    for (int i = 0; i < mid.size(); i++) {
        py.push_back(mid[i]->frames[0]);
    }
    lowpass1 = py;
    lowpass2 = py;
    filtered = py;
//    EVM::showPyramid(py);
//    waitKey(0);
    out = video->frames[0].clone();
    cvtColor(out, out, CV_Lab2BGR);
    out.convertTo(out, CV_8UC3, 255.0, 1.0/255.0);
    outputvideo->addframe(out);
    for (int i = 1; i < video->framelength; i++) {
        for (int j = 0; j <= pyramidlevel; j++) {
            tem.IIRFilter(mid[j]->frames[i], wl, wh, lowpass1[j], lowpass2[j], filtered[j]);
//            mid[j]->play();
//            EVM::showFrame(mid[j]->frames[i], "mid");
//            EVM::showFrame(filtered[j], "filtered");
//            waitKey(0);
        }
//        EVM::showPyramid(filtered);
        Size filterSize = filtered[0].size();
        int w = filterSize.width;
        int h = filterSize.height;
        
        double delta = lambda_c/8/(1+alpha);
        double exaggeration_factor = 2;
        double lambda = sqrt(w*w+h*h)/3;
        for (int j = pyramidlevel - 1; j > 0; j--) {
            double currAlpha = lambda/delta/8 - 1;
            currAlpha *= exaggeration_factor;
//            cout<<"curr alpha for level: "<<j<<" is "<<min(currAlpha, alpha)<<endl;
            tem.amplify(filtered[j], filtered[j], min(currAlpha, alpha));
            lambda /= 2.0;
        }
    
        filtered[0] = filtered[0]*0;
        filtered[pyramidlevel] = filtered[pyramidlevel]*0;
//        EVM::showPyramid(filtered);
        SpacialFilter::reconImgFromLaplacianPyramid(filtered, pyramidlevel, motion);
        attenuate(motion, motion);
        
//        EVM::showFrame(motion, "motion");
        
        Mat tmp = video->frames[i] + motion;

        //EVM::showFrame(video->frames[i], "videoframe");
        
        out = tmp.clone();
        cvtColor(out, out, CV_Lab2BGR);
        out.convertTo(out, CV_8UC3, 255.0, 1.0/255.0);
        
        
//        blur(out, out, Size(5,5));
        medianBlur(out, out, 5);
        
        EVM::showFrame(out, "final");
        outputvideo->addframe(out);
        waitKey(1);
    }
}




void VideoProcessor::splitVideo(){
    int length = this->video->framelength;
    for (int i = 0; i < pyramidlevel; i++) {
        Video* _video = new Video();
        _video->framerate = video->framerate;
        mid.push_back(_video);
    }
    
    int n = pyramidlevel;
    if (processType==MOTION){
        n = n + 1;
        Video* _video = new Video();
        _video->framerate = video->framerate;
        mid.push_back(_video);
    }
    
    for (int i = 0; i < length; i++) {
        Mat curr = video->frames[i];
        vector<Mat> pyramid;
        if (processType == COLOR) {
            SpacialFilter::buildGuassianPyramid(curr, pyramidlevel, pyramid);
        }
        else if (processType == MOTION){
            SpacialFilter::buildLaplacianPyramid(curr, pyramidlevel, pyramid);
//            EVM::showPyramid(pyramid);
//            waitKey(0);
        }
        for (int j = 0; j < n; j++) {
            mid[j]->addframe(pyramid[j]);
        }
        
    }
    
//    for (int i = 0; i < mid.size(); i++) {
//        mid[i] -> play();
//    }
 
}


void VideoProcessor::showoutput(){
    outputvideo->play();
    return;
}

void VideoProcessor::writeOutput(){
    outputvideo->writeVideo();
    return;
}

void VideoProcessor::writeInput(){
    video->writeVideo();
    return;
}

void VideoProcessor::rgb2yiq(){
    int length = video->framelength;
    for (int i = 0; i < length; i++) {
       // showframe(video->frames[i], "before");
        EVM::rbg2yiq(video->frames[i], video->frames[i]);
        //showframe(video->frames[i], "after");
    }
}

void VideoProcessor::attenuate(const cv::Mat &src, cv::Mat &dst){
    Mat channel[3];
    split(src, channel);
    channel[1] = channel[1] * chromAttenuation;
    channel[2] = channel[2] * chromAttenuation;
    merge(channel, 3, dst);
}

void VideoProcessor::showframe(const cv::Mat &frame, const string winName){
    Mat show;
    if (frame.type()!=CV_8UC3) {
        double minV,maxV;
        minMaxLoc(frame, &minV, &maxV);
        frame.convertTo(show, CV_8UC3, 255.0/(maxV-minV), -minV*255.0/(maxV-minV));
    }
    else{
        show = frame;
    }
    imshow(winName, show);
}


void VideoProcessor::yiq2rgb(){
    int length = outputvideo->framelength;
    for (int i = 0; i < length; i++) {
       // showframe(outputvideo->frames[i], "before");
        EVM::yiq2rgb(outputvideo->frames[i], outputvideo->frames[i]);
       // showframe(outputvideo->frames[i], "after");
    }
}

void VideoProcessor::copyOutputTo(Video &output ){
    output.framerate = outputvideo->framerate;
    //this->outputvideo->play();
    for (int i = 0; i < outputvideo->framelength; i++) {
        output.addframe(outputvideo->frames[i]);
//        EVM::showFrame(video->frames[i], "debug");
//        waitKey(0);
    }
}



void VideoProcessor::clearOutput(){
    outputvideo->clear();
}

VideoProcessor::~VideoProcessor(){
    //delete video;
    for (int i = 0; i < mid.size(); i++) {
        delete mid[i];
    }
    delete outputvideo;
}










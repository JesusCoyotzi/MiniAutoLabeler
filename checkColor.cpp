#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/videoio.hpp>
#include <stdlib.h>
#include <iostream>
#include <string>

const std::string win_title="Feed";
const std::string seg_title="Segmentation";
const std::string msk = "Mask";
bool compareContourAreas ( std::vector<cv::Point> contour1, std::vector<cv::Point> contour2 ) {
        double i = fabs( contourArea(cv::Mat(contour1)) );
        double j = fabs( contourArea(cv::Mat(contour2)) );
        return ( i > j );
}


int main(int argc, char  *argv[]) {

        if (argc<4) {
                std::cerr << "Error wrong usage" << '\n';
                std::cout << "program image minThreshold maxThreshold" << '\n';
        }
        int thrLarge=atoi(argv[3]);
        int thrSmall = atoi(argv[2]);

        cv::Mat hls,binRange;
        cv::Mat hlsChannnels[3];
        cv::namedWindow("hue",cv::WINDOW_NORMAL);
        cv::namedWindow("light",cv::WINDOW_NORMAL);
        cv::namedWindow("sat",cv::WINDOW_NORMAL);
        cv::namedWindow("bin",cv::WINDOW_NORMAL);
        cv::namedWindow("seg",cv::WINDOW_NORMAL);

        cv::Mat img = cv::imread(argv[1]);
        cv::cvtColor( img, hls, CV_BGR2HLS );
        cv::GaussianBlur(hls,hls,cv::Size(0,0),11);
        cv::split(hls,hlsChannnels);
        cv::inRange(hls,cv::Scalar(thrSmall,0,0),cv::Scalar(thrLarge,255,255),binRange);
        cv::bitwise_not(binRange,binRange);

        cv::Mat element = cv::getStructuringElement( cv::MORPH_ELLIPSE, cv::Size( 20,20) );
        cv::morphologyEx(binRange,binRange,cv::MORPH_OPEN,element);
        cv::morphologyEx(binRange,binRange,cv::MORPH_CLOSE,element);

        std::vector<std::vector<cv::Point> > contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours( binRange, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
        cv::Rect limitRect;
        if (contours.size()>0) {

                sort(contours.begin(),contours.end(),compareContourAreas);
                limitRect = cv::boundingRect(contours[0]);
        }
        else
        {
                std::cout << "Could not find object" << '\n';
                return -1;
        }

        cv::Mat black(binRange.cols, binRange.rows, CV_8UC3,cv::Scalar(0,0,0));
        img.copyTo(black,binRange);


        cv::Mat segImg = black(limitRect);

        cv::imshow("seg",segImg);
        cv::imshow("hue",hlsChannnels[0]);
        cv::imshow("light",hlsChannnels[1]);
        cv::imshow("sat",hlsChannnels[2]);
        cv::imshow("bin", binRange );
        cv::waitKey(0);

        return 0;
}

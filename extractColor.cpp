#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/videoio.hpp>
#include <stdlib.h>
#include <iostream>
#include <string>

#include "boost/filesystem.hpp"

const std::string win_title="Feed";
const std::string seg_title="Segmentation";
const std::string msk = "Mask";

bool compareContourAreas ( std::vector<cv::Point> contour1, std::vector<cv::Point> contour2 ) {
        double i = fabs( contourArea(cv::Mat(contour1)) );
        double j = fabs( contourArea(cv::Mat(contour2)) );
        return ( i > j );
}

std::vector<std::string> getImages(std::string pth)
{
        boost::filesystem::path p (pth);
        std::vector<std::string> files;
        boost::filesystem::recursive_directory_iterator end_itr;
        boost::filesystem::recursive_directory_iterator itr(p);
        // cycle through the directory
        for (; itr != end_itr; ++itr)
        {
                // If it's not a directory, list it. If you want to list directories too, just remove this check.
                if (is_regular_file(itr->path())) {
                        // assign current file name to current_file and echo it out to the console.

                        boost::filesystem::path fullPth = boost::filesystem::canonical(itr->path());
                        std::string current_file = fullPth.string();

                        std::string extension = boost::filesystem::extension(current_file);
                        if (extension.compare("jpg")) {
                                files.push_back(current_file);
                                std::cout << current_file << std::endl;
                        }

                }
        }
        return files;
}


std::string getName(std::string pth)
{
        boost::filesystem::path p (pth);
        return p.stem().string();
}

int main(int argc, char  *argv[]) {
        if (argc<4) {
                std::cerr << "Error wrong usage" << '\n';
                std::cout << "program image/folder minThreshold maxThreshold" << '\n';
        }

        std::vector<std::string>  imageFiles=getImages(argv[1]);
        if(imageFiles.size()<1)
        {
                std::cerr << "Error no image files found" << '\n';
                return -1;
        }

        int thrLarge=atoi(argv[3]);
        int thrSmall = atoi(argv[2]);
        cv::Mat hls,binRange;
        cv::Mat hlsChannnels[3];
        // cv::namedWindow("hue",cv::WINDOW_NORMAL);
        // cv::namedWindow("bin",cv::WINDOW_NORMAL);
        // cv::namedWindow("seg",cv::WINDOW_NORMAL);

        for (unsigned int i = 0; i < imageFiles.size(); i++) {
                std::cout << "Processing image: " <<  imageFiles[i]<<'\n';
                cv::Mat img = cv::imread(imageFiles[i].c_str());
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

                sort(contours.begin(),contours.end(),compareContourAreas);

                cv::Rect limitRect;
                if (contours.size()>0) {

                        limitRect = cv::boundingRect(contours[0]);
                }
                else
                {
                        std::cout << "Could not find object" << '\n';
                }

                cv::Mat black(binRange.cols, binRange.rows, CV_8UC3,cv::Scalar(0,0,0));
                img.copyTo(black,binRange);

                std::string localName = "../segmented/" + getName(imageFiles[i])+ ".png";
                cv::Mat segImg = black(limitRect);
                cv::imwrite(localName,segImg);
                //cv::imshow("seg",segImg);
                // cv::imshow("hue",hsvChannnels[0]);
                // cv::imshow("bin", binRange );
                ///cv::waitKey(0);
        }
        return 0;
}

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/videoio.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <fstream>
#include <string>
#include <algorithm>
#include "boost/filesystem.hpp"
typedef  std::vector<std::string>  stringVector;

stringVector getNames(std::string path)
{
        std::ifstream nameFile;
        nameFile.open(path);
        stringVector objNames;
        std::string name;
        if (nameFile.is_open()) {
                while (nameFile>>name) {
                        objNames.push_back(name);
                        std::cout << "Obj: " << name << '\n';
                }
        }
        else
        {
                std::cout << "Could not open file " << path<<'\n';
        }

        return objNames;
}

void  printStringVector(stringVector sv) {
        for (size_t i = 0; i < sv.size(); i++) {
                std::cout << sv[i] << '\n';
        }
        return;
}

stringVector getImages(std::string pth)
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
                        if (extension.compare("png") || extension.compare("jpg")) {
                                files.push_back(current_file);
                                std::cout << current_file << std::endl;
                        }

                }
        }
        return files;
}

void randomRotate(cv::Mat& src,  cv::Mat& dst){
        float random = ((float) rand()) / (float) RAND_MAX;
        float diff = 180.0 - 0;
        float r = random * diff;
        float angle = 0 + r;
        // std::cout << "\t  Rotated by " << angle*180/3.1416<< '\n';
        cv::Point2f ptCp(src.cols*0.5, src.rows*0.5);
        cv::Mat M = cv::getRotationMatrix2D(ptCp, angle, 1.0);
        cv::warpAffine(src, dst, M, src.size(), cv::INTER_CUBIC); //Nearest is too rough,
        return;
}


void resizeForScene(cv::Mat & obj, cv::Mat &scene, cv::Mat &salida,
                       float maxFrac = 0.05, float minFrac = 0.01)
{

        //resizing
        // std::cout << "\tOriginal Size:" << obj.size()<< '\n';
        //int percentage = rand() % (maxPercent - minPercent) +minPercent;
        float random = ((float) rand()) / (float) RAND_MAX;
        float diff = maxFrac - minFrac;
        float r = random * diff;
        float frac = minFrac + r;
        //Shirnk by a random a percentage from 25 to 1 percent
        //This is based on image height
        // float aspectRatio = (float)obj.cols/(float)obj.rows;
        // int w = scene.rows*frac;
        // cv::Size sz(w*aspectRatio,w);
        //This is for image widht
        // float aspectRatio = (float)obj.rows/(float)obj.cols;
        // int w = scene.cols*frac;
        // cv::Size sz(w,w*aspectRatio);
        //This is for image area
        float aspectRatio = (float)obj.rows/(float)obj.cols;
        float fracArea = scene.cols*scene.rows * frac;
        int w = sqrtf(fracArea/aspectRatio);
        cv::Size sz(w,w*aspectRatio);
        //std::cout << "Output size" <<  <<'\n';
        //printf("w %d and aspect ratio %f\n", w,aspectRatio);
        cv::resize(obj,salida,sz);

        return;
}

cv::Point getSceneLocation(cv::Mat &obj, cv::Mat scene)
{
        int minPxX = 0;
        int minPxY = 0;
        int maxPxX = scene.cols - obj.cols;
        int maxPxY = scene.rows - obj.rows;
        int x = rand()%(maxPxX - minPxX ) + minPxX;
        int y = rand()%(maxPxY - minPxY) + minPxY;
        return cv::Point(x,y);

}


std::string yoloTag(stringVector &names, std::string img,cv::Rect roi, cv::Size sceneSz)
{
        float x_center = (roi.x+roi.width/2)/(float)sceneSz.width;
        float y_center = (roi.y+roi.height/2)/(float)sceneSz.height;
        float width = roi.width/(float)sceneSz.width;
        float height = roi.height/(float)sceneSz.height;
        int cat=0;
        for (size_t i = 0; i < names.size(); i++) {
                if (img.find(names[i])!=std::string::npos)
                {
                        //std::cout << "\tObject of class" << names[i]<< '\n';
                        cat=i; break;
                }
        }
        std::ostringstream oss;
        oss << cat<<" " << x_center << " " << y_center << " " <<
        width << " " << height <<"\n";
        std::string var = oss.str();
        return var;

}

void saveTag(std::string &tag, std::string fname){
        std::ofstream myfile;
        myfile.open (fname);
        myfile << tag;
        myfile.close();
}

void generateSyntethic(stringVector &segImages, stringVector &scenes,
                       stringVector &names,int k) {
        // k number of synthethic images to generate

        if (segImages.size() < 1) {
                std::cout << "No segmented images passed," << '\n';
                return;
        }
        if (scenes.size() < 1) {
                std::cout << "No scene images passed," << '\n';
                return;
        }
        if (names.size() < 1) {
                std::cout << "No object names passed," << '\n';
                return;
        }
        int segSize = segImages.size();
        int scenesSize = scenes.size();
        int namesSize = names.size();
        srand(time(NULL)); // Seed the time

        for (int i = 0; i < k; i++)
        {
                std::string synthethicBase = "../synth/syntethic" + std::to_string(i);
                std::string tag("");
                int maxObjPerImg = names.size()/2;
                std::random_shuffle ( segImages.begin(), segImages.end() );

                //loop over subsegment of objects
                int maxIdx = rand() % segSize;
                maxIdx = ((maxIdx>maxObjPerImg) ? maxObjPerImg : maxIdx);

                //std::cout << maxIdx << '\n';
                int sceneIdx = rand()%(scenesSize);
                cv::Mat scene = cv::imread(scenes[sceneIdx]);
                // printf("Scene [%d] size %d,%d\n",sceneIdx,scene.cols,scene.rows );
                //printf("Using objects [%d,%d]\n",0,maxIdx );
                for (int j = 0; j < maxIdx; j++)
                {
                        cv::Mat obj = cv::imread(segImages[j]);
                        // std::cout << "/-\\Object: " << segImages[j] << '\n';
                        //randomRotate(obj,obj);
                        resizeForScene(obj,scene,obj);
                        cv::Point position = getSceneLocation(obj,scene);
                        // std::cout << "\tPlaced in " << position <<'\n';
                        // std::cout << "\tWith size " << obj.size() << '\n';

                        cv::Rect roi(position,cv::Size(obj.cols,obj.rows));
                        try
                        {
                                cv::Mat dstROI = scene(roi);
                                obj.copyTo(dstROI,obj);
                                tag+=yoloTag(names, segImages[j], roi, scene.size());
                        }
                        catch (cv::Exception& e)
                        {
                                const char* err_msg = e.what();
                                std::cout << "\t<->exception caught: " << err_msg << std::endl;

                                continue;
                        }
                }

                std::cout << "Saving " << synthethicBase<<'\n';
                cv::imwrite(synthethicBase+".png",scene);
                printf("Using %d images \n",maxIdx);
                //std::cout << tag << '\n';
                saveTag(tag,synthethicBase+".txt");

        }

        return;
}



int main(int argc, char  *argv[]) {
        int imgsToCreate = 1;
        if (argc < 4) {
                std::cout << "Usage: program dataset.names segmentedDir sceneDir imgsToCreate" << '\n';
                return -1;
        }
        if (argc == 5) {
          imgsToCreate = atoi(argv[4]);
        }
        std::string namesFile(argv[1]);
        std::string segmentedDirectory(argv[2]);
        std::string sceneDirectory(argv[3]);

        stringVector objNames = getNames(namesFile);
        std::cout << "Loading segmented images" << '\n';
        stringVector objects = getImages(segmentedDirectory);
        std::cout << "Loading scenes images" << '\n';
        stringVector scenes = getImages(sceneDirectory);

        generateSyntethic(objects,scenes, objNames,imgsToCreate);


        return 0;
}

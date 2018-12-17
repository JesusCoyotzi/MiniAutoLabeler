# Mini-synthetic label generation

A dirty and simple program to automatically generate and prepare syntehtic datasets for YOLO!

## Overview

Deep Convolutional Neural Netwrok provide state of the art results for most task and problems. However they run using very large labeled datasets as fuel. And in ceratin cases access to these types of datasets might be very hard or they simply might not exits severly limiting the application of these type of algorithms.
However empirical test show that using synthehic images and transfer learning training an state of the art neural network is possible even with very few samples.
This repo proposes a very simple framework to generate and automatically label synthetic images used to train YOLO v3.
YOLO is a object detection algorithm based on deep convolutional neural networks. Based on an images the software produces the bounding box and the class probability of the objects on the training dataset appearing on the image.
A dataset prepared to be used in YOLO consists on several images labeled to indicate both the bounding box and the class of the objects of interest. Originally this neural network was trained using [COCO](http://cocodataset.org/) and [VOC](http://host.robots.ox.ac.uk/pascal/VOC/) datsets, these, despite having over 80 classes are limited to a certain knolwedsge domain. So using them on an specific task requieres retraining the network somwething slow and time consuming since it requires manually labelling the custom dataset.

## Theory of operation
Inspired by the special effects industries a dataset of the objects of interest is captured under a controlled background a green screen, from different viewpoints, the software then takes this images and segments the objects out of the background.
Once the objects are segmented they are placed on manually captured backgrounds on random positions and sizes since the object position is known before hand the labeling process is trivial.
At the end the user is left with a couple thousands images ready to be used to train YOLO

## Dependencies
This software requires only openCV and boost for its operation. Some utility scripts are written in python.

## Bulding
The packages comes with configured CMakeLists.txt file so to compile the code you only need to create a build folder and run cmake:
```
mkdir build && cd build
cmake ..
make
```

## Usage
Using you obj.names that you intend to use with YOLO run the rename.py this will creatre a folder hierarchy based on such file:
```
python3 makeDirTree.py path/to/obj.names /path/to/images
```
This create the following folder structure:
```
images
-obj_1
-obj_2
...
-obj_n
```
Get a couple (10-20) images of each object you need in a green screen or any sort of controlled background. Save each oine on its ciorresponding folder under images. Now run rename.py
```
python3 rename.py images/folder
```
This will recursively  rename all images to obj_class#_number.png this is  important as to check wich object is on each image the program uses the filename.
Now run the image segmeter. From build:
```
 extractColor /path/to/image/folders minimun_threshold max_threshold
```
The segmentation is based on the background color Hue. Segmented objects are stored on the segmented folder. There are several tools online to check the Hue of a color value but a "dry run" program is also supplied for convenience:
```
checkColor /path/to/img minimun_threshold max_threshold
```
This generates a couple windows one show the hue channel of the image, other the segmentation used the values passed as thresholds. This runs per image as opposed to the previous program wich works on every file on /images/ this also does not waves data on disk so it is usefull to test the segmentation before running the other program.
Finally you save all the backgrounds you desire, preferently places with minimal clutter and somewhere your objects make sense, and run the makeSynthetic program.
```
makeSynthetic dataset.names /path/to/segmentedimages /path/to/backgorunds Number_of_images_to_create
```
This will create N images along with their corresponding label.txt file formatted for YOLO. It is recommendes that you visually check the image results and teh segmentation as sometimes error can be introduced in the process due to irregular lighting and such.


#include "hyper_params.hpp"
#include "my_typedefs.hpp"
#include "platform_log.hpp"
#include "global_functions.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace cv;
using namespace std;

#ifndef NNPREP
#define NNPREP

struct NNprep
{
    vector<Mat> mnist (Mat src, int rows, int cols, string savePathDir = "../../digits/") {
        cout <<"PREPROCESSING IMAGES FOR THE NEURAL NETWORK"<<endl;
        platform_log("PREPROCESSING IMAGES FOR THE NERUAL NETWORK");
        vector<Mat> dst(rows * cols);
        vector<Mat> imgs;

        platform_log("Calculating image dimensions...");
        int cropWidth = src.size().width / cols;
        int cropHeight = src.size().height / rows;

        platform_log("Successfully calculated image dimensions");
        platform_log("Image width: %d, Image height: %d", src.size().width, src.size().height);
        platform_log("Crop width: %d, Crop height: %d" , cropWidth, cropHeight);

        cout<<"Image width is: "<<src.size().width<<endl;
        cout<<"Image height is: "<<src.size().height<<endl;
        cout<<"Crop width is: "<<cropWidth<<endl;
        cout<<"Crop height is: "<<cropHeight<<endl;


        platform_log("Cropping the images...");
        for(int i = 0; i < rows; i++) {
            for(int j = 0; j < cols; j++) {
            platform_log("Cropping image number: %d", i + 1);
                Rect roi( j * cropWidth,i * cropHeight,cropWidth ,cropHeight);
                imgs.push_back(src(roi));
            }
        }
        platform_log("Successfully cropped images");

        platform_log("Extracting character masks...");
        for(int i = 0; i < imgs.size(); i++) {
        platform_log("Extracting character mask for image: %d", i + 1);
            resize(imgs[i], imgs[i], Size(280, 280));
            Mat imgHSV, _dst ;

            // Extracting the digit's mask
            Scalar lower1(0,36,0), upper1(10, 255, 255), lower2(132, 20, 0), upper2(179, 255, 255);
            cvtColor(imgs[i], imgHSV, COLOR_BGR2HSV);
            Mat mask1 , mask2, mask;
            inRange(imgHSV, lower1, upper1, mask1);  
            inRange(imgHSV, lower2, upper2, mask2);    
            mask = mask1 | mask2;
            // bitwise_not(mask, mask);
            // imshow("mask" + to_string(i), mask);
            // imshow("hsv" + to_string(i), imgHSV);
            Mat kernelErode = getStructuringElement(MORPH_RECT, Size(3,3));
            dilate(mask, mask, kernelErode, Point(-1, -1), 2);
            erode(mask, mask, kernelErode, Point(-1, -1), 4);
            dilate(mask, mask, kernelErode, Point(-1, -1), 3);
            erode(mask, mask, kernelErode, Point(-1, -1), 3);
            dilate(mask, mask, kernelErode, Point(-1, -1), 4);
            erode(mask, mask, kernelErode, Point(-1, -1), 3);
            dilate(mask, mask, kernelErode, Point(-1, -1), 4);
            resize(mask, mask, Size(28, 28));
            // imshow("mask_" + to_string(i), mask);

             // Digit Centralization logic
             platform_log("Preparing image for centralization");
            copyMakeBorder(mask, mask, 28, 28, 28, 28, BORDER_CONSTANT, Scalar(0,0,0));
            // imshow("mask_after" + to_string(i), mask);
            Moments m = moments(mask, true);
            Point centroid(m.m10/m.m00, m.m01/m.m00);
            cout<<"The centroid id"<<Mat(centroid)<<endl;
            Rect roi;
            platform_log("Finding the centroid of the image");
            if(centroid.x > 0 && centroid.y > 0) {
                platform_log("--Valid centroid found at: (%d, %d)", centroid.x, centroid.y);
                roi = Rect(Point(centroid.x - 14, centroid.y - 14), Point(centroid.x + 14, centroid.y + 14));
            } else {
                platform_log("--Invalid centroid found at: (%d, %d)", centroid.x, centroid.y);
                roi = Rect(Point(0,0), Point(28, 28));
            }
            Mat finalMask = mask(roi);
            GaussianBlur(finalMask, finalMask, Size(3,3), 0);
            // imshow("Digit" + to_string(i), finalMask);
            dst[i] = finalMask;
            cout<<"Saving image to: "<<savePathDir + to_string(i)<<".jpg"<<endl;
            platform_log("Saving image");
            imwrite(savePathDir + to_string(checkIfValidChar(finalMask)) + "_"+ to_string(i)+".jpg", finalMask);
            // imshow("dst_"+to_string(i), dst[i]);
            // waitKey(1000);
            
        }
        
        return dst;
    }

    // checks if the  digit contained in the image is empty, cancelled or a valid digit
    // Returns 1 if the digit is valid and should be considered by the neural network
    // Return 0 if the digit is invalid and should not be considered by the neural network.

    
    int checkIfValidChar(Mat src) {
        int total = src.total();
        int whitePixelCount = countNonZero(src);
        
        //Return 0 if there is no digit or the digit has been cancelled.
        if(whitePixelCount < 0.05 * total) return 0;
        else if (whitePixelCount > 0.6 * total) return 2;
        //Return 1 if there is a valid digit
        else return 1;

    }

};

#endif 
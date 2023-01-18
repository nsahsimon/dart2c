#include "hyper_params.hpp"
#include "my_typedefs.hpp"
#include "mcqs.hpp"
#include "score_recorder.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "platform_log.hpp"

using namespace cv;
using namespace std;


unsigned char* yuv2rgb(
	uint8_t* plane0Bytes, uint8_t* plane1Bytes, uint8_t* plane2Bytes,
	int width, int height,
	int bytesPerRowPlane0 = 1, int bytesPerRowPlane1 = 2, int bytesPerRowPlane2 = 2,
	int bytesPerPixelPlane0 = 1, int bytesPerPixelPlane1 = 2, int bytesPerPixelPlane2 = 2
	) {

	Mat yplane , uplane, vplane , uvplane, image, rotImage, rotyplane, yplane3Chan;

	if(bytesPerPixelPlane0 == 1){
		yplane = Mat(height , width, CV_8U, plane0Bytes);
	} else if(bytesPerPixelPlane0 == 2) {
		yplane = Mat(height , width, CV_16U, plane0Bytes);
	} else {}

	if(bytesPerPixelPlane1 == 1){
		uplane  = Mat(height/2, width /2, CV_8U, plane1Bytes);
	} else if(bytesPerPixelPlane1 == 2) {
		uplane  = Mat(height/2, width /2, CV_16U, plane1Bytes);
	} else {}

	if(bytesPerPixelPlane2 == 1){
		vplane  = Mat(height/2, width /2, CV_8U, plane2Bytes);
	} else if(bytesPerPixelPlane2 == 2) {
		vplane  = Mat(height/2, width /2, CV_16U, plane2Bytes);
	} else {}

	cvtColorTwoPlane(yplane, vplane, image,90);

	// By default , all images are taken in portrait orientation
	// Check if the image is portrait or not then rotate it if necessary
	if(height / width < 1){
		rotate(image, rotImage, ROTATE_90_CLOCKWISE);

	}else {
		rotImage = image;
	}

	byte* imageBytes = matToBytes(rotImage);
	return imageBytes;

}



extern "C" __attribute__((visibility("default"))) __attribute__((used))
char* gradeMCQ(char* path, char* correctAnswers, char* responses, char* studentId, char* classId, char* subjectId, int subType = 1){
    platform_log("Welcome to C++ backend");
    platform_log("Running grade MCQ function");
	return _gradeMCQ(path, correctAnswers,  responses, studentId, classId, subjectId, subType);
}

extern "C" __attribute__((visibility("default"))) __attribute__((used))
char* record(char* path, char* savePathDir, char* studentId, char* classId, char* subjectId) {
    platform_log("Welcome to C++ backend");
    platform_log("Running score recording function");
	return _record(path, savePathDir, studentId, classId, subjectId);
}


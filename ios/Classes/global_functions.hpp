#include "my_structures.hpp"
#include "hyper_params.hpp"
#include "my_typedefs.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace cv;
using namespace std;

#ifndef GLOBFUNCTIONSDEF
#define GLOBFUNCTIONSDEF

string logStr = "------------C++ LOGS-------------\n";

string log(string text) {
    logStr = logStr + text + "\n";
    return logStr;
}

char* LOGS(){
    char* logs = (char*)malloc(logStr.length() + 1);
    strcpy(logs, logStr.c_str());
    return logs;
}

char* strToChar(string _string, int length) {
	char* array;
	strcpy(array, _string.c_str());
	return array;

}

//Converts an image into an array of bytes
	byte * matToBytes(Mat image)
	{
		int size = image.total() * image.elemSize();
		byte * bytes = new byte[size];  // you will have to delete[] that later
		std::memcpy(bytes,image.data,size * sizeof(byte));
		return bytes;
	}

	//converts a byte array into an image
	Mat bytesToMat(byte * bytes,int width,int height)
	{
		Mat image = Mat(height,width,CV_8UC3,bytes).clone(); // make a copy
		return image;
	}

//rotates an image by a defined angle
Mat rotate(Mat src, double angle)   //rotate function returning mat object with parametres imagefile and angle    
{
    Mat dst;      //Mat object for output image file
    Point2f pt(src.cols/2., src.rows/2.);          //point from where to rotate    
    Mat r = getRotationMatrix2D(pt, angle, 1.0);      //Mat object for storing after rotation
    warpAffine(src, dst, r, Size(src.cols, src.rows));  ///applie an affine transforation to image.
    return dst;         //returning Mat object for output image file
}

//compares the aspect ratio of two sections
bool aspectRatioCmp(Section section1, Section section2) {
	//sections with lower aspect ratios(response sections) always come first
	return section1.aspectRatio() < section2.aspectRatio();
}

bool Ycmp(Section section1, Section section2) {
	return section1.centroid.y < section2.centroid.y;
}

bool YcmpInv(Section section1, Section section2) {
	return section1.centroid.y > section2.centroid.y;
}

bool Xcmp(Section section1, Section section2) {
	return section1.centroid.x < section2.centroid.x;
}

bool YcmpBubble(Bubble bubble1, Bubble bubble2) {
	Mat transposeMat = bubble1.transMat; //also equal to bubble2.transMat
	vector<Point2f> bubbleCentroidsSrc= {bubble1.centroid, bubble2.centroid};
	vector<Point2f> bubbleCentroidsDst;
	transform(bubbleCentroidsSrc, bubbleCentroidsDst, transposeMat);
	return bubbleCentroidsDst[0].y < bubbleCentroidsDst[1].y;
}

bool XcmpBubble(Bubble bubble1, Bubble bubble2) {
	Mat transposeMat = bubble1.transMat; //also equal to bubble2.transMat
	vector<Point2f> bubbleCentroidsSrc= {bubble1.centroid, bubble2.centroid};
	vector<Point2f> bubbleCentroidsDst;
	transform(bubbleCentroidsSrc, bubbleCentroidsDst, transposeMat);
	return bubbleCentroidsDst[0].x < bubbleCentroidsDst[1].x;
}

bool cntAreaCmp(vector<Point> cnt1, vector<Point> cnt2) {
	return contourArea(cnt1) > contourArea(cnt2);
}

Point getCentroid(vector<Point> contour, int xResizeFactor = 1, int yResizeFactor = 1) {
	Point centroid(-1, -1);
	Moments m;
	int x,y; //coordinates of centroid;
	m = moments(contour);
	if(m.m00 != 0.0) {
		centroid.x = (m.m10 / m.m00) / xResizeFactor;
		centroid.y = (m.m01 / m.m00) / yResizeFactor;
	}
	return centroid;
}

//transposes the image and returns the transposition matrix
Mat getTranspose(Mat image, Mat& dstImage) {
	Point2f src[4] = { {0.0f,0.0f}, {(float)image.size().width,0.0f} ,{0.0f,(float)image.size().height} };
	Point2f dst[4] = { {0.0f,0.0f}, {0.0f,(float)image.size().width} ,{(float)image.size().height, 0.0f} };

	Mat matrix = getAffineTransform(src, dst);
	//warpPerspective(image, dstImage, matrix, Size(image.size().height, image.size().width));
	return matrix;
}

//sorts contours in decreasing order of area
void sortContoursByArea(vector<vector<Point>>* contour_ptrs) {
	vector<vector<Point>> contours = *contour_ptrs;
	sort(contours.begin(), contours.end(), cntAreaCmp);
	*contour_ptrs = contours;
}

bool cmpWhitePix(Bubble bubble1, Bubble bubble2) {
	return bubble1.whitePixelCount > bubble2.whitePixelCount;
}

bool cmpZscores(double zscore1, double zscore2){
	return zscore1 > zscore2;
}

vector<double> getZScores(vector<Bubble> bubbles) {
	double mean;
	double sdv;
	double sum = 0;
	vector<double> zScores(bubbles.size());
	for(int i = 0; i < bubbles.size(); i++){
		sum = sum + bubbles[i].whitePixelCount;
	}
	mean = sum / (double)bubbles.size();

	double sumOfSquareDevs = 0;
	for(int i = 0; i < bubbles.size(); i++) {
		sumOfSquareDevs = sumOfSquareDevs + pow((bubbles[i].whitePixelCount - mean),2);
	}

	sdv = sqrt(sumOfSquareDevs / bubbles.size());

	for(int i =0; i < bubbles.size(); i++) {
		zScores[i] = abs(bubbles[i].whitePixelCount - mean) / sdv;
	}

	//Sort zscores
	sort(zScores.begin(), zScores.end(), cmpZscores);
	cout <<"displaying zScores"<<endl;
	for(int i = 0; i < zScores.size(); i++) {
		cout <<zScores[i]<<",";
	}
	return zScores;

}

double getMean(vector<Bubble> bubbles) {
	double mean;
	double sum = 0;
	vector<double> zScores(bubbles.size());
	for(int i = 0; i < bubbles.size(); i++){
		sum = sum + bubbles[i].whitePixelCount;
	}
	mean = sum / (double)bubbles.size();
	return mean;
}

//Retrieves the two most intense bubbles in a row
vector<Bubble> twoMostIntenseBubbles(vector<Bubble> bubbles) {
	Bubble selectedBubble = bubbles[0];
	sort(bubbles.begin(), bubbles.end(), cmpWhitePix);
	double pixRatio = (double) bubbles[0].whitePixelCount / (double) bubbles[1].whitePixelCount;
	selectedBubble = bubbles[0];
	vector<Bubble> mostIntenseBubbles = {bubbles[0], bubbles[1]};
	return mostIntenseBubbles;
}

//Retrieves the checked bubble in a row
Bubble getSelectedBubble(vector<Bubble> bubbles) {
	Bubble selectedBubble = bubbles[0];
	int unCheckedBubblesCount = bubbles.size() - 1;
	for(int i = 0; i < bubbles.size(); i++) {
		if(bubbles[i].isChecked == false) unCheckedBubblesCount -= 1;
		else selectedBubble = bubbles[i];
	}

	if(unCheckedBubblesCount != 0) {
		selectedBubble.cheatingDetected = true;
	}

	// Previous logic
	// sort(bubbles.begin(), bubbles.end(), cmpWhitePix);
	// double pixRatio = (double) bubbles[0].whitePixelCount / (double) bubbles[1].whitePixelCount;
	// selectedBubble = bubbles[0];
	// double mean = getMean(bubbles);
	// cout << "mean:  = "<<mean<<endl;
	// if((double)selectedBubble.whitePixelCount / mean < 1.15) {
	// 	//selectedBubble.cheatingDetected = true;
	// }
	return selectedBubble;
}

bool hasCentralCavity(Mat _bubbleImg){
	int height = _bubbleImg.size().height;
	int width = _bubbleImg.size().width;
	Point  center(_bubbleImg.size().width / 2, _bubbleImg.size().height / 2 );
	int xoffset = 2 , yoffset = 3;
	Rect _roi(Point(center.x - xoffset, center.y - yoffset), Point(center.x + xoffset, center.y + yoffset));
	Mat centerCropped = _bubbleImg(_roi);
	int whitePixCount = countNonZero(centerCropped);
	int total = centerCropped.total();
	cout<<"WhitePixCount: "<<whitePixCount<<" Total: "<<total<<endl;
	if(whitePixCount >=  0.7 * total) return false;
	return true;
 }

// Determine if a particular contour has an inner contour
bool isChecked(vector<Point> cnt, vector<vector<Point>> allCnts, Mat bubbleImg) {
	// int cntCount= allCnts.size();
	// Point refCentroid = getCentroid(cnt);
	// for(int i = 0; i < cntCount; i++) {
	// 	Point testCentroid = getCentroid(allCnts[i]);
	// 	double refArea = contourArea(cnt);
	// 	double testArea = contourArea(allCnts[i]);
	// 	if(refArea != testArea  && pointPolygonTest(cnt, testCentroid, false) == 1) {
	// 		vector<Point> approxCnt;
	// 		double _perimeter = arcLength(allCnts[i], true);
	// 		approxPolyDP(allCnts[i], approxCnt, 0.02 * _perimeter,true);
	// 		cout<<"contour number of sides: "<<approxCnt.size()<<endl;
	// 		// if(approxCnt.size() > 7 ) return true;
	// 		return false;
	// 	}
	// }

///check if it contains a central cavity
	return hasCentralCavity(bubbleImg) ? false : true;
	return true;
}

//extracts the value
Mat preProcessing(Mat img)
{
	cout <<"starting preprocessing" <<endl;
	Mat _imgBin, _imgGray, _imgBlur, _imgCanny,_imgCannyTemp, _imgDil, _newImgDil, _newImgDilTemp,_imgThreTemp, _imgDilTemp, _imgBinTemp, _imgThre, _imgErode, _imgErodeTemp;
	//convert the image to gray scale
	cvtColor(img, _imgGray, COLOR_BGR2GRAY,1);

	Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
	Mat kernelErode = getStructuringElement(MORPH_RECT, Size(3,3));

	adaptiveThreshold(_imgGray, _imgThre, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 75, 2);
	threshold(_imgThre, _imgDil, 0, 255, THRESH_BINARY_INV);
	erode(_imgDil, _imgDil, kernelErode, Point(-1, -1), 1);

	dilate(_imgDil, _imgDil, kernel, Point(-1, -1), 1);
	erode(_imgDil, _imgDil, kernelErode, Point(-1, -1), 1);
	dilate(_imgDil, _imgDil, kernel, Point(-1, -1), 1);
	resize(_imgDil, _newImgDil, Size(), 0.2, 0.2);
	return _imgDil;
}

//
vector<Point> getContours(Mat image, vector<vector<Point>> &approxFormCnts) {
	// vector<vector<Point>> contours;
	vector<Point> biggestFormCnt;
	vector<Vec4i> hierarchy;
	vector<vector<Point>> contoursGlob;
	if (contoursGlob.size() == 0 && approxFormCnts.size() == 0){
		findContours(image, contoursGlob, hierarchy,RETR_LIST, CHAIN_APPROX_SIMPLE);
		vector<vector<Point>> conPoly(contoursGlob.size());
		for (int i = 0; i < contoursGlob.size(); i++)
		{
			int area = contourArea(contoursGlob[i]);
			if (area > 100)
			{
				float peri = arcLength(contoursGlob[i], true);
				approxPolyDP(contoursGlob[i], conPoly[i], 0.02 * peri, true);

				if (conPoly[i].size() == 4 ) {
					cout <<"trying to add approxform"<<endl;
					approxFormCnts.push_back(conPoly[i]);
				}
			}
		}
		sortContoursByArea(&approxFormCnts);
		cout<< "Successfully found the "<<approxFormCnts.size() <<" contours"<<endl;
	}
	//drawContours(imgOriginal, contours, -1, Scalar(255, 0, 255), 2);
	cout<< "Successfully drew the contours"<<endl;
	biggestFormCnt = approxFormCnts[0];
	return biggestFormCnt;
}

//
void drawPoints(vector<Point> points, Scalar color, Mat &imgOriginal, unsigned char* imgPtr)
{
	for (int i = 0; i < points.size(); i++)
	{
		circle(imgOriginal, points[i], 10, color, FILLED);
		putText(imgOriginal, to_string(i), points[i], FONT_HERSHEY_PLAIN, 4, color, 4);
	}
}

//
vector<Point> reorder(vector<Point> points)
{
	vector<Point> newPoints;
	vector<int>  sumPoints, subPoints;

	for (int i = 0; i < 4; i++)
	{
		sumPoints.push_back(points[i].x + points[i].y);
		subPoints.push_back(points[i].x - points[i].y);
	}

	newPoints.push_back(points[min_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]); //0
	newPoints.push_back(points[max_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]); //1
	newPoints.push_back(points[min_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]); //2
	newPoints.push_back(points[max_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]); //3

	return newPoints;
}

//
Mat getWarp(Mat img, vector<Point> points, float w, float h ) {

Mat _imgWarp;
	Point2f src[4] = { points[0],points[1],points[2],points[3] };
	Point2f dst[4] = { {0.0f,0.0f},{w,0.0f},{0.0f,h},{w,h} };

	Mat matrix = getPerspectiveTransform(src, dst);
	warpPerspective(img, _imgWarp, matrix, Point(w, h));

	return _imgWarp;
}

Mat imgFromBytes(uchar* bytes, int32_t width, int32_t height, int32_t type) {
    Mat img = Mat(height,width, type, bytes);
    return img;
}

//this function expects a vector of 40 bubbles as input
//returns true is successful and false if unsuccessful
bool sortBubbles(vector<Bubble> src, vector<Bubble>& dst,  int columns, string sectionType) {
	cout<<src.size()<<" bubbles in this section"<<endl;
	cout<<columns<<" columns"<<endl;
	
	if(src.size() % columns != 0) return false;
	int rows = (int) src.size() / columns;
	cout<<rows<<" rows"<<endl;

	// if(sectionType == ID_SECTION) {
	// 	//Swap rows and column number
	// 	int cols = columns;
	// 	columns = rows;
	// 	rows = cols;
	// 	cout<<"successfully swapped rows and columns"<<endl;
	// 	cout<<"new rows "<<rows<<endl;
	// 	cout<<"new columns "<<columns<<endl;
	// }

	//vertical sorting
	sort(src.begin(), src.end(), YcmpBubble);
	cout<< "sorted bubbles vertically"<<endl;
	cout <<"successfully sorted bubbles vertically"<<endl;
	//horizontal sorting
	//involves breaking the 40 bubbles into chunks of 4
	for(int j = 0; j < rows; j++) {
		sort(src.begin() + columns * j, src.end() - (columns * (rows - 1 - j)), XcmpBubble);
		//else sort(bubbles.begin() + 4 * j, bubbles.end(), XcmpBubble);
	}
	cout<< "sorted bubbles horizontally" <<endl;

	//set the appropropriate section id for each section
	for(int i = 0; i < src.size(); i++) {
		src[i].id = i;
	}

	dst = src;
	//return the sorted list of bubbles
	return true;
}

vector<int> charToVec(char* charArray) {
    string str(charArray);
    vector<int> vec(str.length());
    for(int i = 0; i< vec.size(); i++) {
        vec[i] = (int)charArray[i];
    }
    return vec;
}

char* vecToNullTermChar(vector<int> vector){
    char* charArray = new char[vector.size()];
    for(int i = 0; i < vector.size(); i++) {
        charArray[i] = vector[i] + 48;
    }
	charArray[vector.size()] = '\0';
    return charArray;
}

int getStdDev(Mat bubbleImg_gray) {
	Scalar means;
	Scalar stdDevs;
	meanStdDev(bubbleImg_gray, means, stdDevs);
	float mean = means[0];
	float stdDev = stdDevs[0];
	cout<<"The standard deviation is: "<<stdDev<<endl;
	return (int) stdDev;
}

#endif
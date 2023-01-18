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

#ifndef STRUCTSDEFINED
#define STRUCTSDEFINED 


struct Bubble {
	int area;
	int whitePixelCount;
	bool isChecked = false;
	vector<Point> contour;
	vector<Point> approxContour;
	Point centroid;
	Mat transMat;
	int id;
	Mat img;
	bool cheatingDetected = false;
	//The coordinate of the origin of the section to which the bubble belongs
	Point parentSectionOrigin;
	//The coordinates of the bubble on the cropped original image image.


	Bubble(Point sectionOrigin) {
		parentSectionOrigin = sectionOrigin;
	}

	Bubble() {

	}

	//this method returns the original centroid of the bubble on the cropped original image
	Point OriginalCentroid() {
		Point bubbleCentroid;
		bubbleCentroid.x = parentSectionOrigin.x + secCropWidth + centroid.x - (sectionPadding / 2);
		bubbleCentroid.y = parentSectionOrigin.y + secCropHeight + centroid.y - (sectionPadding / 2);
		return bubbleCentroid;
	}
};

//returns an identity matrix
Mat identityMat() {
	Point2f src[4] = {{0.0f,0.0f}, {1.0f,0.0f} ,{0.0f,1.0f}};
	Point2f dst[4] = {{0.0f,0.0f}, {1.0f,0.0f} ,{0.0f,1.0f}};
	return getAffineTransform(src, dst);
}		

struct Section {
	Mat img; //holds a cropped image of the section.
	Mat transMat = identityMat(); //Used incase the section requires a transposition.
	Point centroid; //holds the centroid of the section.
	vector<Point> contour; // holds the contour of the section
	Rect boundingRect; // hold the contours fo the bounding box of this section
	int id; // describes the specific information held by this section.
	string type = QUESTION_SECTION; //ID, QUESTION, NAME and SCORE SECTIONS.
	double area;
	double perimeter;
	vector<int> responses; //zero means no answer selected.
	vector<Bubble> bubbles; //holds the total number of bubbles detected in the section.
	vector<Bubble> selectedBubbles;
	int rows = qSectionRows;
	int columns = qSectionColumns;

	Section() {

	}


	double aspectRatio() {
		return boundingRect.width /boundingRect.height;
	}
};

struct Form {
	bool isValid;
	string type = MCQ_FORM; //can be MCQ_FORM, STRUCT_FORM OR SCORE_FORM
	vector<Section> sections;
	Mat img;
	Mat segmentedImg;


	Form(bool _isValid, Mat _img, string type = MCQ_FORM) {
		isValid = _isValid;
		img = _img;
		segmentedImg = _img.clone();
		this->type = type;
	}

	Form(bool _isValid, string type = MCQ_FORM) {
		isValid = _isValid;
        this->type = type;
	}

	Form() {

	}

	void strikeBubbles(Mat img, int sectionIndex, int bubbleIndex){
		vector<Bubble> bubbles = sections[sectionIndex].bubbles;
		int columns = sections[sectionIndex].columns;
		Point pt1 = Point(bubbles[bubbleIndex * columns].OriginalCentroid().x - 5, bubbles[bubbleIndex * columns].OriginalCentroid().y - 5 );
		Point pt2 = bubbles[bubbleIndex * columns + columns - 1].OriginalCentroid();
		circle(img, pt1, 3, Scalar(0, 255, 255), FILLED);
		//line(img, pt1, pt2, Scalar(0, 0, 255));
	}

	//Draws and labels the sections on the segmented form
	void drawSections() {
		if(isValid ==  true) {
			segmentedImg = img.clone();
			for (int i = 0; i < sections.size(); i++) {
				putText(segmentedImg, to_string(sections[i].id), sections[i].centroid, FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0,0));
				rectangle(segmentedImg, sections[i].boundingRect, Scalar(255, 0, 0));
			}
		}
	}

	vector<int> getResponses() {
		vector<int> responsesVec;
		vector<int> subjectIdVec;
		vector<int> classIdVec;
		vector<int> studentIdVec;

		for(int i = 0; i < totSectionCount; i++) {
			Section sec = sections[i];
			int columns;
			int rows;
			if(sec.type == "TEXT") continue;
			if(sec.type == ID_SECTION) {
				//swap rows and columns
				columns = sec.rows;
				rows = sec.columns;
			}else {
				columns = sec.columns;
			}
			
			vector<Bubble> bubbles = sec.selectedBubbles;
			for(int j = 0; j < bubbles.size(); j++) {
					if (sec.type == ID_SECTION) {
						if(sec.id == 1) { //i.e student Id
							bubbles[j].cheatingDetected == false ? studentIdVec.push_back(bubbles[j].id % columns) : studentIdVec.push_back(-1);
						}else if (sec.id == 2) { //i.e class Id
							bubbles[j].cheatingDetected == false ? classIdVec.push_back(bubbles[j].id % columns) : classIdVec.push_back(-1);
						}else if (sec.id == 3) { //i.e subject Id
							bubbles[j].cheatingDetected == false ? subjectIdVec.push_back(bubbles[j].id % columns) : subjectIdVec.push_back(-1);
						}else {

						}
						//responsesVec.push_back(bubbles[j].id % 10)
					} 

					else if (sec.type == QUESTION_SECTION) {
						if(responsesVec.size() == expNumOfResp) continue;
						bubbles[j].cheatingDetected == false ? responsesVec.push_back(bubbles[j].id % columns  + 1) : responsesVec.push_back(-1);
					}

					else {

					}
			}
		}

		studentId[studentIdVec.size()] = '\0';
            for(int i = 0; i < studentIdVec.size(); i++) {
            if(studentIdVec[i] != -1) {
            	strcpy(&studentId[i], to_string(studentIdVec[i]).c_str());
            } else {
            	string x = "x";
            	strcpy(&studentId[i], x.c_str());
                }
            }

		classId[classIdVec.size()] = '\0';
            for(int i = 0; i < classIdVec.size(); i++) {
            if(classIdVec[i] != -1) {
            	strcpy(&classId[i], to_string(classIdVec[i]).c_str());
            } else {
            	string x = "x";
            	strcpy(&classId[i], x.c_str());
                }
            }

		subjectId[subjectIdVec.size()] = '\0';
            for(int i = 0; i < subjectIdVec.size(); i++) {
            if(subjectIdVec[i] != -1) {
            	strcpy(&subjectId[i], to_string(subjectIdVec[i]).c_str());
            } else {
            	string x = "x";
            	strcpy(&subjectId[i], x.c_str());
                }
            }	

		return responsesVec;
	}

	bool markResponses(vector<int> correctAnswers = {}) {
		segmentedImg = img.clone();
		if(sections.size() != totSectionCount) {
			cout <<"Invalid section Size"<<endl;
			return false;
		}

		for(int i = 0; i < totSectionCount; i++) {
			if(sections[i].type == NAME_SECTION || sections[i].type == SCORE_SECTION ) continue;

			//Run code below only for QUESTIONS_SECTIONS, ID_SECTIONS OR ...
			int rows = sections[i].rows;
			int columns = sections[i].columns;
			if(sections[i].selectedBubbles.size() == 0) {
				cout <<"Invalid bubble size"<<endl;
				continue;}
			for(int j = 0; j < sections[i].selectedBubbles.size(); j ++) {
				//printing the white pixel count of the selected bubble
				cout<<"white pixel count for bubble "<<j<<" is "<<sections[i].selectedBubbles[j].whitePixelCount<<endl;
				//if cheating was not detected
				
					if(sections[i].type ==  ID_SECTION){
						if(sections[i].selectedBubbles[j].cheatingDetected == false) {
                            circle(segmentedImg, sections[i].selectedBubbles[j].OriginalCentroid(), 6, Scalar(255, 0, 0), 2);						}else {
							// Do nothing
						}
					} else {
						int index = (i - numOfIdSections) * qSectionRows + j;
						if(index + 1  > expNumOfResp) continue;
						if(sections[i].selectedBubbles[j].cheatingDetected == false) {
							if(sections[i].type ==  QUESTION_SECTION) {
								int index = (i - numOfIdSections) * qSectionRows + j;
								if(index + 1  > expNumOfResp) continue;
								if(correctAnswers[index] == (sections[i].selectedBubbles[j].id % columns + 1)) {
									circle(segmentedImg, sections[i].selectedBubbles[j].OriginalCentroid(), 6, Scalar(255, 0, 0), 2);
								}else{
									circle(segmentedImg, sections[i].selectedBubbles[j].OriginalCentroid(), 6, Scalar(0, 0, 255), 2);
								}
							}else if (false) {
							  //Add logic for another type of section
							} else {

							}
						}else {
							strikeBubbles(segmentedImg, i, j);
						}
					
					}
					
					
				
			}
		}

		out_of_loop:
	cout <<"Responses successfully marked" <<endl;
		return true;
	}
};


#endif
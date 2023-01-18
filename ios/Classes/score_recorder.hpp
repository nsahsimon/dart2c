#include "my_structures.hpp"
#include "hyper_params.hpp"
#include "my_typedefs.hpp"
#include "neural_net_prep.hpp"
#include "global_functions.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include "platform_log.hpp"

using namespace cv;
using namespace std;

#ifndef SRFUNCTIONSDEFINED
#define SRFUNCTIONSDEFINED

//Mat imgOriginal;

//vector<vector<Point>> contoursGlob;
//vector<vector<Point>> approxFormCnts;
//vector<Point> biggestFormCnt;

vector<Section> sortSRSections(vector<Section> sections) {
	//first sort sections based on their aspect ratios i.e width/height
		//sort(sections.begin(), sections.end(), aspectRatioCmp);
		sort(sections.begin(), sections.end() , Ycmp);
		sort(sections.begin() + 1, sections.end() - 1, Xcmp);// sorting out the id sections
		
		//set the appropropriate section id for each section.
		//set the appropriate section type for each section.
        sections[0].id = 4;
        sections[0].type = NAME_SECTION;

		for(int i = 1; i < 4; i++) {
			int id = i;
			sections[i].id = id;
			sections[i].rows = idSectionRows;
			sections[i].columns = idSectionColumns;
			sections[i].type = ID_SECTION;
			sections[i].transMat = getTranspose(sections[i].img, sections[i].img);
		}

        sections[4].id = 5;
        sections[4].type = SCORE_SECTION;

		return sections;
}

//
Form validateSRForm(Mat img) {
	// imshow("form", img);
	// waitKey(1000);
	// Declare and initialize an output form to hold the result of the validation
	Form _outputForm(false, img);

	Mat _imgForm, _imgThre, _imgGray, _imgBlur, _imgCanny, _imgDil, _imgErode, _imgThreRes, _img = img.clone();

	vector<Section> _sections;
	vector<vector<Point>> _sectionContours;

	vector<vector<Point>> _contours;
	//convert the image to gray scale
	cvtColor(img, _imgGray, COLOR_BGR2GRAY,1);

	adaptiveThreshold(_imgGray, _imgThre, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 75, 2);
	threshold(_imgThre, _imgThre, 0, 255, THRESH_BINARY_INV);

	Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
	Mat kernelErode = getStructuringElement(MORPH_RECT, Size(3,3));

	vector<Vec4i> hierarchy;
	findContours(_imgThre, _contours,hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);
	vector<vector<Point>> _conPoly(_contours.size());

	cout <<"Found "<<_contours.size()<<"contours in the image form"<<endl;
	//approximate the contours
	for (int i = 0; i < _contours.size(); i++) {
		int area = contourArea(_contours[i]);
		float peri = arcLength(_contours[i], true);
		approxPolyDP(_contours[i], _conPoly[i], 0.02 * peri, true);

		if(area > 15000  && area < 200000 && _conPoly[i].size() == 4 ) {
			bool _sectionAlreadyExists = false;
			Section _section;
			_section.contour = _conPoly[i];
			_section.area = area;
			_section.centroid = getCentroid(_conPoly[i]);
			_section.boundingRect = boundingRect(_section.contour);

			for(int j =0; j< _sections.size(); j++) {
				cout<<"Checking for repeated sections" <<endl;
				Point2f a = _sections[j].centroid;
				Point2f b = _section.centroid;
				float dx = a.x - b.x;
				float dy = a.y - b.y;
				if(sqrt(dx * dx + dy * dy) < 20) {
					_sectionAlreadyExists = true;
					//cout <<"found a repeated section" <<endl;
					//cout <<"breaking out of the loop" <<endl;
					break;
					}
			}

			cout<<"check complete"<<endl;

			// add the section if only it doesn't exist yet
			if(_sectionAlreadyExists == false) {
				cout <<"adding a new section to the list"<<endl;
				auto _boundingRect = boundingRect(_section.contour);
				Rect roi(Point(_boundingRect.tl().x + secCropWidth, _boundingRect.tl().y + secCropHeight), Point(_boundingRect.br().x - secCropWidth, _boundingRect.br().y - secCropHeight));
				_section.img = img(roi);
				//imshow("section " + to_string(i),_section.img);
				//waitKey(10);
				//_section.img = img(roi);
				_sections.push_back(_section);
				}
			//drawContours(img, _conPoly, i, Scalar(255, 0, 255), 2);
		}
	}


	if(_sections.size() != totSectionCount) {
		cout << "Could not find all the section contours something went wrong"<<endl;
		cout << " Rather found "<<_sections.size()<<" sections"<<endl;
		_outputForm.isValid = false;

	} else {
		cout << "Found All the "<<totSectionCount<<" sections"<<endl;
		_sections = sortSRSections(_sections);

		//pick a sample section
		Section _sampleSection = _sections[2];

		//Get area of the section.
		//Calculate the the bounding box area of the section
		//compare these two areas
		//Ideally, these two areas should be the same.
		float areaRatio = _sampleSection.area / _sampleSection.boundingRect.area();
		if (areaRatio < 1.05 && areaRatio > 0.95) {
			_outputForm.sections = _sections;
			_outputForm.isValid = true;
		} else {
			_outputForm.isValid = false;
		}
	}

	_outputForm.isValid ? cout <<"Found a valid form"<< endl : cout <<"Invalid form"<< endl;
	return _outputForm;
}

// Extract the form from the image
Form extractSRForm(Mat imgOriginal, string type = 	STRUCT_FORM) {
	vector<Point> initialPoints,docPoints;
	vector<vector<Point>> approxFormCnts;
	Mat _imgWarp, _imgThre, _imgCrop;
	//cout<<"Image type: "<<imgOriginal.<<endl;
	resize(imgOriginal, imgOriginal, Size(1920, 2560));

	// Preprpcessing - Step 1
	_imgThre = preProcessing(imgOriginal);

	cout<< "successfully preprocessed the image"<<endl;

		getContours:
		if(approxFormCnts.size() == 0  && notFirstTime == true) {
			return Form(false, _imgCrop, type);
			log("(extractForm) premature function termination in extracted form");
		}

		// Getting the initial document contours
		initialPoints = getContours(_imgThre, approxFormCnts);
		notFirstTime = true;
		cout << "successfully got the contours in the image"<<endl;

		docPoints = reorder(initialPoints);
		cout << "successfully reordered the points the image"<<endl;

		// Warp
		_imgWarp = getWarp(imgOriginal, docPoints, w, h);
		cout << "successfully warped the image"<<endl;

		//Crop - Step 4
		int cropVal= 5;

		Rect roi(cropVal, cropVal, w - (2 * cropVal), h - (2 * cropVal));
		_imgCrop = _imgWarp(roi);

		if(approxFormCnts.size() == 0 ) {
			//imwrite(path, _imgCrop);
			return Form(false, _imgCrop, type);
			}

		Form checkedForm = validateSRForm(_imgCrop);

		if(checkedForm.isValid == true) {
			checkedForm.drawSections();
		} else if(approxFormCnts.size() != 0) {
			approxFormCnts.erase(approxFormCnts.begin());
			goto getContours;
		} else  {
			{
			//imwrite(outputPath, _imgCrop);
			return Form(false, _imgCrop, type);
			}
		}

	//imwrite(outputPath, _imgCrop);
	return checkedForm;
}

// Extracts responses from the sections
bool extractSRBubbles(Form& validForm, char* savePathDir) {
		vector<Section> _sections = validForm.sections;
	bool result = false;

	Mat _img, _imgGray, _imgGrayCrop, _imgThre, _imgBlur, _imgErode, _imgDil, _imgMorph;

		//extracting the responses from the question sections
		for(int i = 0 ; i < totSectionCount ; i++) {
			cout <<"EXTRACTING RESPONSES FROM SECTION "<<i<<endl;
			if(_sections[i].type == NAME_SECTION) continue;
			if(_sections[i].type == SCORE_SECTION) continue;

			Mat _img = _sections[i].img.clone();

			//resize section for better analysis
			resize(_img, _img, Size(), sectionResizeFactor, sectionResizeFactor);

			//Get the section's specifications
			int rows = _sections[i].rows;
			int columns = _sections[i].columns;

			int maxBubbleHeight = _img.size().height / rows;
			int maxBubbleWidth  = _img.size().width / columns;
			int minBubbleHeight = _img.size().height / (2 * (rows + 1));
			int minBubbleWidth  = _img.size().width / (2 * (columns + 1));

			if(_sections[i].type == ID_SECTION) {
				//Swap rows and columns
				int cols = columns;
				columns = rows;
				rows = cols;
					cout<<"successfully swapped rows and columns"<<endl;
					cout<<"new rows "<<rows<<endl;
					cout<<"new columns "<<columns<<endl;
			}

			vector<vector<Point>> _contours, _allContours;

			//Image to display the inscribed contours
			Mat _imgExp = _img.clone();

			cvtColor(_img, _imgGray, COLOR_BGRA2GRAY,1);

			_imgGrayCrop = _imgGray.clone();
			Mat _kernel = getStructuringElement(MORPH_RECT, Size(3,3));
			Mat _morphKernel = getStructuringElement(MORPH_RECT, Size(5,5));

			//Blur the image to get the bubble contours
			GaussianBlur(_imgGray, _imgGray, Size(7,7),1);
			adaptiveThreshold(_imgGray, _imgThre, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 95, 2);
			threshold(_imgThre, _imgThre, 0, 255, THRESH_BINARY_INV);

			erode(_imgThre, _imgErode, _kernel, Point(-1,-1), 1);
			dilate(_imgErode, _imgDil, _morphKernel, Point(-1,-1), 1);

			morphologyEx(_imgDil, _imgMorph, MORPH_CLOSE, _kernel, Point(-1, -1), 1);			

			//add some padding to the section inorder to detect bubbles at edges of the section
			copyMakeBorder(_imgThre, _imgThre, sectionPadding, sectionPadding, sectionPadding, sectionPadding, BORDER_CONSTANT, Scalar(0,0,0));
			copyMakeBorder(_imgGrayCrop, _imgGrayCrop, sectionPadding, sectionPadding, sectionPadding, sectionPadding, BORDER_REPLICATE, Scalar(0,0,0));
			copyMakeBorder(_imgGray, _imgGray, sectionPadding, sectionPadding, sectionPadding, sectionPadding, BORDER_REPLICATE, Scalar(0,0,0));
			copyMakeBorder(_imgDil, _imgDil, sectionPadding, sectionPadding, sectionPadding, sectionPadding, BORDER_CONSTANT, Scalar(0,0,0));
			copyMakeBorder(_imgErode, _imgErode, sectionPadding, sectionPadding, sectionPadding, sectionPadding, BORDER_CONSTANT, Scalar(0,0,0));
			copyMakeBorder(_imgExp, _imgExp, sectionPadding, sectionPadding, sectionPadding, sectionPadding, BORDER_REPLICATE, Scalar(0,0,0));
			copyMakeBorder(_imgMorph, _imgMorph, sectionPadding, sectionPadding, sectionPadding, sectionPadding, BORDER_CONSTANT, Scalar(0,0,0));


			findContours(_imgDil, _contours, RETR_LIST, CHAIN_APPROX_NONE);
			findContours(_imgDil, _allContours, RETR_LIST, CHAIN_APPROX_NONE);

			drawContours(_imgExp,_allContours, -1, Scalar(255, 0, 0));

			//Delete this later on
			// imshow("Section" + to_string(i), _imgExp);
			// imshow("Section" + to_string(i) + "Morph", _imgMorph);
			// waitKey(100);

			int numberOfContoursFoundInThisSection = _contours.size();
			if(numberOfContoursFoundInThisSection < rows * columns) {
				cout <<"Invalid number of contours found"<<endl;
				cout <<"Rather found "<<numberOfContoursFoundInThisSection<<" which is less than the minimun "<<rows * columns<<endl;
				return false;
			}


			for(int j = 0; j < numberOfContoursFoundInThisSection; j++) {
				auto _bRect = boundingRect(_contours[j]);
				// int _area = contourArea(_contours[j]);
				// int _perimeter = arcLength(_contours[j], true);

				int _area = _bRect.height * _bRect.width;
				int _perimeter = 2 * (_bRect.height + _bRect.width);

				if(_perimeter > (2 * (minBubbleHeight + minBubbleWidth)) && _perimeter < (2 * (maxBubbleHeight + maxBubbleWidth)) && _area < (maxBubbleHeight * maxBubbleWidth) && _area > (minBubbleHeight * minBubbleWidth)) {
					Bubble _newBubble(_sections[i].boundingRect.tl());
					_newBubble.area = _area;
					_newBubble.contour = _contours[j];
					_newBubble.transMat = _sections[i].transMat;
					auto _boundingRect = boundingRect(_contours[j]);
					Rect _roi(_boundingRect.tl(), _boundingRect.br());
					cout<<"Started cropping the grayscale bubble"<<endl;
					_newBubble.img = _imgMorph(_roi).clone();
					_newBubble.isChecked = isChecked(_contours[j], _contours, _newBubble.img);
					if(_newBubble.isChecked) imwrite("../../Resources/selected/" + to_string(i) + to_string(j) + ".jpg", _newBubble.img);
					else imwrite("../../Resources/unselected/" + to_string(i) + to_string(j) + ".jpg", _newBubble.img);
					_newBubble.centroid = getCentroid(_contours[j], sectionResizeFactor, sectionResizeFactor);
					_sections[i].bubbles.push_back(_newBubble);
				}
			}

			// Delete this later on
			// imshow("Section" + to_string(i), _imgExp);
			// imshow("Section" + to_string(i) + "Morph", _imgMorph);
			// waitKey(100);

			int numberOfBubblesFoundInThisSection = _sections[i].bubbles.size();
			// imshow("section" + to_string(i), _sections[i].img);
			if(numberOfBubblesFoundInThisSection != rows * columns) {
            		cout <<"Invalid number of bubbles found"<<endl;
            		cout <<"Trying to fix this problem"<<endl;
            		for(int x = 0; x < rows * columns - numberOfBubblesFoundInThisSection; x++) {
            		    Bubble _newBubble(_sections[i].boundingRect.tl());
            				_newBubble.area = 24;
            				_newBubble.contour = {Point(0,0), Point(0, 5), Point(5, 0), Point(5,5)};
            				_newBubble.transMat = _sections[i].transMat;
            				auto _boundingRect = boundingRect(_newBubble.contour);
            				Rect _roi(_boundingRect.tl(), _boundingRect.br());
            				cout<<"Started cropping the grayscale bubble"<<endl;
            				_newBubble.img = _imgMorph(_roi).clone();
            				_newBubble.isChecked = isChecked(_newBubble.contour, _contours, _newBubble.img);
            				_newBubble.centroid = getCentroid(_newBubble.contour, sectionResizeFactor, sectionResizeFactor);
            				_sections[i].bubbles.push_back(_newBubble);
            			}
            			cout <<"Found "<<numberOfBubblesFoundInThisSection<<" bubbles in section "<<"Instead of "<<rows * columns<<" bubbles."<<i<<endl;
            			// return false;
            		}

			//sort bubbles in the section
			cout <<"STARTED SORTING SECTIONS "<<i<<" BUBBLES"<<endl;
			result = sortBubbles(_sections[i].bubbles, _sections[i].bubbles, columns, _sections[i].type);
			cout <<"COMPLETED SORTING SECTION "<<i<<" BUBBLES"<<endl;
			if(result == false) {
				return false;
			}

			//extract answers to questions
			for(int k = 0; k < rows; k++) {
				vector<Bubble> bubblesInRow;
				for(int r = 0; r < columns; r++) {
					bubblesInRow.push_back(_sections[i].bubbles[(k * columns + r)]);
				}
				Bubble _selectedBubble = getSelectedBubble(bubblesInRow);
				cout <<"now adding selected bubble"<<endl;
				_sections[i].selectedBubbles.push_back(_selectedBubble);
			}

		}

        for(int i = 0; i < totSectionCount; i++) {
            // imshow("section " + to_string(i), _sections[i].img);
			if(_sections[i].type == SCORE_SECTION) {
				//printing the preprocessed images
				string specificDir = string("score_digits/");
				cout<<"savePathDir: "<<savePathDir;
				platform_log("Rendering the score digit characters");
				string newSavePathDir = savePathDir + specificDir ;
				cout<<"newSavePathDir: "<<newSavePathDir;
				char* _nsPath = &newSavePathDir[0];
				platform_log("new path: %s", _nsPath);
				vector<Mat> prepImages  = NNprep().mnist(_sections[i].img, 1, 5, newSavePathDir);
			}

			if(_sections[i].type == NAME_SECTION) {
				//printing the preprocessed images
				string specificDir = string("name_chars/");
				cout<<"savePathDir: "<<savePathDir;
				platform_log("Rendering the name section characters");
				string newSavePathDir = savePathDir + specificDir;
				cout<<"newSavePathDir: "<<newSavePathDir;
				char* _nsPath = &newSavePathDir[0];
                platform_log("new path: %s", _nsPath);
				vector<Mat> prepImages  = NNprep().mnist(_sections[i].img, 2, 10, newSavePathDir);
			}
			
        }

		cout <<"response extraction complete"<<endl;
		//send the new _sections to the location referenced by _sections_ptr
		validForm.sections = _sections;
		return true;
}

char* _record(char* path, char* savePathDir, char* _studentId, char* _classId, char* _subjectId) {
Mat _image = imread(path);
byte* imageBytes = matToBytes(_image);
int imgHeight = _image.size().height;
int imgWidth = _image.size().width;
studentId = _studentId;
classId = _classId;
subjectId = _subjectId;

totSectionCount = 5;
numOfQSections = 0;
numOfIdSections = 3;
numOfNameSections = 1;
numOfScoreSections = 1;
qSectionRows = 10, qSectionColumns = 5;
idSectionRows = 10, idSectionColumns = 5;

//clear logs
logStr = "------------C++ LOGS-------------\n";

//reset variables
notFirstTime = false;

//string pathStr(path);
char* msg = "successful";
Mat image = bytesToMat(imageBytes, imgWidth, imgHeight);
	
Form form = extractSRForm(image, SCORE_FORM);
if(form.isValid) {
log("(ProcessImage) form is valid");
bool respExtSuccessful = extractSRBubbles(form, savePathDir);
            form.getResponses();
            if(true) {
                bool responsesMarked = form.markResponses();
                cout<<"successfully marked all responses"<<endl;
                if(responsesMarked){
                    log("-----------CONGRATULATION: THE ANSWER SHEET WAS SUCCESSFULLY GRADED-------------");
                    imwrite(path, form.segmentedImg);
                    return "0"; //LOGS();
                }else{
                    log("(ProcessImage) Failed to mark responses");
                    return "1"; //LOGS();
                }
            }else {
                log("(ProcessImage) Could not get all the responses from the form");
                return "2"; //LOGS();
            }
    }else {
        log("(ProcessImage) form is invalid");
        return "3"; //LOGS();
    }
}



#endif
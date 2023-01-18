#include "my_typedefs.hpp"
#include <iostream>
#include <stdlib.h>

//using namespace cv;
using namespace std;

#ifndef HYPERPARAMSDEFINED
#define HYPERPARAMSDEFINED


char* studentId;
char* classId;
char* subjectId;
bool notFirstTime = false;
int radius = 20;
float w = 420, h = 596;
int sectionResizeFactor = 2;
int sectionPadding = 10;

//hyperparameters 
int totSectionCount = 9;
int numOfQSections = 6;
int numOfIdSections = 3;
int numOfNameSections = 0;
int numOfScoreSections = 0;
int expNumOfResp; //correctAnswers.size()
int qSectionRows = 10, qSectionColumns = 5;
int idSectionRows = 10, idSectionColumns = 5;

//defining the various form types
string MCQ_FORM = "MCQ_FORM";
string STRUCT_FORM = "STRUCT_FORM";
string SCORE_FORM = "SCORE_FORM";

//defining the various types of sections
string QUESTION_SECTION = "QUESTION_SECTION";
string ID_SECTION = "ID_SECTION";
string NAME_SECTION = "NAME_SECTION";
string SCORE_SECTION = "SCORE_SECTION";


//This is used to crop the section height so that it doesn't include the section contours
int secCropWidth = 3 , secCropHeight = 3 ;
int bubbleCropWidth = 8 , bubbleCropHeight = 8;
#endif
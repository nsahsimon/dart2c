
#include "hyper_params.hpp"
#include "my_typedefs.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

#ifndef SECTIONSORTDEFINED
#define SECTIONSORTDEFINED

//tis function contains the different sorting algorithms for the different inputs
vector<Section> sortSecs(vector<Section> sections, string type, int subType) {
    if(type == MCQ_FORM) {
        if(subType == MCQ1){

        }else if(subtype == MCQ2){
            //first sort sections based on their aspect ratios i.e width/height
		//sort(sections.begin(), sections.end(), aspectRatioCmp);
		sort(sections.begin(), sections.end() , Ycmp);
		sort(sections.begin(), sections.end() - 6, Xcmp);// sorting out the response sets in the first row.
		sort(sections.begin() + 3, sections.end() - 3, Xcmp); //sorting out the response sets in the second row.
		sort(sections.begin() + 6, sections.end(), Xcmp); //sorting out the response sets in the third row.

		//sorting out the id contours vertically.
		//the code section comes first followed by the name section.
		//sort(sections.begin() + 6 , sections.end(), YcmpInv);

		//set the appropropriate section id for each section.
		//set the appropriate section type for each section.
		for(int i = 0; i < 3; i++) {
			int id = i + 1;
			sections[i].id = id;
			sections[i].rows = idSectionRows;
			sections[i].columns = idSectionColumns;
			sections[i].type = ID_SECTION;
			sections[i].transMat = getTranspose(sections[i].img, sections[i].img);
		}

        for(int i = 3; i < sections.size(); i++){
            int id = i + 1;
            sections[i].id = id;
			sections[i].rows = qSectionRows;
			sections[i].columns = qSectionColumns;
			sections[i].type = QUESTION_SECTION;
        }

        }else{

        }
        
    }
	    
	else if (type = SCORE_FORM) {

	} 
	else {

	}	
		return sections;
}


#endif
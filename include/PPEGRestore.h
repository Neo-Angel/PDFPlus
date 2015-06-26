

#ifndef __PDFPlusLib__PPEGRestore__
#define __PDFPlusLib__PPEGRestore__

#include <PPElement.h>

//
//      PPEGRestore
//
///////////////////////////////////////////////////////////////
class PPEGRestore : public PPElement {
    
public:
    PPEGRestore(PPContext *gcontext):PPElement(gcontext){}
	PPEGRestore(){}
	PPBase *Create(){return new PPEGRestore;}


    string makeCommandString();
    string xmlString(int level);
	PPElementType getType() {return PPET_GRESTORE;}

};



#endif
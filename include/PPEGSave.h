

#ifndef __PDFPlusLib__PPGSave__
#define __PDFPlusLib__PPGSave__

#include <PPElement.h>


//
//      PPEGSave
//
///////////////////////////////////////////////////////////////
class PPEGSave : public PPElement {
    
public:
    PPEGSave(PPContext *gcontext):PPElement(gcontext){}
	PPEGSave(){}
	PPBase *Create(){return new PPEGSave;}

    string makeCommandString();
    string xmlString(int level);
	PPElementType getType() {return PPET_GSAVE;}
};


#endif
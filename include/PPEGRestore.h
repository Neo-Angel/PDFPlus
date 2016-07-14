

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


    string MakeCommandString();
    string XMLString(int level);
	PPElementType Type() {return PPET_GRESTORE;}

};



#endif
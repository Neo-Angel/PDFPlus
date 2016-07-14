
#ifndef __PDFPlusLib__PPEEndMarkedContent__
#define __PDFPlusLib__PPEEndMarkedContent__

#include <PPElement.h>


//
//      PPEEndMarkedContent
//
///////////////////////////////////////////////////////////////
class PPEEndMarkedContent : public PPElement {
    
public:
    PPEEndMarkedContent(PPContext *gcontext) : PPElement(gcontext){}
	PPEEndMarkedContent(){}
	PPBase *Create(){return new PPEEndMarkedContent;}

	string MakeCommandString();
    string XMLString(int level);
	PPElementType Type() {return PPET_END_MARKED_CONTENT;}
};


#endif
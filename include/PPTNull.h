
#ifndef __PDFPlusLib__PPTNull__
#define __PDFPlusLib__PPTNull__

#include <PPToken.h>




///////////////////////////////////////// PPTNull
class PPTNull : public PPToken {

public:
	PPTNull() {}
    string XMLString(int level);
    string PDFString();
	PPBase *Create() {return new PPTNull();}
};
/////////////////////////////////////////

#endif


#ifndef __PDFPlusLib__PPTNull__
#define __PDFPlusLib__PPTNull__

#include <PPToken.h>




///////////////////////////////////////// PPTNull
class PPTNull : public PPToken {
    string xmlString(int level);
    string pdfString();
};
/////////////////////////////////////////

#endif




#ifndef __PDFPlusLib__PPTBool__
#define __PDFPlusLib__PPTBool__

#include <PPToken.h>




///////////////////////////////////////// PPTBool
class PPTBool : public PPToken {
    bool _bool;
public:
    PPTBool(PPParser *parser, string &str);
    string xmlString(int level);
    string pdfString();

};
/////////////////////////////////////////


#endif

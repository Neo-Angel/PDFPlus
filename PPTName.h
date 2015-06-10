
#ifndef __PDFPlusLib__PPTName__
#define __PDFPlusLib__PPTName__

#include <PPToken.h>





///////////////////////////////////////// PPTName
class PPTName : public PPToken {
public:
    string *_name;
    PPTName(PPParser *parser, string *str);
    ~PPTName();
//    string *getName();
    string description();
    string xmlString(int level);
    inline const char *classType(){return PPTN_NAME;};
    string pdfString();

};
/////////////////////////////////////////

#endif

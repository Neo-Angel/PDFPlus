

#ifndef __PDFPlusLib__PPTNumber__
#define __PDFPlusLib__PPTNumber__

#include <PPToken.h>

class PPParser;


///////////////////////////////////////// PPTNumber
class PPTNumber : public PPToken {
    string *_numstr;
    double _number;
    
public:
    PPTNumber(PPParser *parser, string *str);
    PPTNumber(PPParser *parser, int num);
	PPTNumber(PPParser *parser, float num);
    ~PPTNumber();
    float floatValue();
    long long longlongValue();
    long longValue();
	int intValue();
    string stringValue();
    string pdfString() {return stringValue();}
    string xmlString(int level);
    inline const char *classType(){return PPTN_NUMBER;};
};
/////////////////////////////////////////



#endif

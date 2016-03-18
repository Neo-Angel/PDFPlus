


#ifndef __PDFPlusLib__PPTBool__
#define __PDFPlusLib__PPTBool__

#include <PPToken.h>




///////////////////////////////////////// PPTBool
class PPTBool : public PPToken {
public:
    bool _bool;

public:
    PPTBool(PPParser *parser, string &str);
	PPTBool();
    string XMLString(int level);
    string pdfString();

	PPBase *Create() {return new PPTBool();}
	void CopyMembersTo(PPBase *obj) ;
};
/////////////////////////////////////////


#endif

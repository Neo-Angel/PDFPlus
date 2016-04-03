
#ifndef __PDFPlusLib__PPTName__
#define __PDFPlusLib__PPTName__

#include <PPToken.h>





///////////////////////////////////////// PPTName
class PPTName : public PPToken {
public:
    string *_name;
    PPTName(PPDocument *doc, string *str);
	PPTName(PPDocument *doc, string str);
	PPTName() {_name = NULL;}
    ~PPTName();
//    string *getName();
    string Description();
    string XMLString(int level);
    inline const char *ClassType(){return PPTN_NAME;};
    string pdfString();

	PPBase *Create() {return new PPTName();}
	void CopyMembersTo(PPBase *obj) ;

};
/////////////////////////////////////////

#endif

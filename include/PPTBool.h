


#ifndef __PDFPlusLib__PPTBool__
#define __PDFPlusLib__PPTBool__

#include <PPToken.h>



// PPTBool
/////////////////////////////////////////////////////////////
// 'true'나 'false' 두개의 값 중 하나를 나타냄
/////////////////////////////////////////////////////////////
class PPTBool : public PPToken {
public:
    bool _bool;

public:
    PPTBool(PPDocument *doc, string &str);
	PPTBool();
    string XMLString(int level);
    string PDFString();

	PPBase *Create() {return new PPTBool();}
	void CopyMembersTo(PPBase *obj) ;
};
/////////////////////////////////////////


#endif

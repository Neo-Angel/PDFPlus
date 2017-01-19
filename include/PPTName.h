
#ifndef __PDFPlusLib__PPTName__
#define __PDFPlusLib__PPTName__

#include <PPToken.h>





///////////////////////////////////////// PPTName
class PPTName : public PPToken {
public:
	// 객체 외부에서 할당된 메모리를 사용하나
	// 메모리 해제는 이 객체가 소멸될 때 같이 해제한다.
    string *_name;

public:
    PPTName(PPDocument *doc, string *str);
	PPTName(PPDocument *doc, string str);
	PPTName() {_name = NULL;}
    ~PPTName();
//    string *getName();
    string Description();
    string XMLString(int level);
    inline PPClassType ClassType(){return PPTN_NAME;};
    string PDFString();

	PPBase *Create() {return new PPTName();}
	void CopyMembersTo(PPBase *obj) ;

};
/////////////////////////////////////////

#endif

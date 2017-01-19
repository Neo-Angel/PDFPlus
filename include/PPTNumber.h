

#ifndef __PDFPlusLib__PPTNumber__
#define __PDFPlusLib__PPTNumber__

#include <PPToken.h>

class PPParser;


///////////////////////////////////////// PPTNumber
// 숫자를 다루기 위한 클래스 
class PPTNumber : public PPToken {
public:
	// 객체 외부에서 할당된 메모리를 사용하나
	// 메모리 해제는 이 객체가 소멸될 때 같이 해제한다.
    string *_numstr; 
    double _number;
    
public:
    PPTNumber(PPDocument *doc, string *str);
    PPTNumber(PPDocument *doc, int num);
    PPTNumber(PPDocument *doc, uint num);
	PPTNumber(PPDocument *doc, float num);
	PPTNumber() {_numstr = NULL; _number = 0.0;}
    ~PPTNumber();
    float floatValue();
    long long longlongValue();
    long longValue();
	int intValue();
    string stringValue();
    string PDFString() {return stringValue();}
    string XMLString(int level);
    inline PPClassType ClassType(){return PPTN_NUMBER;}

	PPBase *Create() {return new PPTNumber();}
	void CopyMembersTo(PPBase *obj);


};
/////////////////////////////////////////



#endif

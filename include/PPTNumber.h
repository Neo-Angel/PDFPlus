

#ifndef __PDFPlusLib__PPTNumber__
#define __PDFPlusLib__PPTNumber__

#include <PPToken.h>

class PPParser;


///////////////////////////////////////// PPTNumber
// ���ڸ� �ٷ�� ���� Ŭ���� 
class PPTNumber : public PPToken {
public:
	// ��ü �ܺο��� �Ҵ�� �޸𸮸� ����ϳ�
	// �޸� ������ �� ��ü�� �Ҹ�� �� ���� �����Ѵ�.
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

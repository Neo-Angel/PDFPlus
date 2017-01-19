
#ifndef __PDFPlusLib__PPTName__
#define __PDFPlusLib__PPTName__

#include <PPToken.h>





///////////////////////////////////////// PPTName
class PPTName : public PPToken {
public:
	// ��ü �ܺο��� �Ҵ�� �޸𸮸� ����ϳ�
	// �޸� ������ �� ��ü�� �Ҹ�� �� ���� �����Ѵ�.
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

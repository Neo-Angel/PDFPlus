
#ifndef __PDFPlusLib__PPTIndirectRef__
#define __PDFPlusLib__PPTIndirectRef__

#include <PPToken.h>



///////////////////////////////////////// PPTIndirectRef

// _objNum�� IndirectObj ��ü�� ����Ų��.
// �ش� IndirectObj�� _array �� �������� ��ü�Ѵ�. 
class PPTIndirectObj;
class PPTIndirectRef : public PPToken {
public:
    int _objNum;// Target Object Number , ��ü ��ȣ
    int _genNum;
    
	PPTIndirectRef(){_objNum = 0; _genNum = 0;}
    PPTIndirectRef(PPDocument *doc, int num1, int num2);
    string Description();

    string XMLString(int level);
    string PDFString();
    inline PPClassType ClassType(){return PPTN_INDIRECTREF;};
	void MoveInto(PPDocument *doc);
	PPBase *Create() {return new PPTIndirectRef();}
	void CopyMembersTo(PPBase *obj) ;

    PPTIndirectObj *TargetObject(); // ��ť��Ʈ ���� ��ü ��ȣ�� _objNum �� IndirectObj
    PPToken *ValueObject(); // Ÿ�� IndirectObj�� _array�� ù��° ������(Token)
};
/////////////////////////////////////////

#endif

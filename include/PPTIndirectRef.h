
#ifndef __PDFPlusLib__PPTIndirectRef__
#define __PDFPlusLib__PPTIndirectRef__

#include <PPToken.h>



///////////////////////////////////////// PPTIndirectRef

// _objNum의 IndirectObj 객체를 가르킨다.
// 해당 IndirectObj의 _array 의 내용으로 대체한다. 
class PPTIndirectObj;
class PPTIndirectRef : public PPToken {
public:
    int _objNum;// Target Object Number , 객체 번호
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

    PPTIndirectObj *TargetObject(); // 도큐먼트 내의 객체 번호가 _objNum 인 IndirectObj
    PPToken *ValueObject(); // 타겟 IndirectObj의 _array의 첫번째 아이템(Token)
};
/////////////////////////////////////////

#endif


#ifndef __PDFPlusLib__PPTIndirectObj__
#define __PDFPlusLib__PPTIndirectObj__

#include <PPToken.h>



///////////////////////////////////////// PPTIndirectObj

class PPTStream;
class PPTIndirectRef;
class PPTDictionary;


// '<_objNum> <_genNum> obj' 형식의 객체로 '_objNum'이 이 객체의 ID역할을 한다.
////////////////////////////////////////////////////////////////////////
class PPTIndirectObj : public PPToken {
private:
	vector <PPToken *> _array; 

public:
    int _objNum; // Object Number , 객체 번호
    int _genNum;
    
	// 내용들, 주로 PPToken에게 상속된 객체들

	// 이 객체를 가리키고있는 리퍼런스 목록들.
	// _objNum가 바뀔경우 이 목록에 있는 리퍼런스들의
	// '_objNum'값들도 변경시켜준다.
	vector <PPTIndirectRef *> _ref_list;	
											 
public:
    PPTIndirectObj(PPDocument *doc, vector<PPToken *> token_list, int num1, int num2);
	PPTIndirectObj(PPDocument *doc, int num1, int num2);
	PPTIndirectObj() {_objNum=0;_genNum=0;}
    ~PPTIndirectObj();

    string Description();
    string XMLString(int level);
    string PDFString();
    inline PPClassType ClassType(){return PPTN_INDIRECTOBJ;};
    
	// CopyMembersTo 멤버를 계승 받기 위해선 Create 멤버함수를 구현해야함.
	PPBase *Create() {return new PPTIndirectObj();}
	void CopyMembersTo(PPBase *obj) ;
	void SetDocument(PPDocument *doc);
	void MoveInto(PPDocument *doc);

	vector <PPToken *>::iterator Begin() {return _array.begin();}
	vector <PPToken *>::iterator End() {return _array.end();}
	void Erase(vector <PPToken *>::iterator it) {_array.erase(it);}
	size_t NumberOfTokens() {return _array.size();};
	PPToken *TokenAtIndex(int idx) {return _array[idx];}
	void AddToken(PPToken *token);
	void Clear();
	vector <PPToken *> & TokenList() {return _array;}


    bool IsObjStream();
    bool IsStream();
    PPTDictionary *FirstDictionary();
	void SetFirstObject(PPToken *obj);
    PPTStream *Stream();
	PPTIndirectObj *GetParentObj();

	// OCG related
	bool IsOCG();
	string OCGName();
    
    void AddRefObj(PPTIndirectRef *ref);
    void SetObjNum(int num);
	int ObjNum(){return _objNum;}
    
    void Write(std::ostream &os);
    void Merge(PPTIndirectObj *ohter_indir);
};
/////////////////////////////////////////

#endif

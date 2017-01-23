
#ifndef __PDFPlusLib__PPTIndirectObj__
#define __PDFPlusLib__PPTIndirectObj__

#include <PPToken.h>



///////////////////////////////////////// PPTIndirectObj

class PPTStream;
class PPTIndirectRef;
class PPTDictionary;


// '<_objNum> <_genNum> obj' ������ ��ü�� '_objNum'�� �� ��ü�� ID������ �Ѵ�.
////////////////////////////////////////////////////////////////////////
class PPTIndirectObj : public PPToken {
private:
	vector <PPToken *> _array; 

public:
    int _objNum; // Object Number , ��ü ��ȣ
    int _genNum;
    
	// �����, �ַ� PPToken���� ��ӵ� ��ü��

	// �� ��ü�� ����Ű���ִ� ���۷��� ��ϵ�.
	// _objNum�� �ٲ��� �� ��Ͽ� �ִ� ���۷�������
	// '_objNum'���鵵 ��������ش�.
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
    
	// CopyMembersTo ����� ��� �ޱ� ���ؼ� Create ����Լ��� �����ؾ���.
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

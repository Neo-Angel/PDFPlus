
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
public:
    int _objNum; // Object Number , ��ü ��ȣ
    int _genNum;
    
	// �����, �ַ� PPToken���� ��ӵ� ��ü��
	vector <PPToken *> _array; 

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
    inline const char *ClassType(){return PPTN_INDIRECTOBJ;};
    
	// CopyMembersTo ����� ��� �ޱ� ���ؼ� Create ����Լ��� �����ؾ���.
	PPBase *Create() {return new PPTIndirectObj();}
	void CopyMembersTo(PPBase *obj) ;
	void SetDocument(PPDocument *doc);
	void MoveInto(PPDocument *doc);

    bool IsObjStream();
    bool IsStream();
    PPTDictionary *FirstDictionary();
	void AddObj(PPToken *obj);
    PPTStream *Stream();
    
    void AddRefObj(PPTIndirectRef *ref);
    void SetObjNum(int num);
	int ObjNum(){return _objNum;}
    
    void Write(std::ostream &os);
    void Merge(PPTIndirectObj *ohter_indir);
    
};
/////////////////////////////////////////

#endif

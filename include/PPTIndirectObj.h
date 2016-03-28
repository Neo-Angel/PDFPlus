
#ifndef __PDFPlusLib__PPTIndirectObj__
#define __PDFPlusLib__PPTIndirectObj__

#include <PPToken.h>



///////////////////////////////////////// PPTIndirectObj

class PPTStream;
class PPTIndirectRef;
class PPTDictionary;

class PPTIndirectObj : public PPToken {

private:

public:
    int _objNum;
    int _genNum;
//    unsigned long long _filept;
    
	vector <PPToken *> _array;
	vector <PPTIndirectRef *> _ref_list;

    
    PPTIndirectObj(PPParser *parser, vector<PPToken *> token_list, int num1, int num2);
	PPTIndirectObj(PPParser *parser, int num1, int num2);
	PPTIndirectObj() {_objNum=0;_genNum=0;}
    ~PPTIndirectObj();

    string Description();
    string XMLString(int level);
    string pdfString();
//    string pdfString(std::ostream &os);
    
    bool isObjStream();
    bool isStream();
    inline const char *ClassType(){return PPTN_INDIRECTOBJ;};
    PPTDictionary *firstDictionary();
	PPTDictionary *FirstDictionary(){return firstDictionary();}
	void AddObj(PPToken *obj);
    PPTStream *stream();
    
    void addRefObj(PPTIndirectRef *ref);
    void setObjNum(int num);
	int getObjNum(){return _objNum;}
    
    void Write(std::ostream &os);
    void merge(PPTIndirectObj *ohter_indir);
    
	// CopyMembersTo 멤버를 계승 받기 위해선 Create 멤버함수를 구현해야함.
	PPBase *Create() {return new PPTIndirectObj();}
	void CopyMembersTo(PPBase *obj) ;
	void SetParser(PPParser *parser);
	void MoveInto(PPDocument *doc);
};
/////////////////////////////////////////

#endif

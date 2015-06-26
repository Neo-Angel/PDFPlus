
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
    ~PPTIndirectObj();
    string description();
    string xmlString(int level);
    string pdfString();
//    string pdfString(std::ostream &os);
    
    bool isObjStream();
    bool isStream();
    inline const char *classType(){return PPTN_INDIRECTOBJ;};
    PPTDictionary *firstDictionary();
	void AddObj(PPToken *obj);
    PPTStream *stream();
    
    void addRefObj(PPTIndirectRef *ref);
    void setObjNum(int num);
	int getObjNum(){return _objNum;}
    
    void write(std::ostream &os);
    void merge(PPTIndirectObj *ohter_indir);
    
};
/////////////////////////////////////////

#endif

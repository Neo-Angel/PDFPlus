
#ifndef __PDFPlusLib__PPTTrailer__
#define __PDFPlusLib__PPTTrailer__

#include <PPToken.h>



///////////////////////////////////////// PPTTrailer
class PPTArray;
class PPTDictionary;
class PPTIndirectObj;
class PPTIndirectRef;
class PPTXRef;

class PPTTrailer : public PPToken {
    PPTDictionary *_dict;

public:
    PPTIndirectObj *_xrefIndirect;
    long long _startxref;  // 파일 상의 물리적 위치
    PPTXRef *_xrefObj;
    PPTTrailer *_next;
    
    PPTTrailer(PPParser *parser, PPTDictionary *dict, unsigned long long xref);
    PPTTrailer(PPParser *parser, PPTIndirectObj *indir, unsigned long long xref);
    PPTTrailer(PPParser *parser, unsigned long long xref);
    PPTTrailer(PPParser *parser);
	PPTTrailer();
    ~PPTTrailer();
    
    string XMLString(int level);
    string pdfString();
    inline const char *ClassType(){return PPTN_TRAILER;};
    PPTDictionary *getDictionary();
    void Write(std::ostream &os);

    PPToken *rootObject();
    PPToken *infoObject();
    void setRootObject(PPTIndirectRef *indir_obj);
    void setInfoObject(PPTIndirectRef *indir_obj);
    
    void setFileID(PPTArray *idarr);
    void build();
    void merge(PPTTrailer *trailer);

	PPBase *Create() {return new PPTTrailer();}
	void CopyMembersTo(PPBase *obj);
	void SetParser(PPParser *parser);


};

/////////////////////////////////////////

#endif

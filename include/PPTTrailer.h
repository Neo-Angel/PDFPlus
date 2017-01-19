
#ifndef __PDFPlusLib__PPTTrailer__
#define __PDFPlusLib__PPTTrailer__

#include <PPToken.h>



///////////////////////////////////////// PPTTrailer
class PPTArray;
class PPTDictionary;
class PPTIndirectObj;
class PPTIndirectRef;
class PPTXRef;

// Root Object, Info Object, Cross Reference 등의 정보를 담고있다. 
class PPTTrailer : public PPToken {
protected:
    PPTDictionary *_dict;

public:
    PPTIndirectObj *_xrefIndirect;
    long long _startxref;  // Cross Reference Table 의 파일 상의 물리적 위치
    PPTXRef *_xrefObj;
    PPTTrailer *_next;

public:
    PPTTrailer(PPDocument *doc, PPTDictionary *dict, unsigned long long xref);
    PPTTrailer(PPDocument *doc, PPTIndirectObj *indir, unsigned long long xref);
    PPTTrailer(PPDocument *doc, unsigned long long xref);
    PPTTrailer(PPDocument *doc);
	PPTTrailer();
    ~PPTTrailer();
    
    string XMLString(int level);
    string PDFString();
	PPBase *Create() {return new PPTTrailer();}
	void CopyMembersTo(PPBase *obj);
    inline PPClassType ClassType(){return PPTN_TRAILER;};

    PPTDictionary *Dictionary();
    void Write(std::ostream &os);

    PPToken *RootObject();
    PPToken *InfoObject();
    void SetRootObject(PPTIndirectRef *indir_obj);
    void SetInfoObject(PPTIndirectRef *indir_obj);
    
    void SetFileID(PPTArray *idarr);
    void Build();
    void Merge(PPTTrailer *trailer);

	void SetDocument(PPDocument *doc);


};

/////////////////////////////////////////

#endif

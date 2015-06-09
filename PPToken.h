//
//  PPToken.h
//  PDFPlusLib
//
//  Created by Neo Angel on 2014. 9. 11..
//  Copyright (c) 2014년 #CORP#. All rights reserved.
//

#ifndef __PDFPlusLib__PPToken__
#define __PDFPlusLib__PPToken__

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "PPBase.h"


class PPParser;


using namespace std;

// Class Type Name
static const char  *PPTN_NUMBER =       "PPTNumber";
static const char  *PPTN_NAME =         "PPTName";
static const char  *PPTN_STRING =       "PPTSTRING";
static const char  *PPTN_INDIRECTOBJ =  "PPTIndirectObj";
static const char  *PPTN_INDIRECTREF =  "PPTIndirectRef";
static const char  *PPTN_DICTIONARY =   "PPTDictionary";
static const char  *PPTN_ARRAY =        "PPTArray";
static const char  *PPTN_STREAM =       "PPTStream";
static const char  *PPTN_TRAILER =      "PPTTrailer";
static const char  *PPTN_XREF =         "PPTXRef";

// PDF Spec. Key Name
static const char	*PPKN_ROOT = "Root"; // PPTrailer
static const char	*PPKN_INFO = "Info";
static const char	*PPKN_FILEID = "ID";

static const char	*PPKN_CREATOR	= "Creator";
static const char	*PPKN_AUTHOR	= "Author";
static const char	*PPKN_CREATIONDATE = "CreationDate";
static const char	*PPKN_MODDATE = "ModDate";
static const char	*PPKN_PRODUCER = "Producer";
static const char	*PPKN_SUBJECT = "Subject";
static const char	*PPKN_TITLE = "Title";

static const char	*PPKN_PREV = "Prev";
static const char	*PPKN_KIDS = "Kids";
static const char	*PPKN_TYPE = "Type";
static const char	*PPKN_PAGE = "Page";

static const char	*PPKN_VERSION = "Version";
static const char	*PPKN_PAGELAYOUT = "PageLayout";
static const char	*PPKN_PAGEMODE = "PageMode";
static const char	*PPKN_PAGES = "Pages";
static const char	*PPKN_RESOURCES = "Resources";
static const char	*PPKN_CONTENTS = "Contents";

static const char	*PPKN_PARENT = "Parent";

// PDF Spec. Value Name
static const char *PPVN_CREATOR = "PDFPlus";
static const char *PPVN_CATALOG = "Catalog";

void PPwstrToUtf8(string& dest, const wstring& src);
string PPwstrToUtf8(const wstring& str);
void stringToWString(wstring &wstr, string &str);
void PPstringToUTF8String(string &utf8str, string &str);

///////////////////////////////////////// PPToken
class PPToken : public PPBase {
public:
    PPParser *_parser;

    PPToken();
    PPToken(PPParser *parser);
    unsigned long long _filepos;
    virtual string xmlString(int level) = 0; //  level : tap count
    virtual string internalXmlString(int level) {return xmlString(level);} //  level : tap count
    virtual string pdfString() {return "";}
    virtual string pdfString(ostream &os) {return pdfString();}
    virtual void write(std::ostream &os);

};
/////////////////////////////////////////



///////////////////////////////////////// PPTName
class PPTName : public PPToken {
public:
    string *_name;
    PPTName(PPParser *parser, string *str);
    ~PPTName();
//    string *getName();
    string description();
    string xmlString(int level);
    inline const char *classType(){return PPTN_NAME;};
    string pdfString();

};
/////////////////////////////////////////

///////////////////////////////////////// PPTArray
class PPTArray : public PPToken {
public:
    vector <PPToken *> _array;
    
	PPTArray(PPParser *parser) : PPToken(parser) {};
    PPTArray(PPParser *parser, vector<PPToken *> token_list);
    ~PPTArray();
    string internalXmlString(int level);
    string xmlString(int level);
    inline size_t size() {return _array.size();}
    inline PPToken *objectAtIndex(int idx) {return _array.at(idx);}
    inline const char *classType() {return PPTN_ARRAY;}
    string pdfString();
	void AddToken(PPToken *token) {_array.push_back(token);}
	void AddToken(int num);

};
/////////////////////////////////////////

///////////////////////////////////////// PPTDictionary
class PPTIndirectObj;

class PPTDictionary : public PPToken {

public:
    map <string, PPToken *> _dict;

    PPTDictionary(PPParser *parser);
    ~PPTDictionary();
    void setTokenAndKey(PPToken *token, string key);
	void SetTokenAndKey(PPToken *token, string key){setTokenAndKey(token, key);}
	void SetTokenAndKey(string name, string key);
	PPTIndirectObj *SetRefTokenAndKey(PPToken *token, string key, int obj_num);
    PPToken *objectForKey(string &keyname);
    PPToken *objectForKey(const char *keyname);
	PPTName *nameForKey(const char *keyname);
    PPToken *indirectObjectForKey(string &keyname);
    PPToken *valueObjectForKey(string &keyname);
    PPToken *indirectObjectForKey(const char *keyname){string str(keyname);return indirectObjectForKey(str);}
    PPToken *valueObjectForKey(const char *keyname){string str(keyname);return valueObjectForKey(str);}
    string description();
    string xmlString(int level);
    string internalXmlString(int level);
    string pdfString();
    
    inline const char *classType(){return PPTN_DICTIONARY;};
};
/////////////////////////////////////////

///////////////////////////////////////// PPStreamBuf

struct PPData;

class PPStreamBuf {
protected:
    vector<PPData *> _streamBuf;
    size_t _length;
public:
    PPStreamBuf();
    ~PPStreamBuf();
    void appendBuf(char *buf, size_t len);
    size_t totalLength();
    void collect(char *buf);
};

/////////////////////////////////////////


///////////////////////////////////////// PPONull
class PPTNull : public PPToken {
    string xmlString(int level);
    string pdfString();
};
/////////////////////////////////////////

///////////////////////////////////////// PPTIndirectObj
class PPTStream;
class PPTIndirectRef;

class PPTIndirectObj : public PPToken {
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
    PPTStream *stream();
    
    void addRefObj(PPTIndirectRef *ref);
    void setObjNum(int num);
	int getObjNum(){return _objNum;}
    
    void write(std::ostream &os);
    void merge(PPTIndirectObj *ohter_indir);
    
};
/////////////////////////////////////////

///////////////////////////////////////// PPTIndirectRef
class PPTIndirectRef : public PPToken {
public:
    int _objNum;
    int _genNum;
    
    PPTIndirectRef(PPParser *parser, int num1, int num2);
    string description();

    string xmlString(int level);
    string pdfString();
    string internalXmlString(int level);
    PPTIndirectObj *targetObject();
    PPToken *valueObject();

    inline const char *classType(){return PPTN_INDIRECTREF;};
};
/////////////////////////////////////////


///////////////////////////////////////// PPTXRef
typedef struct {
    unsigned long long  offset;
    unsigned int        generation;
    char                type;
} XRefItem;

class PPTTrailer;

class PPTXRef : public PPToken {
public:
    map <unsigned int, XRefItem *> _xreflist;
    PPTTrailer *_trailerObj;

    PPTXRef(PPParser *parser);
    ~PPTXRef();
    void addXRef(unsigned int objnum, unsigned long long offset, int gennum, char type);
    size_t numberOfItems();
    string xmlString(int level);
    string pdfString();
    inline const char *classType() {return PPTN_XREF;}
    void write(std::ostream &os);
    
    void merge(PPTXRef *trailer);
    
};
/////////////////////////////////////////


///////////////////////////////////////// PPTTrailer
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
    ~PPTTrailer();
    
    string xmlString(int level);
    string pdfString();
    inline const char *classType(){return PPTN_TRAILER;};
    PPTDictionary *getDictionary();
    void write(std::ostream &os);

    PPToken *rootObject();
    PPToken *infoObject();
    void setRootObject(PPTIndirectRef *indir_obj);
    void setInfoObject(PPTIndirectRef *indir_obj);
    
    void setFileID(PPTArray *idarr);
    void build();
    void merge(PPTTrailer *trailer);
};

/////////////////////////////////////////

#endif /* defined(__PDFPlusLib__PPToken__) */

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

#include <PPBase.h>


class PPParser;
class PPDocument;

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
//	virtual PPToken *Copy(PPParser *parser);

	// 모든 하위 토큰들을 통해 IndirectRef에서 IndirectObj 들을 doc 소속으로 변경함.
	virtual void MoveInto(PPDocument *doc) {}; // 유일하게 PPDocument::WriteResource() 에서 쓰임
	virtual void SetParser(PPParser *parser);
//	PPBase *Create() {return new PPToken();} // 추상 클래스라서 사용할 수 없답니다. 
	PPBase *Copy() {return PPBase::Copy();};


	void CopyMembersTo(PPBase *obj) ;

};
/////////////////////////////////////////




#endif /* defined(__PDFPlusLib__PPToken__) */

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
// PPToken의 서브클래스들의 ClassType() 리턴값
//static const char  *PPTN_TOKEN =		"PPToken";
//static const char  *PPTN_NUMBER =       "PPTNumber";
//static const char  *PPTN_NAME =         "PPTName";
//static const char  *PPTN_STRING =       "PPTString";
//static const char  *PPTN_INDIRECTOBJ =  "PPTIndirectObj";
//static const char  *PPTN_INDIRECTREF =  "PPTIndirectRef";
//static const char  *PPTN_DICTIONARY =   "PPTDictionary";
//static const char  *PPTN_ARRAY =        "PPTArray";
//static const char  *PPTN_STREAM =       "PPTStream";
//static const char  *PPTN_TRAILER =      "PPTTrailer";
//static const char  *PPTN_XREF =         "PPTXRef";


// PDF Spec. Key Name
// PDF 내용중 딕셔너리 타입의 객체에서 사용되어지는 키 모음  
static const char	*PPKN_ROOT =		"Root"; // PPTrailer
static const char	*PPKN_INFO =		"Info";
static const char	*PPKN_FILEID =		"ID";

static const char	*PPKN_CREATOR	=	"Creator";
static const char	*PPKN_AUTHOR	=	"Author";
static const char	*PPKN_CREATIONDATE ="CreationDate";
static const char	*PPKN_MODDATE =		"ModDate";
static const char	*PPKN_PRODUCER =	"Producer";
static const char	*PPKN_SUBJECT =		"Subject";
static const char	*PPKN_TITLE =		"Title";

static const char	*PPKN_PREV =		"Prev";
static const char	*PPKN_KIDS =		"Kids";
static const char	*PPKN_TYPE =		"Type";
static const char	*PPKN_PAGE =		"Page";

static const char	*PPKN_VERSION =		"Version";
static const char	*PPKN_PAGELAYOUT =	"PageLayout";
static const char	*PPKN_PAGEMODE =	"PageMode";
static const char	*PPKN_PAGES =		"Pages";
static const char	*PPKN_RESOURCES =	"Resources";
static const char	*PPKN_CONTENTS =	"Contents";

static const char	*PPKN_PARENT =		"Parent";

// PDF Spec. Value Name
// PDF내용중 딕셔너리 키에 대한 값들 중 문자열 상수들.
static const char *PPVN_CREATOR =		"PDFPlus"; // Key : "Creator"
static const char *PPVN_CATALOG =		"Catalog";  // Key : "Type"

class PPTIndirectObj;

// PPToken
///////////////////////////////////////////////////////////////
// PDF 자료구조를 이루는 각 종 토큰 객체들의 기반 클래스
/////////////////////////////////////////////////////////////////
class PPToken : public PPBase {
public:
    PPDocument *_document; //
	PPTIndirectObj *_parentObj;

    unsigned long long _filepos; // PDF 파일상에 이 토큰의 물리적 위치(byte 단위)

public:
    PPToken();
    PPToken(PPDocument *doc);
	// PDF 파일의 내용을 XML로 내보낼 경우 이 토큰의 XML 문자열 생성함.
    virtual string XMLString(int level) = 0; //  level : tap count

    virtual string PDFString() {return "";}  // 실제 PDF에 기록될 문자열을 리턴한다.
    virtual string PDFString(ostream &os) {return PDFString();}
    virtual void Write(std::ostream &os);

	// 모든 하위 토큰들을 IndirectRef에서 IndirectObj 들을 doc 소속으로 변경함.
	virtual void MoveInto(PPDocument *doc); 
	virtual PPToken *CopyInto(PPDocument *doc);

	virtual void SetDocument(PPDocument *doc);
//	PPBase *Create() {return new PPToken();} // 추상 클래스라서 사용할 수 없답니다. 

	PPClassType ClassType() {return PPTN_TOKEN;}

	void CopyMembersTo(PPBase *obj) ; // Inherited from PPBase

};

/////////////////////////////////////////

#endif /* defined(__PDFPlusLib__PPToken__) */

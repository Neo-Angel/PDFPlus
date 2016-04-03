//
//  PPParser.h
//  PDFPlusLib
//
//  Created by Neo Angel on 2014. 9. 14..
//  Copyright (c) 2014년 #CORP#. All rights reserved.
//

#ifndef __PDFPlusLib__PPParser__
#define __PDFPlusLib__PPParser__

#include <iostream>
#include <vector>
#include <map>
#include "PPBase.h"
#include "PPToken.h"
#include "PPTBool.h"

using namespace std;

class PPToken;
class PPParser;

typedef enum {
    PPPR_Ok,
    PPPR_CannotOpen,
    PPPR_ParsingError
    
}PPParserReturn;

// PPParser 를 이용해 파싱하는데 있어서 각각의 용도(소스의 특성)에 맞게 
// 코드를 핸들링 할 수 있도록 함. PPParser에서는 이 클래스의 인스턴스를
// 이용해 파싱에 필요한 코드들을 공급받음.
// 사용 예 : PDF 도큐먼트, 그래픽 코맨드, 오브젝 스트림, ...

class PPParserSource { // : PPBase {
public:
    virtual bool canParseString(string str) = 0;
    virtual PPToken *parseString(string str, vector <PPToken *> &tokens, PPParser *parser) = 0;
    virtual void get(char &ch) = 0;
    virtual bool eof() = 0;
    virtual size_t tellg() = 0;
    virtual void seekg(size_t pos) = 0;
    virtual void read(char *buf, size_t size) = 0;
    virtual void getline(char *buf, size_t size) = 0;
};

// PPToken의 서브 클래스들
// PPParser에 의해서 파싱될 객체들의 종류들
//class PPTBool;
class PPTNumber;
class PPTString;
class PPTName;
class PPTArray;
class PPTDictionary;
class PPTStream;
class PPTIndirectObj;
class PPTIndirectRef;
class PPTComment;


// PDF의 기본적인 신텍스 룰에 따라 파싱기능을 제공함.
// 용도(혹은 ParseSource (PPParseSource의 서브클래스))에 따라 PDF 메인 코드들을 
// 파싱하거나 스트림등의 지엽적인 부분들을 파싱할 수 있음.
class PPParser : PPBase {
	
protected:
    PPTBool									*parseBool(PPParserSource &source, char start_ch);
	PPTNumber								*parseNumber(PPParserSource &source, char start_ch);
    PPTString								*parseHexString(PPParserSource &source);
    PPTString								*parseStringObj(PPParserSource &source);
    PPTName									*parseName(PPParserSource &source);
    PPTArray								*parseArray(PPParserSource &source);
    PPTDictionary							*parseDictionary(PPParserSource &source);
    PPTStream								*parseStream(PPParserSource &source, unsigned long length);
    PPTStream								*parseStream(PPParserSource &source);
    PPTIndirectObj							*parseIndirectObj(PPParserSource &source, PPTNumber *num1, PPTNumber *num2);
    PPTComment								*parseComment(PPParserSource &source);

public:
	PPDocument *							_document; // mainly PPDocument(?)

public:
											~PPParser(); // 소멸자		
	// 메인 파서 함수. 
	// source에서 데이터를 읽어들여 파싱한 후 token_list에 Array로 담아 냄.
    bool									ParseSource(PPParserSource &source, vector<PPToken *> &token_list);  // start parsing
};

#endif /* defined(__PDFPlusLib__PPParser__) */

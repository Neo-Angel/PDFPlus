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

using namespace std;

class PPToken;
class PPParser;

typedef enum {
    PPPR_Ok,
    PPPR_CannotOpen,
    PPPR_ParsingError
    
}PPParserReturn;


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


class PPParser : PPBase {
protected:
//    PPParserSource *_source;

    PPTComment *parseComment(PPParserSource &source);
    PPTBool *parseBool(PPParserSource &source, char start_ch);
    PPTNumber *parseNumber(PPParserSource &source, char start_ch);
    PPTString *parseHexString(PPParserSource &source);
    PPTString *parseStringObj(PPParserSource &source);
    PPTName *parseName(PPParserSource &source);
    PPTArray *parseArray(PPParserSource &source);
    PPTDictionary *parseDictionary(PPParserSource &source);
    PPTStream *parseStream(PPParserSource &source, unsigned long length);
    PPTStream *parseStream(PPParserSource &source);
    PPTIndirectObj *parseIndirectObj(PPParserSource &source, PPTNumber *num1, PPTNumber *num2);
    
public:
    unsigned int _last_obj_idx;
    map <int, PPTIndirectObj *> _objDict;
    map <unsigned long long, PPToken *> _filePtDict; // IndirectObj, Trailer, XRef
    vector <PPTIndirectRef *> _ref_list;

    
    ~PPParser();
    map <int, PPTIndirectObj *> &objectsDictionary();
    PPToken *objectByID(int objid);
    PPToken *objectAtFilePosition(unsigned long long pos);

    bool parseSource(PPParserSource &source, vector<PPToken *> &token_list);  // startParse
};

#endif /* defined(__PDFPlusLib__PPParser__) */

//
//  PPTStream.h
//  PDFPlusLib
//
//  Created by Neo Angel on 2014. 11. 9..
//  Copyright (c) 2014년 #CORP#. All rights reserved.
//

#ifndef __PDFPlusLib__PPTStream__
#define __PDFPlusLib__PPTStream__

#include <stdio.h>

#include <PPToken.h>
#include <PPParser.h>



// VC에서는 타입이 미리 정의되어있지 않으면 인스탄스 베리어블로 선언되어 있어도
// 값이 할당되지 않음.
class PPTDictionary;

///////////////////////////////////////// PPTStream
class PPTStream : public PPToken, public PPParserSource {
public:
    unsigned long _index;
    char *_streamData;
    unsigned long _streamSize;
	string _filterName;
    unsigned long _next;

    bool _decoded;
    bool _decodeFailed;
    PPTDictionary *_dict;
    
    PPTStream(PPParser *parser);
    PPTStream(PPParser *parser, unsigned long length);
    ~PPTStream();
	void SetDictionary(PPTDictionary *dict);
    void appendData(char *data, unsigned long length);
    inline char *getBuffer() { return _streamData;}
    string xmlString(int indent);
    size_t read(unsigned char **ptr, size_t length);
//    void write(unsigned char *buf, size_t length);
    void writeTo(const char *tar_path);
    void flateDecodeStream();
    unsigned long flateEncodeStream(char **strm_dat);
    bool isDecoded();
    bool z_eof();
    inline const char *classType() {return PPTN_STREAM;}
    bool parseObjStm(vector<PPToken *> &token_list, PPParser *parser);

    
    // PPParserSource overriding
    bool canParseString(string str);
    PPToken *parseString(string str, vector <PPToken *> &tokens, PPParser *parser);
    void get(char &ch);
    bool eof();
    size_t tellg();
    void seekg(size_t pos);
    void read(char *buf, size_t size);
    void getline(char *buf, size_t size);

    string pdfString();
    string makePDFString(unsigned long &length);

};


#endif /* defined(__PDFPlusLib__PPTStream__) */

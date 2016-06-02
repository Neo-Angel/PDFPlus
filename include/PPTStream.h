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
protected:
    unsigned long _cur_pos; // 현재 위치

public:
	// stream 에 대한 기본적인 정보(Filter 명등..)를 담고있는 dictionary
    PPTDictionary *_infoDict; 

    char *_streamData;
    unsigned long _streamSize;
	string _filterName; // 인코딩 방식 이름
    unsigned long _next; 

    bool _decoded; // 현재 스트림이 상태가 디코드된 상태인지.
    bool _decodeFailed; //디코딩에 실패했는지...


public:
    PPTStream(PPDocument *doc);
    PPTStream(PPDocument *doc, unsigned long length);
	PPTStream();
    ~PPTStream();

    string PDFString();
    string MakePDFString(unsigned long &length);

	PPBase *Create() {return new PPTStream();}
	void CopyMembersTo(PPBase *obj) ;
    inline const char *ClassType() {return PPTN_STREAM;}

    // PPParserSource overriding
    bool canParseString(string str);
    PPToken *parseString(string str, vector <PPToken *> &tokens, PPParser *parser);
    void get(char &ch);
    bool eof();
    size_t tellg();
    void seekg(size_t pos);
    void read(char *buf, size_t size);
    void getline(char *buf, size_t size);

	/* 자체 함수들... (코멘트에 '//'를 사용하면 문제발생)*/
	void SetDictionary(PPTDictionary *dict);
    void AppendData(char *data, unsigned long length);
    inline char *Buffer() { return _streamData;}
    string XMLString(int indent);
    size_t GetReadPointer(unsigned char **ptr, size_t length);
    void WriteTo(const char *tar_path);
    void FlateDecodeStream();
    unsigned long FlateEncodeStream(char **strm_dat);
    bool IsDecoded();
    bool IsZipEOF();
    bool ParseObjStm(vector<PPToken *> &token_list, PPParser *parser);
};


#endif /* defined(__PDFPlusLib__PPTStream__) */

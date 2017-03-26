//
//  PPToken.cpp
//  PDFPlusLib
//
//  Created by Neo Angel on 2014. 9. 11..
//  Copyright (c) 2014년 #CORP#. All rights reserved.
//

#include <string.h>
#include <sstream>
#include "PPBase.h"
#include "PPToken.h"
#include "PPParser.h"

#include "PPTNumber.h"


// 10진 정수를 Hex 캐릭터로 변환하기위한 테이블
byte hexlist[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

// ch 값을 두개의 파라미티 b1,b2에 나눠 담고, 조합해서 2바이트 크기의 ret에 담아 리턴한다.  
word haxFromByte(byte ch, byte &b1, byte &b2)
{
	word ch1 = hexlist[ (ch & 0xf0) >> 4 ];
	word ch2 = hexlist[ ch & 0x0f ];
	b1 = (byte)ch1;
	b2 = (byte)ch2;
	word ret = (ch1 << 8) | ch2;
	return ret;
}

// 일반 문자열 str의 각 바이트들을 hax로 변환한뒤 문자열 ret에 담아 리턴한다.
string PPToHexStr(string &str)
{
	string ret;
	int i, icnt = (int)str.length();
	char twobytes[3];
	twobytes[2] = NULL;
	for(i=0;i<icnt;i++) {
		byte ch = str.at(i);
		byte b1, b2;
		word wch = haxFromByte(ch, b1, b2);

		twobytes[0] = b1;
		twobytes[1] = b2;
		ret = ret + twobytes;
	}
	return ret;
}

// cnt 만큼의 탭을 문자열 retstr에 담아 리턴한다.
string PPTabStr(int cnt)
{
    string retstr;
    int i;
    for (i=0; i<cnt; i++) {
        retstr += "\t";
    }
    return retstr;
}

// wstring을 UTF8 string으로 변환한다.
void PPwstrToUtf8(const wstring& src, string& dest)
{
    dest.clear();
    for (size_t i = 0; i < src.size(); i++){
        wchar_t w = src[i]; //wchar_t w = src[i];
        if (w == 0) {
            continue;
        }
        else if (w <= 0x7f)
            dest.push_back((char)w);
        else if (w <= 0x7ff){
            dest.push_back(0x00c0 | ((w >> 6)& 0x1f));
            dest.push_back(0x080| (w & 0x3f));
        }
        else if (w <= 0xffff){
            dest.push_back(0x00e0 | ((w >> 12)& 0x0f));
            dest.push_back(0x0080 | ((w >> 6) & 0x3f));
            dest.push_back(0x0080 | (w & 0x3f));
        }
        else if (w <= 0x10ffff){ // wchar_t 4-bytes situation
			unsigned long wl = (unsigned long)w;
            dest.push_back(0xf0 | ((wl >> 18)& 0x07));
            dest.push_back(0x80| ((wl >> 12) & 0x3f));
            dest.push_back(0x80| ((wl >> 6) & 0x3f));
            dest.push_back(0x80| (wl & 0x3f));
        }
        else
            dest.push_back('?');
    }
}

// str을 UTF8 스트링으로 변환한다.
string PPwstrToUtf8(const wstring& str)
{
    string result;
    PPwstrToUtf8(str, result);
    return result;
}

// 일반 string 을 두바이트 wstring 으로 변환함.
void PPstringToWString(string &src_str, wstring &dest_wstr)
{
    size_t i;
    size_t icnt = src_str.size() / 2;
    for (i=0; i<icnt; i++) {
        wchar_t wch = (unsigned char)src_str[i*2];
        wch <<= 8;
        wch = wch | (unsigned char)src_str[i*2+1];
        dest_wstr.push_back(wch);
    }
}
void PPstringToWString2(string &src_str, wstring &dest_wstr)
{
    size_t i;
    size_t icnt = src_str.size();
    for (i=0; i<icnt; i++) {
        wchar_t wch = (unsigned char)src_str[i];
        dest_wstr.push_back(wch);
    }
}


// 일반 string 을 utf8 string으로 변환함.
void PPstringToUTF8String(string &src_str, string &dest_utf8str)
{
    wstring wstr;
    PPstringToWString(src_str, wstr);
    PPwstrToUtf8(wstr, dest_utf8str);
}

#define PP_NOENDIAN			0
#define PP_BIGENDIAN		1
#define PP_LITTLEENDIAN		2


bool PPCompareUnicodeString(string str1, string str2)
{
	uint endian1 = PP_NOENDIAN; // bigendian
	uint endian2 = PP_NOENDIAN;
	uint start1 = 0;
	uint start2 = 0;
	uint length1 = (uint)str1.length();
	uint length2 = (uint)str2.length();

	if(length1 < 2)
		return false;
	if(length2 < 2)
		return false;

	byte ch1_0 = str1[0], ch1_1 = str1[1];
	byte ch2_0 = str2[0], ch2_1 = str2[1];

	if(ch1_0 == 0xfe && ch1_1 == 0xff) {
		endian1 = PP_BIGENDIAN;
		start1 = 2;

	}
	else if(ch1_0 == 0xff && ch1_1 == 0xef) {
		endian1 = PP_LITTLEENDIAN;
		start1 = 2;
	}

	if(ch2_0 == 0xfe && ch2_1 == 0xff) {
		endian2 = PP_BIGENDIAN;
		start2 = 2;
	}
	else if(ch2_0 == 0xff && ch2_1 == 0xef) {
		endian2 = PP_LITTLEENDIAN;
		start2 = 2;
	}

	length1 -= start1;
	length2 -= start2;

	if(length1 != length2)
		return false;

	if(length1 % 2 != 0)
		return false;

	uint icnt = length1 / 2;

	for(uint i = 0;i < icnt;i++) {
		uint idx = i * 2;
		char ch1, ch2;
		///////////////////////////////////////////
		if(endian1 == PP_LITTLEENDIAN) {
			ch1 = str1[idx+start1+1];
		}
		else {
			ch1 = str1[idx+start1];
		}
		if(endian2 == PP_LITTLEENDIAN) {
			ch2 = str2[idx+start2+1];
		}
		else {
			ch2 = str2[idx+start2];
		}
		if(ch1 != ch2)
			return false;
		///////////////////////////////////////////
		if(endian1 == PP_LITTLEENDIAN) {
			ch1 = str1[idx+start1];
		}
		else {
			ch1 = str1[idx+start1+1];
		}
		if(endian2 == PP_LITTLEENDIAN) {
			ch2 = str2[idx+start2];
		}
		else {
			ch2 = str2[idx+start2+1];
		}
		if(ch1 != ch2)
			return false;
	}
	return true;
}


//////////////////////////////////// PPToken
PPToken::PPToken()
{
    _document = NULL;
	_parentObj = NULL;
}

PPToken::PPToken(PPDocument *doc)
{
    _document = doc;
	_parentObj = NULL;
}

void PPToken::Write(std::ostream &os)
{
    _filepos = os.tellp();
        string pdfstr = PDFString();
        if (pdfstr.length() == 0) {
            cout << "Zero PDF string for token " << TypeName() << PP_ENDL;
        }
        os << pdfstr << PP_ENDL;
}

void PPToken::SetDocument(PPDocument *doc)
{
	_document = doc;
}

void PPToken::CopyMembersTo(PPBase *obj)
{
	PPBase::CopyMembersTo(obj);
	PPToken *token = (PPToken *)obj;

	// 외부에 의해서 _document가 다시 지정되기 전 까지는 
	//이전 _document(출처)를 가지고 있는다.
	token->_document = _document; 
	token->_parentObj = _parentObj;
}

void PPToken::MoveInto(PPDocument *doc) 
{
	_document = doc;
} 

PPToken *PPToken::CopyInto(PPDocument *doc)
{
	PPToken *token = (PPToken *)this->Copy();
	token->MoveInto(doc);
	return token;
}


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



byte hexlist[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

word haxFromByte(byte ch, byte &b1, byte &b2)
{
	word ch1 = hexlist[ (ch & 0xf0) >> 4 ];
	word ch2 = hexlist[ ch & 0x0f ];
	b1 = ch1;
	b2 = ch2;
	word ret = (ch1 << 8) | ch2;
	return ret;
}

string toHexStr(string str)
{
	string ret;
	int i, icnt = str.length();
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

void PPwstrToUtf8(string& dest, const wstring& src)
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

string PPwstrToUtf8(const wstring& str)
{
    string result;
    PPwstrToUtf8(result, str);
    return result;
}



void stringToWString(wstring &wstr, string &str)
{
    size_t i;
    size_t icnt = str.size() / 2;
    for (i=0; i<icnt; i++) {
        wchar_t wch = (unsigned char)str[i*2];
        wch <<= 8;
        wch = wch | (unsigned char)str[i*2+1];
        wstr.push_back(wch);
    }
}

void PPstringToUTF8String(string &utf8str, string &str)
{
    wstring wstr;
    stringToWString(wstr, str);
    PPwstrToUtf8(utf8str, wstr);
    
}


//  ////////////////////////////////// PPToken
PPToken::PPToken()
{
    _parser = NULL;

}

PPToken::PPToken(PPParser *parser)
{
    _parser = parser;
}

void PPToken::write(std::ostream &os)
{
    _filepos = os.tellp();
        string pdfstr = pdfString();
        if (pdfstr.length() == 0) {
            cout << "Zero PDF string for token " << typeName() << PP_ENDL;
        }
        os << pdfstr << PP_ENDL;
}

//string PPToken::xmlString(int level)
//{
//    string retstr;
//    return retstr;
//}

//PPToken::~PPToken()
//{
//    
//}

string tabStr(int cnt)
{
    string retstr;
    int i;
    for (i=0; i<cnt; i++) {
        retstr += "\t";
    }
    return retstr;
}


string toNomalASCIIString(string src_str)
{
    string retstr;
    size_t i, icnt = src_str.length();
    for (i=0; i<icnt;i++) {
        unsigned char ch = src_str[i];
        if (ch > 26 && ch < 128) {
            retstr += ch;
        }
        else {
            retstr += '-';
        }
    }
    return retstr;
}

void PPToken::SetParser(PPParser *parser)
{
	_parser = parser;
}
/*
PPToken *PPToken::Copy(PPParser *parser)
{
	PPToken *ret_token = (PPToken *)PPBase::Copy();
	ret_token->SetParser(parser);
	return ret_token;
}
*/

void PPToken::CopyMembersTo(PPBase *obj)
{
	PPBase::CopyMembersTo(obj);
	PPToken *token = (PPToken *)obj;

	// 외부에 의해서 _parser다 다시 지정되기 전 까지는 
	//이전 _parser(출처)를 가지고 있는다.
	token->_parser = _parser; 
}
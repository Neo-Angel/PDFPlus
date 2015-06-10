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

string tapStr(int cnt)
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


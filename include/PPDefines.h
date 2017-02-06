//
//  PPDefines.h
//  PDFPlusLib
//
//  Created by Neo Angel on 2014. 9. 12..
//  Copyright (c) 2014년 #CORP#. All rights reserved.
//

#ifndef __PDFPlusLib_PPDefines_h__
#define __PDFPlusLib_PPDefines_h__

#include <string.h>
#include <vector>

using namespace std;

struct PPData{
    size_t _length;
    char *_data;

};

#ifndef byte
typedef unsigned char  byte;
#endif

#ifndef word
typedef unsigned short  word;
#endif

#ifndef uint
typedef unsigned int  uint;
#endif

#ifndef ulong
typedef unsigned long  ulong;
#endif


// String Constants
static const char	*PP_ENDL = "\x0a";


// #define macros
#define PP_DBG cout << "PP_DBG:"
#define PP_ERR cout << "PP_ERR:"
#define PP_WRN cout << "PP_WRN:"


//
// Utility Fuctions
//
////////////////////////////////////////////////////////////

// PPBase.cpp 에 있는 함수들
// int 값을 string 값으로 변환함
string PPStringFromInt(int n);

// int 값을 * string 값으로 생성(new)함
// PPNew 로 시작되는 함수로부터 받은 객체들은 반드시 delete해 줘야함.
string *PPNewStringFromInt(int n); // needs to delete

// float 값을 string 이나 string *변환함.
string PPStringFromFloat(float n);
string *PPNewStringFromFloat(float n); // needs to delete


// PPToken.cpp에 있는 함수들
// cnt 수 만큼의 '/t'가 있는 문자열을 만들어 줌
string PPTabStr(int cnt);

// str 문자열의 각 바이트들을 2바이트 hex 코드로 변환한 문자열을 만든다.
// (PPToken.cpp)
string PPToHexStr(string &str);

// str 을 ch를 기준으로 분리하여 components에 나눈다. 리턴값은 components.size() 이다.
// (PPParser.cpp)
size_t PPComponentsSepratedByChar(string &str, char ch, vector<string> &components);


// UTF8 Converting
void PPwstrToUtf8(const wstring& src, string& dest);
string PPwstrToUtf8(const wstring& str);
void PPstringToWString(string &src_str, wstring &dest_wstr);
void PPstringToUTF8String(string &src_str, string &dest_utf8str);
bool PPCompareUnicodeString(string str1, string str2);

#endif  // PDFPlusLib_PPDefines_h


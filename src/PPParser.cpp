//
//  PPParser.cpp
//  PDFPlusLib
//
//  Created by Neo Angel on 2014. 9. 14..
//  Copyright (c) 2014년 #CORP#. All rights reserved.
//


#include <map>
#include <string>
#include "PPDocument.h"
#include "PPParser.h"
#include "PPToken.h"
#include "PPTStream.h"
#include "PPTComment.h"
#include "PPTBool.h"
#include "PPTNumber.h"
#include "PPTString.h"
#include "PPTName.h"
#include "PPTArray.h"
#include "PPTDictionary.h"
#include "PPTNull.h"
#include "PPTIndirectObj.h"
#include "PPTXRef.h"
#include "PPTTrailer.h"


const char *delimiters = "()<>[]{}/%";
const char *numberchrs = "0123456789+-.";
const char *decimalchrs = "0123456789";
const char *octalchrs = "01234567";
const char *hexchrs = "0123456789ABCDEF";
const char *stringescs = "nrtbf()\\";

// source의 다음 End Of Line(CR or LF)이 나타랄 때까지 무의미하게 읽어들임
bool ignoreToEOL(PPParserSource &source)
{
    while (!source.eof()) {
        char ch;
        source.get(ch);
        if(ch == 0x0d || ch == 0x0a)  // CR, LF
            return true;
    }
    return false; // eof;
}

typedef enum {
    PPPS_None
    
}PPParserState;

// if 'ch' is visible character?
bool isWhiteSpace(unsigned char ch) 
{
    if (ch <= 32) {
        return true;
    }
    return false;
}

// ch 가 숫자를 표시하는데 쓰이는 캐랙터인지 판단함.
bool isNumberRange(char ch)
{
    if (ch >= '0' && ch <= '9') {
        return true;
    }
    else if(ch == '+' || ch == '-' || ch == '.') {
        return true;
    }
    return false;
}

bool isMiddleNumberRange(char ch)
{
    if (ch >= '0' && ch <= '9') {
        return true;
    }
    else if(ch == '+' || ch == '-' || ch == '.' || ch == 'e') {
        return true;
    }
    return false;
}
// str을 ch를 기준으로 문자열들을 나누어 줌
// 나누어진 문자열들은 components에 담아서 넘겨줌.
size_t PPComponentsSepratedByChar(string &str, char ch, vector<string> &components)
{
    size_t start = 0;
    size_t idx = str.find(ch);
    while (idx != string::npos) {
        string substr = str.substr(start, idx - start);
        components.push_back(substr);
        start = idx+1;
        idx = str.find(ch, start);
    }
    size_t end = str.size();
    string substr = str.substr(start, end - start);
    components.push_back(substr);
    
    return components.size();
}


//////////////////////////////////////////////////////////////////////////////
// PPParser 클래스의 메소드들


// 각 타입별 파싱 메소드
//////////////////////////////////////////////////////////////////////////////

// Parsing Comment
//////////////////////////////////////////////////////////////////////////////
PPTComment *PPParser::parseComment(PPParserSource &source)
{
    string *strbuf = new string;
    while (!source.eof()) {
        char ch;
        source.get(ch);
		if(ch == 0x0d || ch == 0x0a)  // CR, LF
            break;;
        *strbuf += ch;
    }
    PPTComment *ret = new PPTComment(_document, strbuf);
    return ret;
}

// Parsing String
//////////////////////////////////////////////////////////////////////////////

// ascii로 표현되는 hex_ch를 숫자 값으로 변환함. 
char hex2num(char hex_ch)
{
    if (hex_ch >= '0' && hex_ch <= '9') {
        return (hex_ch - '0');
    }
    else if (hex_ch >= 'A' && hex_ch <= 'F') {
        return (hex_ch - 'A' + 10);
    }
    else if (hex_ch >= 'a' && hex_ch <= 'f') {
        return (hex_ch - 'a' + 10);
    }
    return 0;
}

// 두개의 개별 ascii hex 값을 조합해 하나의 문자 ASCII 값으로 변환함.
char cvtHex2Ascii(char ch1, char ch2)
{
    char ret_ch = hex2num(ch1) * 16 + hex2num(ch2);
    return ret_ch;
}

// 8진수 asscii 코드값을 한 개의 문자 코드값으로 변환함
unsigned char cvtOct2Ascii(string octstr)
{
    unsigned char ret = 0;
    int icnt = (int)octstr.length();
    int idx = icnt -1;
    ret = octstr[idx] - '0';
    idx --;
    if (idx >= 0) {
        ret += (octstr[idx] - '0') * 8;
    }
    idx --;
    if (idx >= 0) {
        ret += (octstr[idx] - '0') * 8 * 8;
    }
    return ret;
}

// 일반적인 스타일의 문자열을 파싱함.
PPTString *PPParser::parseStringObj(PPParserSource &source)
{
    string *strbuf = new string;
    char prev_ch = NULL;
    char ch = NULL;
    int parenthese = 0;
    source.get(ch);
    while(!source.eof() && !(parenthese == 0 && ch == ')')){
        if (ch == '(') {
            parenthese++;
        }
        else if(ch == ')') {
            parenthese--;
        }
        else if(ch == '\\') {
            source.get(ch);
            if (ch == 'n') {
                ch = '\xa';
            }
            else if(ch == 'r') {
                ch = '\r';
            }
            else if(ch == 'b') {
                ch = '\b';
            }
            else if(ch == 't') {
                ch = '\t';
            }
            else if(ch == 'f') {
                ch = '\f';
            }
            else if(ch == '(') {
            }
            else if(ch == ')') {
            }
            else if(strchr(octalchrs, ch) != NULL) {
                string oct_buf;
                oct_buf += ch;
                source.get(ch);
                if(strchr(octalchrs, ch) != NULL) {
                    oct_buf += ch;
                    source.get(ch);
                    if(strchr(octalchrs, ch) != NULL) {
                        oct_buf += ch;
                        ch = (char)cvtOct2Ascii(oct_buf);
                    }
                    else {
                        *strbuf += '\\';
                        *strbuf += oct_buf;
                    }
                }
                else {
                    *strbuf += '\\';
                    *strbuf += oct_buf;
                }
            }
        }
        *strbuf += ch;
        prev_ch = ch;
        source.get(ch);
    }
    PPTString *ret = new PPTString(_document, strbuf);
    return ret;
}

// parse string that consist of hex.
PPTString *PPParser::parseHexString(PPParserSource &source)
{
    char ch1;
    char ch2;
    char ch = 0;

    string *strbuf = new string;
    source.get(ch1);
    source.get(ch2);
    if (ch1 == '>' || ch2 == '>') {
        return NULL;
    }
    while(!source.eof()){
        ch = cvtHex2Ascii(ch1, ch2);
        *strbuf += ch;
        source.get(ch1);
        if (ch1 == '>') {
            break;
        }
        source.get(ch2);
        if (ch2 == '>') {
            break;
        }
    }
    PPTString *ret = new PPTString(_document, strbuf);
    return ret;
}

// Parsing Number
//////////////////////////////////////////////////////////////////////////////
PPTNumber *PPParser::parseNumber(PPParserSource &source, char start_ch)
{
    char ch = start_ch;
    string *strbuf = new string;
    *strbuf += ch;
    source.get(ch);
    while(!source.eof() && isMiddleNumberRange(ch)){
        *strbuf += ch;
        source.get(ch);
    }
    if (!isMiddleNumberRange(ch)) { //(ch == '/' || ch == ']'|| ch == '>') {
        size_t filept = source.tellg();
        source.seekg(filept-1);
    }
    PPTNumber *ret = new PPTNumber(_document, strbuf);
    return ret;
    
}


// Parsing Name
//////////////////////////////////////////////////////////////////////////////
PPTName *PPParser::parseName(PPParserSource &source)
{
    char ch = '\0';
    string *strbuf = new string;
    source.get(ch);
    while(!source.eof() && !isWhiteSpace(ch) && strchr(delimiters, ch) == NULL){
        if (ch == '#') {
            char hexch1 = 0;
            source.get(ch);
            if (strchr(hexchrs, ch)) {
                char hexch2 = 0;
                hexch1 = ch;
                source.get(ch);
                if (strchr(hexchrs, ch)) {
                    hexch2 = ch;
                    ch = cvtHex2Ascii(hexch1, hexch2);
                }
                else {
                    *strbuf += '#';
                    *strbuf += hexch1;
                }
            }
            else {
                *strbuf += '#';
            }
        }
        *strbuf += ch;
        source.get(ch);
    }
    if (strchr(delimiters, ch) != NULL) {
        size_t filept = source.tellg();
        source.seekg(filept-1);
    }
    PPTName *ret = new PPTName(_document, strbuf);
    return ret;
    
}
/*
// Parsing Bool
//////////////////////////////////////////////////////////////////////////////
*/
PPTBool *PPParser::parseBool(PPParserSource &source, char start_ch)
{
    char ch = start_ch;
    string strbuf = "";
    strbuf += ch;
    source.get(ch);
    while(!source.eof() && !isWhiteSpace(ch) && strchr(delimiters, ch) == NULL){
        strbuf += ch;
        source.get(ch);
   }
    if (strbuf.compare("false") != 0 || strbuf.compare("true") != 0)  {
        return NULL;
    }
    if (strchr(delimiters, ch) != NULL) {
        size_t filept = source.tellg();
        source.seekg(filept-1);
    }
    
    PPTBool *ret = new PPTBool(_document, strbuf);
    return ret;
    
}

// Parsing Array
//////////////////////////////////////////////////////////////////////////////
PPTArray *PPParser::parseArray(PPParserSource &source)
{
    vector<PPToken *> token_list;
    if(!ParseSource(source, token_list))  // 메인 함수인 ParseSource()를 재귀호출함. 
        return NULL;
    PPTArray *ret = new PPTArray(_document, token_list); // Array용 토큰 객체로 재 생성함.
    return ret;
    
}

// Parsing Dictionary
//////////////////////////////////////////////////////////////////////////////
PPTDictionary *PPParser::parseDictionary(PPParserSource &source)
{
	// Dictionary는 Array와 구성이 같음. 단지,
	// 짝수번째 객체는 Key 값이고,
	// 홀수번째 객체는 Value 임
	// Key는 Name 토큰으로 되어있음.
	// 그래서 우선 배열을 파싱한 다음 그 배열을 기반으로
	// Dictionary를 구성함.
    vector<PPToken *> token_list;
    if(!ParseSource(source, token_list))  // recursive function call
        return NULL;
    int i, icnt = (int)token_list.size();
    if (icnt % 2 != 0) {
        return NULL;
    }
    
	// Build dictionary from array 'token_list'.
    PPTDictionary *ret_dict = new PPTDictionary(_document);
    icnt = icnt / 2;
    for (i=0; i<icnt; i++) {
        PPTName *name = (PPTName *)token_list.at(i*2);
        if (dynamic_cast<PPTName *>(name) == NULL) {
            delete ret_dict;
            return NULL;
        }
        PPToken *token = token_list.at(i*2+1);
        ret_dict->SetTokenAndKey(token, *name->_name);
        delete name;
    }
    return ret_dict;  
}

// Parsing Stream
//////////////////////////////////////////////////////////////////////////////
void skipToCRLF(PPParserSource &source)
{
    char ch;
    source.get(ch);
    while (ch!=0x0d && ch == 0x0a) {
        source.get(ch);
    }
    while (ch == 0x0d || ch == 0x0a) {
        source.get(ch);
    }
    size_t filept = source.tellg();
    source.seekg(filept-1);
}

// 굳이 멤버함수로 하지 않아도 되는 것들은 그냥 일반 C함수로 정의한다.
// 멤버 함수에 비해 다루기가 쉽다.
bool findWord(PPParserSource &source, const char *word)
{
    bool ret = false;
    char ch = NULL;
    
    while (!source.eof()) {
        while (ch != word[0] && !source.eof()) {
            source.get(ch);
        }
        
        size_t wordlen = strlen(word);
        int i = 0;
        while (ch == word[i] && !source.eof()) {
            i++;
            if(i == wordlen) {
                return true;
            }
            source.get(ch);
        }
    }
    return ret;
}

PPTStream *PPParser::parseStream(PPParserSource &source, unsigned long length)
{
    PPTStream *ret = new PPTStream(_document, length);
    skipToCRLF(source);
    if (length > 0) {
        source.read(ret->Buffer(), length);
    }
    
    bool res = findWord(source, "endstream");
    if (res == false) {
        delete ret;
        return NULL;
    }
    return ret;
}

#define PP_STREAM_BUF_SIZE      1024

PPTStream *PPParser::parseStream(PPParserSource &source)
{
    skipToCRLF(source);
    
    vector<char *> buflist;
    char *buf = new char[PP_STREAM_BUF_SIZE];
    unsigned long bytes_len = 0;
    unsigned int bufidx = 0;
    char ch;
    source.get(ch);
    while (true) {
        buf[bufidx] = ch;
        bufidx ++;
        bytes_len ++;
        if(bufidx == PP_STREAM_BUF_SIZE) {
            buflist.push_back(buf);
            buf = new char[PP_STREAM_BUF_SIZE];
            bufidx = 0;
        }
        
        if (ch == 0x0d || ch == 0x0a) {
            char strline[10];
            if (bufidx >= 10) {
                strncpy(strline, buf + bufidx - 10, 9);
            }
            else if(buflist.size() > 0) {
                size_t shortage = 10 - bufidx;
                char *prevbuf = buflist.at(buflist.size()-1);
                size_t prevbufidx = PP_STREAM_BUF_SIZE - shortage;
                strncpy(strline, prevbuf+prevbufidx, shortage);
                strncpy(strline + shortage, buf, bufidx);
            }
            if (strncmp(strline, "endstream", 9) == 0) {
                bytes_len -= 10;
                if (bufidx <= 10) {
                    bufidx = 0;
                }
                break;
            }
        }
        source.get(ch);
        if (source.eof()) {
            // error
            return NULL;
        }
    }
    if (bufidx > 0) {
        buflist.push_back(buf);
    }
    else {
        delete buf;
    }
    
    PPTStream *ret_stream = new PPTStream(_document, bytes_len);
    bufidx = 0;
    int i, icnt = (int)buflist.size();
    for (i=0; i<icnt; i++) {
        char *buf = buflist.at(i);
        unsigned int copy_size = PP_STREAM_BUF_SIZE;
        if(bytes_len < bufidx + copy_size) {
            copy_size = (int)bytes_len - bufidx;
        }
        memcpy(ret_stream->Buffer()+bufidx, buf,copy_size);
        bufidx += PP_STREAM_BUF_SIZE;
        delete buf;
    }
    return ret_stream;
}


// Parsing IndirectObj
//////////////////////////////////////////////////////////////////////////////
PPTIndirectObj *PPParser::parseIndirectObj(PPParserSource &source, PPTNumber *num1, PPTNumber *num2)
{
    PPTIndirectObj *ret_obj = new PPTIndirectObj(_document, num1->intValue(), num2->intValue());

	if(!ParseSource(source, ret_obj->_array, ret_obj)) {
		delete ret_obj;
        return NULL;
	}
    return ret_obj;
}


// Parsing XRef
//////////////////////////////////////////////////////////////////////////////
bool parseXRef(PPParserSource &source, PPTXRef *xref, int objnum, int count)
{
    skipToCRLF(source);
    char numstr[30];
    int i;
    for (i=0; i<count; i++) {
        source.getline(numstr, 30);
        vector<string> str_list;
        string linestr = numstr;
        PPComponentsSepratedByChar(linestr, ' ', str_list);
        if (str_list.size() != 3) {
            return false;
        }
        const char *offsetcstr = str_list[0].c_str(); //  .at(0).c_str;
        unsigned long long offset = stoull(offsetcstr);
        
        const char *gencstr = str_list[1].c_str();
        int gennum = stoi(gencstr);
        
        const char *typecstr = str_list[2].c_str();
        char typec = typecstr[0];
        
        xref->AddXRef(objnum+i, offset, gennum, typec);        
    }
    return true;
}

// 메인함수(ParseSource()) 위한 보조 함수들
//
//////////////////////////////////////////////////////////////////////////////

bool isAlphabet(char ch)
{
    if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '*') {
        return true;
    }
    return false;
}

bool isLastCharAlphabet(string str)
{
    size_t slen = str.length();
    if (slen < 2) {
        return false;
    }
    char ch1 = str[slen-2];
    char ch2 = str[slen-1];
    if (isAlphabet(ch1) && !isAlphabet(ch2)) {
        return true;
    }
    return false;
}

bool isKindOfNumber(PPToken *token)
{
    if (token->ClassType() == PPTN_NUMBER) {
        return true;
    }
    return false;
}

bool isKindOfXRefIndirectObjType(PPToken *token)
{
    if (token->ClassType() != PPTN_INDIRECTOBJ) {
        return false;
    }
    PPTIndirectObj *indir = (PPTIndirectObj *)token;
    PPTDictionary *dict = indir->FirstDictionary();
	if(dict == NULL) {
		return false;
	}
    PPTName *type = (PPTName *)dict->ValueObjectForKey("Type");
    if (type != NULL && *type->_name == "XRef") {
        return true;
    }
    return false;
}

bool isKindOfTrailer(PPToken *token)
{
    if (token->ClassType() == PPTN_TRAILER) {
        return true;
    }
    return false;

}


bool canSourceParseString(PPParserSource &source, string curstr)
{
    string tstr = curstr.substr(0, curstr.length()-1);
    if (source.canParseString(tstr)) {
        return true;
    }
    return false;
}

//  Main Parsing Function
//
//////////////////////////////////////////////////////////////////////////////////
bool PPParser::ParseSource(PPParserSource &source, vector<PPToken *> &token_list, PPToken *parent_token)
{
    PPTXRef *XRef = NULL;
    size_t filept = 0;
    unsigned long long wordoffset = 0;
    string curstr = "";
    char prev_ch = '\0';
    char ch = '\0';
    while(!source.eof()){
        PPToken *token_obj = NULL;
        if (curstr.length() == 0) {
            wordoffset = filept;
        }

        source.get(ch);
        filept = source.tellg();
        if (filept < 0) {
            break;
        }
        curstr += ch;
        
        /* canSourceParseString 함수로 source에서 파싱할 게 있는 지 먼저 확인한 후 */
        if (curstr.length() >= 2 && isLastCharAlphabet(curstr) && canSourceParseString(source, curstr)) {
            string tstr = curstr.substr(0, curstr.length()-1);
			/* ParserSource 에서 파싱할 것들은 먼저 파싱함. */
            token_obj = source.parseString(tstr, token_list, this);
            if(!token_obj) {
                return false;
            }
            ch = prev_ch;
            source.seekg(filept-1);
        }
        else if(ch == '>' && prev_ch == '>')  {  // end of dictionary
            break;
        }
        else if (ch == ']') { //  end of array
            break;
        }
        else if (curstr.length() == 6 && curstr == "endobj") { //  end of indirect obj
            break;
        }
        else if(ch == '%') { // comment
            token_obj = (PPToken *)parseComment(source);
            if(token_obj) {
                token_list.push_back(token_obj);
            }
            else
                return false;
            
            continue;
        }
        else if((curstr.length() == 4 && curstr == "true")
                || (curstr.length() == 5 && curstr == "false")) { 
            token_obj = new PPTBool(_document, curstr);
            if(token_obj)
                token_list.push_back(token_obj);
            else
                return false;
        }
        else if(isNumberRange(ch)) {
            token_obj = (PPToken *)parseNumber(source, ch);
            if(token_obj) {
                token_obj->_filepos = wordoffset;
                if (XRef != NULL) {
                    int cnt = (int)token_list.size();
                    PPToken *token0 = token_list[cnt-1];
                    
                    if(isKindOfNumber((PPTNumber *)token0)) {
                        PPTNumber *num1 = (PPTNumber *)token0;
                        PPTNumber *num2 = (PPTNumber *)token_obj;
                        token_list.pop_back();

                        parseXRef(source, XRef, num1->intValue(), num2->intValue());
                    }
                    else {
                        token_list.push_back(token_obj);
                    }
                }
                else {
                    token_list.push_back(token_obj);
                }
            }
            else
                return false;
        }
        else if(ch == '(') { // string
            // parse string
            token_obj = (PPToken *)parseStringObj(source);
            if(token_obj)
                token_list.push_back(token_obj);
            else
                return false;
        }
        else if(ch == '<') {
            source.get(ch);
            if(ch == '<') { // dictionary '<<'
                token_obj = (PPToken *)parseDictionary(source);
                if(token_obj)
                    token_list.push_back(token_obj);
                else {
                    filept = source.tellg();
                    return false;
                }
            }
            else { // hex string
                source.seekg(filept);
                token_obj = (PPToken *)parseHexString(source);
                if(token_obj)
                    token_list.push_back(token_obj);
                else
                    return false;
            }
        }
        else if(ch == '/') {  // name
            token_obj = (PPToken *)parseName(source);
            if (token_obj) {
                token_list.push_back(token_obj);
            }
            else
                return false;
        }
        else if(ch == '[') { // array
            token_obj = (PPToken *)parseArray(source);
            if (token_obj) {
                token_list.push_back(token_obj);
            }
            else
                return false;
        }
        else if(curstr.length() == 4 && curstr == "null") {
            token_obj = new PPTNull();
            token_list.push_back(token_obj);
            
        }
        else if(curstr.length() == 6 && curstr == "stream") {
            int cnt = (int)token_list.size();
            PPTDictionary *dict = (PPTDictionary *)token_list[cnt-1];
            PPToken *val_obj = (PPToken *)dict->ValueObjectForKey("Length");
            if (val_obj) {
                PPTNumber *len_obj = (PPTNumber *)val_obj;
                long length = len_obj->longValue();
                token_obj = (PPToken *)parseStream(source, length);
                if (token_obj) {
                    PPTStream *stream = (PPTStream *)token_obj;
                    stream->_infoDict = dict;
                    PPTName *filter = (PPTName *)dict->NameForKey("Filter");
                    bool obj_stream_parsed = false;
                    if (filter != NULL && *filter->_name == "FlateDecode") {
                        stream->FlateDecodeStream();
                        PPTName *type = (PPTName *)dict->ObjectForKey("Type");
                        if (type != NULL && *type->_name == "ObjStm") {
							if(parent_token != NULL && parent_token->ClassType() == PPTN_INDIRECTOBJ) {
								stream->_parentObj = (PPTIndirectObj *)parent_token;
							}
                            if(stream->ParseObjStm(token_list, this) == false) {
                                return false;
                            }
                            obj_stream_parsed = true;
                        }
                    }
                    if (!obj_stream_parsed) {
                        token_list.push_back(token_obj);
                    }
                }
                else
                    return false;
            }
            else {
                token_obj = (PPToken *)parseStream(source);
                PPTStream *stream = (PPTStream *)token_obj;
                stream->_infoDict = dict;
                token_list.push_back(token_obj);
				_document->_stream_list.push_back(token_obj);
            }
        }
        else if(curstr.length() == 3 && curstr == "obj") {  //  indirect object
            int cnt = (int)token_list.size();
            PPToken *token1 = token_list[cnt-2];
            PPToken *token2 = token_list[cnt-1];
//            if (token1->isKindOfClass<PPTNumber *>() && token2->isKindOfClass<PPTNumber *>()) {
            if (isKindOfNumber((PPTNumber *)token1) && isKindOfNumber((PPTNumber *)token2)) {
                PPTNumber *num1 = (PPTNumber *)token1;
                PPTNumber *num2 = (PPTNumber *)token2;
                token_obj = (PPToken *)parseIndirectObj(source, num1, num2);
                if(token_obj) {
                    token_obj->_filepos = num1->_filepos;
                    _document->_filePtDict[token_obj->_filepos] = token_obj;
                    token_list.erase(token_list.begin() + (cnt-1));
                    token_list.erase(token_list.begin() + (cnt-2));
					PPTIndirectObj *ind_obj = (PPTIndirectObj *)token_obj;
					if(ind_obj->IsObjStream() == false) {
						token_list.push_back(token_obj);
					}
                    _document->_objDict[num1->intValue()] = (PPTIndirectObj *)token_obj;
                    if (_document->_objNumber < (unsigned)num1->intValue()) {
                        _document->_objNumber = num1->intValue();
                    }
                }
                else {
                    return false;
                }
            }
        }
        else if(curstr.length() == 7 && curstr == "trailer") {  //  indirect object
            skipToCRLF(source);
            char ch1, ch2;
            source.get(ch1);
            source.get(ch2);
            if(ch1 == '<' && ch2 == '<') { // dictionary
                PPTDictionary *dict_obj = parseDictionary(source);
                if(dict_obj) {
                    PPTTrailer *trailer_obj = new PPTTrailer(_document, dict_obj, (unsigned long long)0);
                    token_obj = (PPToken *)trailer_obj;
                    
                    PPTIndirectObj *indirect = (PPTIndirectObj *)token_list.at(token_list.size() - 1);
                    if (isKindOfXRefIndirectObjType(indirect)) { // check 'XRef' type. Adobe Style
                        int cnt = (int)token_list.size();
                        token_list.erase(token_list.begin() + (cnt-1));
                        trailer_obj->_xrefIndirect = indirect;
                        _document->_filePtDict[indirect->_filepos] = trailer_obj;
                    }
                    token_list.push_back(token_obj);
                }
                else {
                    filept = source.tellg();
                    return false;
                }
            }
            else {
                filept = source.tellg();
                return false;
            }

        }
        else if(curstr.length() == 9 && curstr == "startxref") {  //  indirect object
            skipToCRLF(source);
            PPTTrailer *last_token = (PPTTrailer *)token_list.at(token_list.size() - 1);
            if (last_token->ClassType() != PPTN_TRAILER) {
                if (isKindOfXRefIndirectObjType(last_token)) { // check 'XRef' type. Adobe Style  or 40.pdf
                    PPTIndirectObj *indirect = (PPTIndirectObj *)last_token;
                    int cnt = (int)token_list.size();
                    token_list.erase(token_list.begin() + (cnt-1));
                    last_token = new PPTTrailer(_document, indirect, 0);
                    _document->_filePtDict[indirect->_filepos] = last_token;
                    token_list.push_back(last_token);
                }
                else {
                    last_token = new PPTTrailer(_document, 0);
                    token_list.push_back(last_token);
                }
            }
            char numstr[20];
            source.getline(numstr, 20);
            unsigned long long xref_loc = stoull(numstr);
            last_token->_startxref = xref_loc;
            source.getline(numstr, 20);
        }
        else if(curstr.length() == 4 && curstr == "xref") {  //
            XRef = new PPTXRef(_document);
            XRef->_filepos = wordoffset;
            _document->_filePtDict[wordoffset] = XRef;
        }
        if (XRef != NULL && token_obj != NULL && !isKindOfNumber((PPTNumber *)token_obj)) {
            if (token_obj->ClassType() == PPTN_TRAILER) {
                PPTTrailer *trailer_obj = (PPTTrailer *)token_obj;
                trailer_obj->_xrefObj = XRef;
                XRef->_trailerObj = trailer_obj;
            }
            
            token_list.pop_back();
            token_list.push_back(XRef);
            token_list.push_back(token_obj);
            XRef = NULL;
        }
        
        if (token_obj || isWhiteSpace(ch)) {
            curstr = "";
        }
        prev_ch = ch;
    }

    return true;
}


PPParser::~PPParser()
{
}


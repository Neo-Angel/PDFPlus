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
#include "PPTStream.h"

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



// PPTName //////////////////////////////////
PPTName::PPTName(PPParser *parser, string *str):PPToken(parser)
{
    _name = str;
}
PPTName::~PPTName()
{
    delete _name;
}
//string *PPTName::getName()
//{
//    return _name;
//}

string PPTName::description()
{
    return *_name;
}

string PPTName::xmlString(int level)
{
    string retstr;
    retstr += tapStr(level) + "<Name>" +*_name + "</Name>\xa";
    return retstr;
}


string PPTName::pdfString()
{
    string retstr = "/";
    retstr += *_name;
    return retstr;
}

// PPTArray //////////////////////////////////

PPTArray::PPTArray(PPParser *parser, vector<PPToken *> token_list) : PPToken(parser)
{
    _array = token_list;
}

PPTArray::~PPTArray()
{
    int i, icnt = (int)_array.size();
    for (i=0; i<icnt; i++) {
        PPToken *token = _array.at(i);
        delete token;
    }
}

string PPTArray::xmlString(int level)
{
    string retstr;
    retstr += tapStr(level) + "<Array>\xa";
    int i, icnt = (int)_array.size();
    for (i=0; i<icnt; i++) {
        PPToken *token = _array.at(i);
        retstr += token->xmlString(level+1);
    }
    retstr += tapStr(level) + "</Array>\xa";

    return retstr;
}

string PPTArray::internalXmlString(int level)
{
    string retstr;
    retstr += tapStr(level) + "<Array>\xa";
    int i, icnt = (int)_array.size();
    for (i=0; i<icnt; i++) {
        PPToken *token = _array.at(i);
        retstr += token->internalXmlString(level+1);
    }
    retstr += tapStr(level) + "</Array>\xa";
    
    return retstr;
}

string PPTArray::pdfString()
{
    string retstr = "[";
    int i, icnt = (int)_array.size();
    for (i=0; i<icnt; i++) {
        PPToken *token = _array.at(i);
        if (i > 0) {
            retstr += " ";
        }
        retstr += token->pdfString();
    }
    retstr += "]";
    return retstr;
}


void PPTArray::AddToken(int num) 
{
	AddToken((PPToken *)new PPTNumber(_parser, num));
}

// PPTDictionary //////////////////////////////////

PPTDictionary::PPTDictionary(PPParser *parser) : PPToken(parser)
{
    
}

PPTDictionary::~PPTDictionary()
{
    map <string, PPToken *>::iterator it_dict;
	for(it_dict = _dict.begin(); it_dict != _dict.end(); it_dict++) {
        PPToken *token = it_dict->second;
        delete token;
    }
    _dict.clear();
}

void PPTDictionary::setTokenAndKey(PPToken *token, string key)
{
    _dict[key] = token;
}

void PPTDictionary::SetTokenAndKey(string name, string key)
{
	string *name_ptr = new string(name);
	PPTName *name_obj = new PPTName(_parser, name_ptr);
	setTokenAndKey(name_obj, key);
}

PPTIndirectObj *PPTDictionary::SetRefTokenAndKey(PPToken *token, string key, int obj_num)
{
	PPTIndirectRef *ref = new PPTIndirectRef(_parser, obj_num, 0);
	SetTokenAndKey(token, key);
	PPTIndirectObj *obj = new PPTIndirectObj(_parser, obj_num, 0);
	obj->_array.push_back(token);
	return obj;
}

PPToken *PPTDictionary::objectForKey(string &keyname)
{
    PPToken *ret_token = _dict[keyname];
    if (ret_token == NULL) {
        _dict.erase(keyname);
    }
    return ret_token;
}

PPToken *PPTDictionary::objectForKey(const char *keyname)
{
    PPToken *ret_token = _dict[keyname];
    if (ret_token == NULL) {
        _dict.erase(keyname);
    }
    return ret_token;
}

PPTName *PPTDictionary::nameForKey(const char *keyname)
{
    PPTName *ret_name = (PPTName *)_dict[keyname];
    if (ret_name == NULL) {
        _dict.erase(keyname);
		return NULL;
    }

    if (ret_name->typeName() == PPTN_ARRAY) {
		PPTArray *arr = (PPTArray *)ret_name;
		if(arr->size() == 0) {
			return NULL;
		}
		ret_name = (PPTName *) arr->objectAtIndex(0);
    }
    return ret_name;
}

PPToken *PPTDictionary::indirectObjectForKey(string &keyname)
{
    PPToken *ret = _dict[keyname];
    if (ret) {
        if (ret->classType() == PPTN_INDIRECTREF) {
            PPTIndirectRef *indref = (PPTIndirectRef *)ret;
            PPTIndirectObj *tar = indref->targetObject();
            return tar;
        }
    }
    else {
        _dict.erase(keyname);
    }
    return ret;
}

PPToken *PPTDictionary::valueObjectForKey(string &keyname)
{
    PPToken *ret = _dict[keyname];
    if (ret != NULL && ret->classType() == PPTN_INDIRECTREF) {
        PPTIndirectRef *indref = (PPTIndirectRef *)ret;
        PPTIndirectObj *tar = indref->targetObject();
        if (tar) {
            return tar->_array.size() > 0 ? tar->_array[0] : NULL;
        }
        return NULL;
    }
    else if (ret == NULL){
        _dict.erase(keyname);
    }
    return ret;
}


string PPTDictionary::description()
{
    string str;
    str = "Dict {\xa";
    
    map <string, PPToken *> ::iterator it_token_objs;
    for(it_token_objs = _dict.begin(); it_token_objs != _dict.end(); it_token_objs++) {
        string name = it_token_objs->first;
        PPToken *token = (PPToken *)(it_token_objs->second);
        string desc = token->description();
        str += "\t["+name + "] : [" + desc+"]\xa";
    }
    str += "}\xa";
    return str;
}

string PPTDictionary::internalXmlString(int level)
{
    string retstr;
    retstr += tapStr(level) + "<Dict>\xa";
    map <string, PPToken *> ::iterator it_token_objs;
    for(it_token_objs = _dict.begin(); it_token_objs != _dict.end(); it_token_objs++) {
        string name = it_token_objs->first;
        retstr += tapStr(level+1) + "<Key>" + name + "</Key>\xa";
        
        PPToken *token = valueObjectForKey(name);
        retstr += token->internalXmlString(level+1);
    }
    retstr += tapStr(level) + "</Dict>\xa";
    
    return retstr;
}

string PPTDictionary::xmlString(int level)
{
    string retstr;
    retstr += tapStr(level) + "<Dict>\xa";
    map <string, PPToken *> ::iterator it_token_objs;
    for(it_token_objs = _dict.begin(); it_token_objs != _dict.end(); it_token_objs++) {
        string name = it_token_objs->first;
        retstr += tapStr(level+1) + "<Key>" + name + "</Key>\xa";
        
        PPToken *token = (PPToken *)(it_token_objs->second);
        retstr += token->xmlString(level+1);
    }
    retstr += tapStr(level) + "</Dict>\xa";
    
    return retstr;
}

string PPTDictionary::pdfString()
{
    string retstr = "<<";
    map <string, PPToken *> ::iterator it_token_objs;
    for(it_token_objs = _dict.begin(); it_token_objs != _dict.end(); it_token_objs++) {
        retstr += "/";
        string name = it_token_objs->first;
        retstr += name;
        retstr += " ";
        PPToken *token = (PPToken *)(it_token_objs->second);
        retstr += token->pdfString();
        retstr += "\xa";
    }
    retstr += ">>\xa";
    return retstr;
}

// PPStreamBuf ////////////////////////////////// This is not a PPToken's subclass
#include "PPDefines.h"
PPStreamBuf::PPStreamBuf()
{
    _length = 0;
}

PPStreamBuf::~PPStreamBuf()
{
    int i, icnt = (int)_streamBuf.size();
    for (i=0; i<icnt; i++) {
        PPData *data = _streamBuf.at(i);
        delete[] data->_data;
        delete data;
    }
}

void PPStreamBuf::appendBuf(char *buf, size_t len)
{
    PPData *data = new PPData();
    data->_data = new char[len]; //buf;
    data->_length = len;
    memcpy(data->_data, buf, len);
    _streamBuf.push_back(data);
    _length += len;
}

size_t PPStreamBuf::totalLength()
{
    return _length;
}

void PPStreamBuf::collect(char *buf)
{
    int i, icnt = (int)_streamBuf.size();
    size_t pos = 0;
    for (i=0; i<icnt; i++) {
        PPData *data = _streamBuf.at(i);
        memcpy(buf + pos, data->_data, data->_length);
        pos += data->_length;
    }
}


//  ////////////////////////////////// PPTNull
string PPTNull::xmlString(int level)
{
    string retstr = tapStr(level) + "<Null/>";
    return retstr;
}

string PPTNull::pdfString() {
    return "null";
}

//  ////////////////////////////////// PPTIndirectObj
PPTIndirectObj::PPTIndirectObj(PPParser *parser, vector<PPToken *> token_list, int num1, int num2):PPToken(parser)
{
    _objNum = num1;
    _genNum = num2;
    _array = token_list;
}

PPTIndirectObj::PPTIndirectObj(PPParser *parser, int num1, int num2):PPToken(parser)
{
	_objNum = num1;
	_genNum = num2;
}


PPTIndirectObj::~PPTIndirectObj()
{
    int i, icnt = (int)_array.size();
    for (i=0; i<icnt; i++) {
        PPToken *token = _array.at(i);
        delete token;
    }
}

string PPTIndirectObj::description()
{
    string retstr;
    int i, icnt = (int)_array.size();
    for (i=0; i<icnt; i++) {
        PPToken *token = _array.at(i);
        retstr += token->description() + '\xa';
    }
    
    return retstr;
}


string PPTIndirectObj::xmlString(int level)
{
    ostringstream ostr;
    ostr <<tapStr(level)<<"<Object ID='"<<_objNum<<"' Gen='"<<_genNum<< "' Pos='"<<_filepos<<"'>\xa";
    string retstr = ostr.str();
//    retstr += tapStr(level) + "<Indirect Object=" + _objNum + ", Generation="+ _genNum+ ">\xa";
    size_t icnt = _array.size();
    size_t i;
    icnt = _array.size();
    for (i=0; i<icnt; i++) {
        PPToken *token = _array.at(i);
        retstr += token->xmlString(level+1);
//        delete token;
    }
    retstr += tapStr(level) + "</Object>\xa";
    
    return retstr;
}
PPTDictionary *PPTIndirectObj::firstDictionary()
{
    if (_array.size() == 0) {
        return NULL;
    }
    PPToken *token = _array.at(0);
    if (token->classType() != PPTN_DICTIONARY) {
        return NULL;
    }
    return (PPTDictionary *)token;
}

PPTStream *PPTIndirectObj::stream()
{
    if (_array.size() != 2) {
        return NULL;
    }
    PPToken *token = _array.at(1);
    if (token->classType() != PPTN_STREAM) {
        return NULL;
    }
    return (PPTStream *)token;
}

bool PPTIndirectObj::isObjStream()
{
    PPTDictionary *dict = (PPTDictionary *)_array[0];
    if (dict->classType() != PPTN_DICTIONARY)
        return false;
    
    PPTName *type = (PPTName *)dict->objectForKey("Type");
    if (type != NULL && *type->_name == "ObjStm") {
        return true;
    }
    return false;
}

bool PPTIndirectObj::isStream()
{
    PPTDictionary *dict = (PPTDictionary *)_array[0];
    if (dict->classType() != PPTN_DICTIONARY)
        return false;
    
    PPTName *filter = (PPTName *)dict->objectForKey("Filter");
    if (filter != NULL && *filter->_name == "FlateDecode") {
        return true;
    }
    return false;
}

void PPTIndirectObj::write(ostream &os)
{
    if(isObjStream()) {
        size_t i, icnt = _array.size();
        for(i=1;i<icnt;i++) {
            PPToken *token = _array[i];
            if(token->classType() == PPTN_INDIRECTOBJ) {
                PPTIndirectObj *indir_obj = (PPTIndirectObj *)token;
                indir_obj->_filepos = os.tellp();
                _parser->_filePtDict[indir_obj->_filepos] = indir_obj;
                _parser->_objDict[indir_obj->_objNum] = indir_obj;
            }
            string pdfstr = token->pdfString();
            if (pdfstr.length() == 0) {
                cout << "Zero PDF string for token " << token->typeName() << PP_ENDL;
            }
            os << pdfstr << PP_ENDL;
        }
    }
    else if (isStream()) {
        ostringstream ostr;
        
        _filepos = os.tellp();
        
        ostr << _objNum << " " << _genNum << " obj" << PP_ENDL;
        PPTDictionary *dict = (PPTDictionary *)_array[0];
        PPTStream *stream = (PPTStream *)_array[1];
        unsigned long len;
        string stream_pdfstr = stream->makePDFString(len);
        PPTNumber *len_num = new PPTNumber(_parser, (int)len);
        dict->setTokenAndKey(len_num, "Length");
        
        ostr << dict->pdfString();
        ostr << stream_pdfstr;

        ostr << "endobj" << PP_ENDL;
        os << ostr.str() << PP_ENDL;
        
        _parser->_filePtDict[_filepos] = this;
        _parser->_objDict[_objNum] = this;
    }
    else {
        PPToken::write(os);
        
        _parser->_filePtDict[_filepos] = this;
        _parser->_objDict[_objNum] = this;
    }
}

void PPTIndirectObj::addRefObj(PPTIndirectRef *ref)
{
    _ref_list.push_back(ref);
}

void PPTIndirectObj::setObjNum(int num)
{
    _objNum = num;
    size_t i, icnt = _ref_list.size();
    for (i=0; i<icnt; i++) {
        PPTIndirectRef *ref = _ref_list[i];
        ref->_objNum = num;
    }
}

string PPTIndirectObj::pdfString()
{
    ostringstream ostr;
    ostr << _objNum << " " << _genNum << " obj" << PP_ENDL;
    size_t i, icnt = _array.size();
    for(i=0;i<icnt;i++) {
        PPToken *token = _array[i];
        string pdfstr = token->pdfString();
        if (pdfstr.length() == 0) {
            cout << "Zero PDF string for token " << token->typeName() << PP_ENDL;
        }
        ostr << pdfstr;
    }
    ostr << "endobj" << PP_ENDL;
    string retstr = ostr.str();
    return retstr;
}

//void PPTIndirectObj::write(std::ostream &os)
//{
//    PPToken::write(os);
//    
//    _filepos = os.tellp();
//    string pdfstr = pdfString(os);
//    if (pdfstr.length() == 0) {
//        cout << "Zero PDF string for token " << typeName() << PP_ENDL;
//    }
//    os << pdfstr << PP_ENDL;
//    
//    
//    
//    _parser->_filePtDict[_filepos] = this;
//    _parser->_objDict[_objNum] = this;
//    
//}

void PPTIndirectObj::merge(PPTIndirectObj *ohter_indir)
{
    
}


//  ////////////////////////////////// PPTIndirectRef

PPTIndirectRef::PPTIndirectRef(PPParser *parser, int num1, int num2):PPToken(parser)
{
    _objNum = num1;
    _genNum = num2;
    parser->_ref_list.push_back(this);
}

string PPTIndirectRef::description()
{
    string retstr = "";
    retstr += "IndirectRef("+to_string((_Longlong)_objNum)+","+to_string((_Longlong)_genNum)+")";
    return retstr;
}

string PPTIndirectRef::xmlString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr <<tapStr(level)<<"<Ref ObjID='"<<_objNum<<"' Gen='"<<_genNum<< "'/>\xa";
    retstr = ostr.str();
    return retstr;
}

string PPTIndirectRef::pdfString()
{
    string retstr;
    ostringstream ostr;
    ostr << _objNum << " " << _genNum << " R";
    retstr = ostr.str();
    return retstr;
}

PPToken *PPTIndirectRef::valueObject()
{
    PPTIndirectObj *tar = targetObject();
    if (tar) {
        return tar->_array.size() > 0 ? tar->_array[0] : NULL;
    }
    return NULL;
    
}

string PPTIndirectRef::internalXmlString(int level)
{
    string retstr;
    PPToken *obj = valueObject();
//    if (obj->classType() == PPTN_DICTIONARY) {
//        retstr += tapStr(level) + "<Dict>\xa";
//        retstr += obj->internalXmlString(level+1);
//        retstr += tapStr(level) + "</Dict>\xa";
//    }
//    else {
        retstr += obj->internalXmlString(level);
//    }
    return retstr;
}

PPTIndirectObj *PPTIndirectRef::targetObject()
{
    PPTIndirectObj *ret = (PPTIndirectObj *)_parser->objectByID(_objNum);
    return ret;
}

//  ////////////////////////////////// PPTXRef
PPTXRef::PPTXRef(PPParser *parser):PPToken(parser)
{
    
}

PPTXRef::~PPTXRef()
{
    map <unsigned int, XRefItem *>::iterator it_dict;
    for(it_dict = _xreflist.begin(); it_dict != _xreflist.end(); it_dict++) {
        XRefItem *item = it_dict->second;
        delete item;
    }
    _xreflist.clear();
}

void PPTXRef::addXRef(unsigned int objnum, unsigned long long offset, int gennum, char type)
{
    XRefItem *item = new XRefItem;
    item->offset = offset;
    item->generation = gennum;
    item->type = type;
    _xreflist[objnum] = item;
}

string PPTXRef::xmlString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr <<tapStr(level)<< "<CrossRef Pos='" << _filepos << "'>\xa";
    map <unsigned int, XRefItem *> ::iterator it_xref_objs;
    for(it_xref_objs = _xreflist.begin(); it_xref_objs != _xreflist.end(); it_xref_objs++) {
        unsigned int objnum = it_xref_objs->first;
        ostr <<tapStr(level+1)<<"<XRef ObjID='"<<objnum<<"'>\xa";
        
        XRefItem *item = it_xref_objs->second;
        ostr <<tapStr(level+2)<<"<Offset>"<<item->offset<<"</Offset>\xa";
        ostr <<tapStr(level+2)<<"<Generation>"<<item->generation<<"</Generation>\xa";
        ostr <<tapStr(level+2)<<"<Type>"<<item->type<<"</Type>\xa";
        
        ostr <<tapStr(level+1)<<"</XRef>\xa";
    }
    ostr <<tapStr(level)<< "</CrossRef>\xa";
    retstr = ostr.str();
    return retstr;
}

string PPTXRef::pdfString()
{
    string retstr;
    ostringstream ostr;
    ostr << "xref" << PP_ENDL;
    ostr << "0 " << _parser->_last_obj_idx << PP_ENDL;
    int i, icnt = _parser->_last_obj_idx;
    for (i=0; i<icnt; i++) {
        PPTIndirectObj *obj = _parser->_objDict[i];
        char buf[20];
        if (i == 0) {
            sprintf(buf, "%010d %05d %c",0, 65535, 'f' );
        }
        else if (obj == NULL) {
            cout << "Unexpected Error : IndirectObj not exists at " << i << PP_ENDL;
            sprintf(buf, "%010d %05d %c",0, 65535, 'f' );
        }
        else {
            sprintf(buf, "%010d %05d %c",obj->_objNum, obj->_genNum, 'n' );
        }
        ostr << buf << PP_ENDL;
    }
    retstr = ostr.str();
    return retstr;
}

void PPTXRef::write(std::ostream &os)
{
    PPToken::write(os);
    _parser->_filePtDict[_filepos] = this;
}

size_t PPTXRef::numberOfItems()
{
    return _xreflist.size();
}

void PPTXRef::merge(PPTXRef *trailer)
{
    
}

//  ////////////////////////////////// PPTTrailer
//static const char  *PPKN_ROOT = "Root";
//static const char  *PPKN_INFO = "Info";
//static const char  *PPKN_FILEID = "ID";

PPTTrailer::PPTTrailer(PPParser *parser, PPTDictionary *dict, unsigned long long xref) : PPToken(parser)
{
    _dict = dict;
    _xrefIndirect = NULL;
    _startxref = xref;
    _next = NULL;
    _xrefObj = NULL;
}

PPTTrailer::PPTTrailer(PPParser *parser, PPTIndirectObj *indir, unsigned long long xref) : PPToken(parser)
{
    _dict = NULL;
    _xrefIndirect = indir;
    _startxref = xref;
    _next = NULL;
    _xrefObj = NULL;
}

PPTTrailer::PPTTrailer(PPParser *parser, unsigned long long xref) : PPToken(parser)
{
    _dict = NULL;
    _xrefIndirect = NULL;
    _startxref = xref;
    _next = NULL;
    _xrefObj = NULL;
}

PPTTrailer::PPTTrailer(PPParser *parser) : PPToken(parser)
{
    _dict = new PPTDictionary(_parser);//other_trailer->getDictionary();
    //    _dict = NULL;
    _xrefIndirect = NULL;
    _startxref = 0;
    _next = NULL;
    _xrefObj = NULL;
}

PPTTrailer::~PPTTrailer()
{
    if (_dict) {
        delete _dict;
    }
}
string PPTTrailer::xmlString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr <<tapStr(level)<< "<Trailer>\xa";
    if (_xrefIndirect) {
        ostr <<_xrefIndirect->xmlString(level+1);
    }
    if (_xrefObj) {
        ostr <<_xrefObj->xmlString(level+1);
    }
    if (_dict) {
        ostr <<_dict->xmlString(level+1);
    }
    ostr <<tapStr(level+1)<<"<StartXRef>"<<_startxref<<"</StartXRef>\xa";
    ostr <<tapStr(level)<< "</Trailer>\xa";
    retstr = ostr.str();
    return retstr;
}

string PPTTrailer::pdfString()
{
    string retstr;
    ostringstream ostr;
//    if (_xrefIndirect) { // for PDF 1.5 ^
//        ostr << _xrefIndirect->pdfString() << PP_ENDL;
//    }
//    if (_xrefObj) {
//        ostr << _xrefObj->pdfString() << PP_ENDL;
//    }
    if (_dict) {
        ostr << "trailer" << PP_ENDL;
        ostr << _dict->pdfString() << PP_ENDL;
    }
    else if (_xrefIndirect) { // for PDF 1.5 ^
        ostr << "trailer" << PP_ENDL;
        ostr << _xrefIndirect->pdfString() << PP_ENDL;
    }
    ostr << "startxref" << PP_ENDL;
    ostr << _startxref << PP_ENDL;
    ostr << "%%EOF" << PP_ENDL;
    retstr = ostr.str();
    return retstr;
}

PPTDictionary *PPTTrailer::getDictionary()
{
    if (_xrefIndirect) {
        return _xrefIndirect->firstDictionary();
    }
    if (_dict) {
        return _dict;
    }
    if (_startxref > 0) {
        PPToken *obj = _parser->objectAtFilePosition(_startxref);
        if (obj != NULL && obj->classType() == PPTN_INDIRECTOBJ) {
            PPTIndirectObj *indobj = (PPTIndirectObj *)obj;
            PPTDictionary *dict = indobj->firstDictionary();
            return dict;
        }
    }
    return _dict;
}

void PPTTrailer::setRootObject(PPTIndirectRef *indir_obj)
{
    PPTDictionary *dict = getDictionary();
    dict->setTokenAndKey(indir_obj, PPKN_ROOT);
}

void PPTTrailer::setInfoObject(PPTIndirectRef *indir_obj)
{
    PPTDictionary *dict = getDictionary();
    dict->setTokenAndKey(indir_obj, PPKN_INFO);
}

void PPTTrailer::setFileID(PPTArray *idarr)
{
    PPTDictionary *dict = getDictionary();
    dict->setTokenAndKey(idarr, PPKN_FILEID);
}


PPToken *PPTTrailer::rootObject()
{
    PPTDictionary *dict = getDictionary();
    PPToken *ret = dict->indirectObjectForKey(PPKN_ROOT); // Indirect Ref.
    return ret;
}

PPToken *PPTTrailer::infoObject()
{
    PPTDictionary *dict = getDictionary();
    PPToken *ret = dict->indirectObjectForKey(PPKN_INFO); // Indirect Ref.
    return ret;
}

void PPTTrailer::build()
{
    int obj_cnt = (int) _parser->_objDict.size();
    _dict->_dict["Size"] = new PPTNumber(_parser, obj_cnt+1);
}

void PPTTrailer::merge(PPTTrailer *other_trailer)
{
    if(other_trailer->_xrefIndirect) {
        if (!_xrefIndirect)
            _xrefIndirect = other_trailer->_xrefIndirect;
        else {
            _xrefIndirect->merge(other_trailer->_xrefIndirect);
        }
    }
    

    if (other_trailer->getDictionary() == NULL) {
        return;
    }
    
    if (_dict == NULL) {
        _dict = new PPTDictionary(_parser);//other_trailer->getDictionary();
    }
    
//    map <string, PPToken *> _dict;
    int obj_cnt = (int) _parser->_objDict.size();
    char buf[10];
    sprintf(buf, "%d", obj_cnt);
    
    string *numstr = new string(buf);
    _dict->_dict["Size"] = new PPTNumber(_parser, numstr);
    

    
    map <string, PPToken *> other_dict = other_trailer->getDictionary()->_dict;
    
    map <string, PPToken *> ::iterator it_token_objs;
    for(it_token_objs = other_dict.begin(); it_token_objs != other_dict.end(); it_token_objs++) {
        string key = it_token_objs->first;
        PPToken *mytoken = _dict->_dict[key];
        if(mytoken) {
        }
        else {
            _dict->_dict.erase(key);
            if (key == "Prev") {
                cout << "key is Prev..." << PP_ENDL;
            }
            else {
                PPToken *other_token = it_token_objs->second;
                _dict->_dict[key] = other_token;
            }
        }
    }
}

void PPTTrailer::write(std::ostream &os)
{
    PPToken::write(os);
    _parser->_filePtDict[_filepos] = this;
    
    
}

//PPToken *PPTTrailer::rootObject()
//{
//    if (_dict != NULL) {
//        PPToken *ret = _dict->indirectObjectForKey(PPKN_ROOT); // Indirect Ref.
//        return ret;
//    }
//    if (_startxref > 0) {
//        PPToken *obj = _parser->objectAtFilePosition(_startxref);
//        if (obj != NULL && obj->classType() == PPTN_INDIRECTOBJ) {
//            PPTIndirectObj *indobj = (PPTIndirectObj *)obj;
//            _dict = indobj->firstDictionary();
//            if (_dict == NULL) {
//                return NULL;
//            }
//            PPToken *ret = _dict->indirectObjectForKey(PPKN_ROOT);
//            return ret;
//        }
//    }
//    return NULL;
//}


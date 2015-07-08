
#include <sstream>
#include "PPTTrailer.h"
#include "PPParser.h"
#include "PPTNumber.h"


//  ////////////////////////////////// PPTTrailer
//static const char  *PPKN_ROOT = "Root";
//static const char  *PPKN_INFO = "Info";
//static const char  *PPKN_FILEID = "ID";
#include "PPTArray.h"
#include "PPTDictionary.h"
#include "PPTIndirectObj.h"
#include "PPTIndirectRef.h"
#include "PPTXRef.h"

PPTTrailer::PPTTrailer()
{
    _dict = NULL;
    _xrefIndirect = NULL;
    _startxref = 0L;
    _next = NULL;
    _xrefObj = NULL;
}

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

void PPTTrailer::CopyMembersTo(PPBase *obj)
{
	PPToken::CopyMembersTo(obj);

	PPTTrailer *trailer = (PPTTrailer *)obj;
//	trailer->_startxref = _startxref;
}

void PPTTrailer::SetParser(PPParser *parser)
{
	PPToken::SetParser(parser);
	_dict->SetParser(parser);
	_xrefObj->SetParser(parser);
}
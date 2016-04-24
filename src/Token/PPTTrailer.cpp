
#include <sstream>
#include "PPTTrailer.h"
#include "PPDocument.h"
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

PPTTrailer::PPTTrailer(PPDocument *doc, PPTDictionary *dict, unsigned long long xref) : PPToken(doc)
{
    _dict = dict;
    _xrefIndirect = NULL;
    _startxref = xref;
    _next = NULL;
    _xrefObj = NULL;
}

PPTTrailer::PPTTrailer(PPDocument *doc, PPTIndirectObj *indir, unsigned long long xref) : PPToken(doc)
{
    _dict = NULL;
    _xrefIndirect = indir;
    _startxref = xref;
    _next = NULL;
    _xrefObj = NULL;
}

PPTTrailer::PPTTrailer(PPDocument *parser, unsigned long long xref) : PPToken(parser)
{
    _dict = NULL;
    _xrefIndirect = NULL;
    _startxref = xref;
    _next = NULL;
    _xrefObj = NULL;
}

PPTTrailer::PPTTrailer(PPDocument *doc) : PPToken(doc)
{
    _dict = new PPTDictionary(_document);//other_trailer->getDictionary();
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
string PPTTrailer::XMLString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr <<PPTabStr(level)<< "<Trailer>\xa";
    if (_xrefIndirect) {
        ostr <<_xrefIndirect->XMLString(level+1);
    }
    if (_xrefObj) {
        ostr <<_xrefObj->XMLString(level+1);
    }
    if (_dict) {
        ostr <<_dict->XMLString(level+1);
    }
    ostr <<PPTabStr(level+1)<<"<StartXRef>"<<_startxref<<"</StartXRef>\xa";
    ostr <<PPTabStr(level)<< "</Trailer>\xa";
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
        PPToken *obj = _document->ObjectAtFilePosition(_startxref);
        if (obj != NULL && obj->ClassType() == PPTN_INDIRECTOBJ) {
            PPTIndirectObj *indobj = (PPTIndirectObj *)obj;
            PPTDictionary *dict = indobj->firstDictionary();
            return dict;
        }
    }
    return _dict;
}

void PPTTrailer::SetRootObject(PPTIndirectRef *indir_obj)
{
    PPTDictionary *dict = getDictionary();
    dict->setTokenAndKey(indir_obj, PPKN_ROOT);
}

void PPTTrailer::SetInfoObject(PPTIndirectRef *indir_obj)
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
    PPToken *ret = dict->IndirectObjectForKey(PPKN_ROOT); // Indirect Ref.
    return ret;
}

PPToken *PPTTrailer::infoObject()
{
    PPTDictionary *dict = getDictionary();
    PPToken *ret = dict->IndirectObjectForKey(PPKN_INFO); // Indirect Ref.
    return ret;
}

void PPTTrailer::Build()
{
    int obj_cnt = (int) _document->_objDict.size();
    _dict->_dict["Size"] = new PPTNumber(_document, obj_cnt+1);
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
        _dict = new PPTDictionary(_document);//other_trailer->getDictionary();
    }
    
//    map <string, PPToken *> _dict;
    int obj_cnt = (int) _document->_objDict.size();
    char buf[10];
    sprintf(buf, "%d", obj_cnt);
    
    string *numstr = new string(buf);
    _dict->_dict["Size"] = new PPTNumber(_document, numstr);
    

    
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

void PPTTrailer::Write(std::ostream &os)
{
    PPToken::Write(os);
    _document->_filePtDict[_filepos] = this;
    
    
}

void PPTTrailer::CopyMembersTo(PPBase *obj)
{
	PPToken::CopyMembersTo(obj);

	PPTTrailer *trailer = (PPTTrailer *)obj;

}

void PPTTrailer::SetDocument(PPDocument *doc)
{
	PPToken::SetDocument(doc);
	_dict->SetDocument(doc);
	_xrefObj->SetDocument(doc);
}


#include <sstream>

#include "PPTIndirectObj.h"

#include "PPTName.h"
#include "PPTNumber.h"
#include "PPTDictionary.h"
#include "PPTStream.h"
#include "PPTIndirectRef.h"

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

void PPTIndirectObj::AddObj(PPToken *obj)
{
	_array.push_back(obj);
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

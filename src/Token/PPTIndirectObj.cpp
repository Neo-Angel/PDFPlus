

#include <sstream>

#include "PPDocument.h"

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
	if(obj == this) {
		cout << "obj == this" << PP_ENDL;
	}
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
		if(_array.size() == 1) {
			cout << "Error : Stream must have 'stream' object!" << PP_ENDL;
		}
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

        ostr << "endobj";// << PP_ENDL;
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
    ostr << "endobj";// << PP_ENDL;
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

void PPTIndirectObj::CopyMembersTo(PPBase *obj)
{
	PPToken::CopyMembersTo(obj);

	PPTIndirectObj *indir_obj = (PPTIndirectObj *)obj;
	indir_obj->_genNum = _genNum;
	indir_obj->_objNum = _objNum;

	indir_obj->_array.clear();
	int i, icnt = _array.size();
	for(i=0;i<icnt;i++) {
		PPToken *org_token = _array.at(i);
		PPToken *new_token = (PPToken *)org_token->Copy();
		if(i == 1 && new_token->classType() == PPTN_STREAM) {
			PPTDictionary *dict = (PPTDictionary *)indir_obj->_array.at(0);
			PPTStream *stream = (PPTStream *)new_token;
			stream->_dict = dict;
		}
		indir_obj->AddObj(new_token);
	}
	_ref_list.clear();
}

void PPTIndirectObj::SetParser(PPParser *parser)
{
	PPToken::SetParser(parser);

	int i, icnt = _array.size();
	for(i=0;i<icnt;i++) {
		PPToken *token = _array.at(i);
		if(parser != token->_parser)
			token->SetParser(parser);
	}

}


// 도큐먼트 파라미터는 나중에 PPParser 로 교체하자.
//     _tokens 를 PPParser 클래스로 옮기기
void PPTIndirectObj::MoveInto(PPDocument *doc)
{
	int i, icnt = _array.size();
	for(i=0;i<icnt;i++) {
		PPToken *token = _array.at(i);
		if(&(doc->_parser) != token->_parser) {
			token->MoveInto(doc);
		}
	}
}




#include <sstream>

#include "PPDocument.h"

#include "PPTIndirectObj.h"

#include "PPTString.h"
#include "PPTName.h"
#include "PPTNumber.h"
#include "PPTDictionary.h"
#include "PPTStream.h"
#include "PPTIndirectRef.h"

//  ////////////////////////////////// PPTIndirectObj
PPTIndirectObj::PPTIndirectObj(PPDocument *doc, vector<PPToken *> token_list, int num1, int num2):PPToken(doc)
{
    _objNum = num1;
    _genNum = num2;
    _array = token_list;
	_parentObj = NULL;
}

PPTIndirectObj::PPTIndirectObj(PPDocument *doc, int num1, int num2):PPToken(doc)
{
	_objNum = num1;
	_genNum = num2;
	_parentObj = NULL;
}

void PPTIndirectObj::Clear()
{
    int i, icnt = (int)_array.size();
    for (i=0; i<icnt; i++) {
        PPToken *token = _array.at(i);
        delete token;
    }
	_array.clear();
}

PPTIndirectObj::~PPTIndirectObj()
{
	this->Clear();
}

string PPTIndirectObj::Description()
{
    string retstr;
    int i, icnt = (int)_array.size();
    for (i=0; i<icnt; i++) {
        PPToken *token = _array.at(i);
        retstr += token->Description() + '\xa';
    }
    
    return retstr;
}


string PPTIndirectObj::XMLString(int level)
{
    ostringstream ostr;
    ostr <<PPTabStr(level)<<"<Object ID='"<<_objNum<<"' Gen='"<<_genNum<< "' Pos='"<<_filepos<<"'>\xa";
    string retstr = ostr.str();
//    retstr += PPTabStr(level) + "<Indirect Object=" + _objNum + ", Generation="+ _genNum+ ">\xa";
    size_t icnt = _array.size();
    size_t i;
    icnt = _array.size();
    for (i=0; i<icnt; i++) {
        PPToken *token = _array.at(i);
        retstr += token->XMLString(level+1);
//        delete token;
    }
    retstr += PPTabStr(level) + "</Object>\xa";
    
    return retstr;
}
PPTDictionary *PPTIndirectObj::FirstDictionary()
{
    if (_array.size() == 0) {
        return NULL;
    }
    PPToken *token = _array.at(0);
    if (token->ClassType() != PPTN_DICTIONARY) {
        return NULL;
    }
    return (PPTDictionary *)token;
}

void PPTIndirectObj::AddToken(PPToken *token)
{
	if(token == this) {
		PP_ERR << "obj == this" << PP_ENDL;
	}
	token->_parentObj = this;
	_array.push_back(token);
}

void PPTIndirectObj::AddRefObj(PPTIndirectRef *ref)
{
    _ref_list.push_back(ref);
}

void PPTIndirectObj::SetFirstObject(PPToken *obj) 
{
	this->Clear();
	this->AddToken(obj);
}
	
PPTStream *PPTIndirectObj::Stream()
{
    if (_array.size() != 2) {
        return NULL;
    }
    PPToken *token = _array.at(1);
    if (token->ClassType() != PPTN_STREAM) {
        return NULL;
    }
    return (PPTStream *)token;
}

bool PPTIndirectObj::IsObjStream()
{
    PPTDictionary *dict = (PPTDictionary *)_array[0];
    if (dict->ClassType() != PPTN_DICTIONARY)
        return false;
    
    PPTName *type = (PPTName *)dict->ObjectForKey("Type");
    if (type != NULL && *type->_name == "ObjStm") {
        return true;
    }
    return false;
}

bool PPTIndirectObj::IsStream()
{
    PPTDictionary *dict = (PPTDictionary *)_array[0];
    if (dict->ClassType() != PPTN_DICTIONARY)
        return false;
    
    PPTName *filter = (PPTName *)dict->ObjectForKey("Filter");

    if (filter != NULL) {
		if(filter->ClassType() == PPTN_NAME) {
			if(*filter->_name == "FlateDecode") {
				return true;
			}
			else if(*filter->_name == "DCTDecode") {
				return true;
			}
			else {
				cout << "Unknown stream type '" << *filter->_name << "'." << PP_ENDL;
				return false;
			}
		}
		else {
			PPTArray *filter_list = (PPTArray *)filter;
			uint i;
			size_t icnt = filter_list->NumberOfTokens();
			for(i=0;i<icnt;i++) {
				PPTName *fname = (PPTName *)filter_list->TokenAtIndex(i);
				if(*fname->_name == "FlateDecode") {
					return true;
				}
			}

		}
    }
    return false;
}

// OCG Related Methods
bool PPTIndirectObj::IsOCG()
{
	PPTDictionary *dict = this->FirstDictionary();
	if(dict == NULL) 
		return false;
	PPTName *type = (PPTName *)dict->ObjectForKey("Type");
	if(type == NULL)
		return false;
	if(*type->_name != "OCG") 
		return false;
	PPTString *name = (PPTString *)dict->ObjectForKey("Name");
	if(name == NULL)
		return false;
	return true;
}
	
string PPTIndirectObj::OCGName()
{
	string name;
	do {
		PPTDictionary *dict = this->FirstDictionary();
		if(dict == NULL) 
			break;
		PPTString *name_token = (PPTString *)dict->ObjectForKey("Name");
		if(name_token == NULL)
			break;
		name = *name_token->_string;

	} while(0);
	return name;
}


PPTIndirectObj *PPTIndirectObj::GetParentObj()
{
	if(_ref_list.size() != 1) 
		return NULL;
	PPTIndirectRef *pref = _ref_list[0];
	PPTIndirectObj *parent = pref->_parentObj;
	return parent;
}


void PPTIndirectObj::Write(ostream &os)
{
    if(IsObjStream()) {
        size_t i, icnt = _array.size();
        for(i=1;i<icnt;i++) {
            PPToken *token = _array[i];
            if(token->ClassType() == PPTN_INDIRECTOBJ) {
                PPTIndirectObj *indir_obj = (PPTIndirectObj *)token;
                indir_obj->_filepos = os.tellp();
                _document->_filePtDict[indir_obj->_filepos] = indir_obj;
                _document->_objDict[indir_obj->_objNum] = indir_obj;
            }
            string pdfstr = token->PDFString();
            if (pdfstr.length() == 0) {
                cout << "Zero PDF string for token " << token->TypeName() << PP_ENDL;
            }
            os << pdfstr << PP_ENDL;
        }
    }
    else if (IsStream()) {
		if(_array.size() == 1) {
			PP_ERR << "Stream must have 'stream' object!" << PP_ENDL;
		}
        ostringstream ostr;
        
        _filepos = os.tellp();
        
        ostr << _objNum << " " << _genNum << " obj" << PP_ENDL;
        PPTDictionary *dict = (PPTDictionary *)_array[0];
        PPTStream *stream = (PPTStream *)_array[1];
        unsigned long len;
        string stream_pdfstr = stream->MakePDFString(len);
        PPToken *length_token = dict->ObjectForKey("Length");
        if(length_token->ClassType() == PPTN_INDIRECTREF) {
            PPTIndirectObj *length_obj = dict->IndirectObjectForKey("Length");
            PPTNumber *length_num = (PPTNumber *)length_obj->TokenAtIndex(0);
            length_num->SetInteger((int)len);
        }
        else {
            PPTNumber *len_num = new PPTNumber(_document, (int)len);
            int obj_num = _document->NewObjNum();
            PPTIndirectObj *indir_obj = new PPTIndirectObj(_document, obj_num, 0);
            indir_obj->AddToken(len_num);
            PPTIndirectRef *indir_ref = new PPTIndirectRef(_document, obj_num, 0);
            indir_obj->AddRefObj(indir_ref);
            _document->PushObj(indir_obj);
            dict->SetTokenAndKey(indir_ref, "Length");
        }
        ostr << dict->PDFString();
        ostr << stream_pdfstr;

        ostr << "endobj";// << PP_ENDL;
        os << ostr.str() << PP_ENDL;
        
        _document->_filePtDict[_filepos] = this;
        _document->_objDict[_objNum] = this;
    }
    else {
        PPToken::Write(os);
        
        _document->_filePtDict[_filepos] = this;
        _document->_objDict[_objNum] = this;
    }
}

void PPTIndirectObj::SetObjNum(int num)
{
    _objNum = num;
    size_t i, icnt = _ref_list.size();
    for (i=0; i<icnt; i++) {
        PPTIndirectRef *ref = _ref_list[i];
        ref->_objNum = num;
    }
}

string PPTIndirectObj::PDFString()
{
    ostringstream ostr;
    ostr << _objNum << " " << _genNum << " obj" << PP_ENDL;
    size_t i, icnt = _array.size();
    string pdfstr;
    for(i=0;i<icnt;i++) {
        PPToken *token = _array[i];
        pdfstr = token->PDFString();
        if (pdfstr.length() == 0) {
            cout << "Zero PDF string for token " << token->TypeName() << PP_ENDL;
        }
        ostr << pdfstr;
    }
    if(pdfstr[pdfstr.length()-1] != '\n') {
        ostr << "\n";
    }
    ostr << "endobj";// << PP_ENDL;
    string retstr = ostr.str();
    return retstr;
}

void PPTIndirectObj::Merge(PPTIndirectObj *ohter_indir)
{
    
}

void PPTIndirectObj::CopyMembersTo(PPBase *obj)
{
	PPToken::CopyMembersTo(obj);

	PPTIndirectObj *indir_obj = (PPTIndirectObj *)obj;
	indir_obj->_genNum = _genNum;
	indir_obj->_objNum = _document->NewObjNum(); //_objNum;

	indir_obj->Clear();
	size_t i, icnt = _array.size();
	for(i=0;i<icnt;i++) {
		PPToken *org_token = _array.at(i);
		PPToken *new_token = (PPToken *)org_token->Copy();
		if(i == 1 && new_token->ClassType() == PPTN_STREAM) {
			PPTDictionary *dict = (PPTDictionary *)indir_obj->TokenAtIndex(0);
			PPTStream *stream = (PPTStream *)new_token;
			stream->_infoDict = dict;
		}
		indir_obj->AddToken(new_token);
	}
}

void PPTIndirectObj::SetDocument(PPDocument *doc)
{
	PPToken::SetDocument(doc);

	size_t i, icnt = _array.size();
	for(i=0;i<icnt;i++) {
		PPToken *token = _array.at(i);
		if(doc != token->_document)
			token->SetDocument(doc);
	}

}

//     _tokens 를 PPParser 클래스로 옮기기
void PPTIndirectObj::MoveInto(PPDocument *doc)
{

	size_t i, icnt = _array.size();
	for(i=0;i<icnt;i++) {
		PPToken *token = _array.at(i);
		if(doc != token->_document) {
			token->MoveInto(doc);
		}
	}


	PPToken::MoveInto(doc);
}

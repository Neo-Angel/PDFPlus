
#include "PPTDictionary.h"

#include "PPDocument.h"
#include "PPTName.h"
#include "PPTArray.h"
#include "PPTIndirectObj.h"
#include "PPTIndirectRef.h"
#include "PPTNumber.h"
#include "PPParser.h"
#include "PPTString.h"

// PPTDictionary //////////////////////////////////


PPTDictionary::PPTDictionary(PPDocument *doc) : PPToken(doc)
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

void PPTDictionary::SetTokenAndKey(PPToken *token, string key)
{
    _dict[key] = token;
}

void PPTDictionary::SetTokenAndKey(string name, string key)
{
	string *name_ptr = new string(name);
	PPTName *name_obj = new PPTName(_document, name_ptr);
	SetTokenAndKey(name_obj, key);
}

void PPTDictionary::SetStringAndKey(string name, string key)
{
	string *name_ptr = new string(name);
	PPTString *str_obj = new PPTString(_document, name_ptr);
	SetTokenAndKey(str_obj, key);
}

void PPTDictionary::SetNameAndKey(string name, string key)
{
	string *name_ptr = new string(name);
	PPTName *str_obj = new PPTName(_document, name_ptr);
	SetTokenAndKey(str_obj, key);
}


void PPTDictionary::SetTokenAndKey(int num, string key)
{
	PPTNumber *num_obj = new PPTNumber(_document, num);
	SetTokenAndKey(num_obj, key);
}

// IndirectObj�� ����� token�� �ְ� IndirectRef�� ����� key�� Set ��.
PPTIndirectObj *PPTDictionary::SetRefTokenAndKey(PPToken *token, string key, int obj_num)
{
	PPTIndirectRef *ref = new PPTIndirectRef(_document, obj_num, 0);
	SetTokenAndKey(ref, key);
	PPTIndirectObj *obj = NULL;
//	obj_num = _document->NextObjectNumber(); // �ӵ������� ��� ����
	if(token->ClassType() == PPTN_INDIRECTOBJ) {
		obj = (PPTIndirectObj *)token;
		obj->_objNum = obj_num; // ???
	}
	else  {
		obj = (PPTIndirectObj *)_document->ObjectForNumber(obj_num);
		if(obj) {
			PP_ERR << "Object must not exists." << PP_ENDL;
			return NULL;
		}
		else {
			obj = new PPTIndirectObj(_document, obj_num, 0);
		}
		obj->AddObj(token);  // �� �κ��� �ٷ� �� if�������� ���� �Ǵ°� �ƴ���...
	}
	obj->AddRefObj(ref);
	return obj;
}

PPToken *PPTDictionary::ObjectForKey(string &keyname)
{
    PPToken *ret_token = _dict[keyname];
    if (ret_token == NULL) {
        _dict.erase(keyname);
    }
    return ret_token;
}

PPToken *PPTDictionary::ObjectForKey(const char *keyname)
{
	string key(keyname);
    PPToken *ret_token = _dict[key];
    if (ret_token == NULL) {
        _dict.erase(keyname);
    }
    return ret_token;
}

PPTName *PPTDictionary::NameForKey(const char *keyname)
{
    PPTName *ret_name = (PPTName *)_dict[keyname];
    if (ret_name == NULL) {
        _dict.erase(keyname);
		return NULL;
    }

    if (ret_name->TypeName() == PPTN_ARRAY) {
		PPTArray *arr = (PPTArray *)ret_name;
		if(arr->Size() == 0) {
			return NULL;
		}
		ret_name = (PPTName *) arr->ObjectAtIndex(0);
    }
    return ret_name;
}

PPTIndirectObj *PPTDictionary::IndirectObjectForKey(string &keyname)
{
    PPToken *ret = _dict[keyname];
    if (ret) {
        if (ret->ClassType() == PPTN_INDIRECTREF) {
            PPTIndirectRef *indref = (PPTIndirectRef *)ret;
            PPTIndirectObj *tar = indref->TargetObject();
            return tar;
        }
    }
    else {
        _dict.erase(keyname);
    }
    return NULL; //(PPTIndirectObj *)ret;
}

PPToken *PPTDictionary::ValueObjectForKey(string &keyname)
{
    PPToken *ret = _dict[keyname];
    if (ret != NULL && ret->ClassType() == PPTN_INDIRECTREF) {
        PPTIndirectRef *indref = (PPTIndirectRef *)ret;
        PPTIndirectObj *tar = indref->TargetObject();
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


string PPTDictionary::Description()
{
    string str;
    str = "Dict {\xa";
    
    map <string, PPToken *> ::iterator it_token_objs;
    for(it_token_objs = _dict.begin(); it_token_objs != _dict.end(); it_token_objs++) {
        string name = it_token_objs->first;
        PPToken *token = (PPToken *)(it_token_objs->second);
        string desc = token->Description();
        str += "\t["+name + "] : [" + desc+"]\xa";
    }
    str += "}\xa";
    return str;
}

string PPTDictionary::XMLString(int level)
{
    string retstr;
    retstr += PPTabStr(level) + "<Dict>\xa";
    map <string, PPToken *> ::iterator it_token_objs;
    for(it_token_objs = _dict.begin(); it_token_objs != _dict.end(); it_token_objs++) {
        string name = it_token_objs->first;
        retstr += PPTabStr(level+1) + "<Key>" + name + "</Key>\xa";
        
        PPToken *token = (PPToken *)(it_token_objs->second);
        retstr += token->XMLString(level+1);
    }
    retstr += PPTabStr(level) + "</Dict>\xa";
    
    return retstr;
}

string PPTDictionary::PDFString()
{
    string retstr = "<<";
    map <string, PPToken *> ::iterator it_token_objs;
    for(it_token_objs = _dict.begin(); it_token_objs != _dict.end(); it_token_objs++) {
        retstr += "/";
        string name = it_token_objs->first;
        retstr += name;
        retstr += " ";
        PPToken *token = (PPToken *)(it_token_objs->second);
		if(name == "Length") {
			cout << "Length = " << token->PDFString() << ";" << PP_ENDL;
		}
        retstr += token->PDFString();
        retstr += "\xa";
    }
    retstr += ">>\xa";
    return retstr;
}


void PPTDictionary::CopyMembersTo(PPBase *obj)
{
	PPToken::CopyMembersTo(obj);

	PPTDictionary *tar_dict = (PPTDictionary *)obj;
	map <string, PPToken *> ::iterator it_token_objs;
    for(it_token_objs = _dict.begin(); it_token_objs != _dict.end(); it_token_objs++) {
        string key = it_token_objs->first;
        PPToken *token = (PPToken *)(it_token_objs->second);
		PPToken *copied_token = (PPToken *)token->Copy();
		tar_dict->_dict[key] = copied_token;
    }

}

void PPTDictionary::SetDocument(PPDocument *doc)
{
	PPToken::SetDocument(doc);

	map <string, PPToken *> ::iterator it_token_objs;
    for(it_token_objs = _dict.begin(); it_token_objs != _dict.end(); it_token_objs++) {
        PPToken *token = (PPToken *)(it_token_objs->second);
		if(doc != token->_document)
			token->SetDocument(doc);
    }
}


void PPTDictionary::MoveInto(PPDocument *doc)
{
	map <string, PPToken *> ::iterator it_token_objs;
    for(it_token_objs = _dict.begin(); it_token_objs != _dict.end(); it_token_objs++) {
        PPToken *token = (PPToken *)(it_token_objs->second);
		token->MoveInto(doc);
    }

}
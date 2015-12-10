
#include "PPTDictionary.h"

#include "PPTName.h"
#include "PPTArray.h"
#include "PPTIndirectObj.h"
#include "PPTIndirectRef.h"
#include "PPTNumber.h"
#include "PPParser.h"
#include "PPTString.h"

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

void PPTDictionary::SetStringAndKey(string name, string key)
{
	string *name_ptr = new string(name);
	PPTString *str_obj = new PPTString(_parser, name_ptr);
	setTokenAndKey(str_obj, key);
}

void PPTDictionary::SetNameAndKey(string name, string key)
{
	string *name_ptr = new string(name);
	PPTName *str_obj = new PPTName(_parser, name_ptr);
	setTokenAndKey(str_obj, key);
}


void PPTDictionary::SetTokenAndKey(int num, string key)
{
	PPTNumber *num_obj = new PPTNumber(_parser, num);
	SetTokenAndKey(num_obj, key);
}

PPTIndirectObj *PPTDictionary::SetRefTokenAndKey(PPToken *token, string key, int obj_num)
{
	PPTIndirectRef *ref = new PPTIndirectRef(_parser, obj_num, 0);
	SetTokenAndKey(ref, key);
	PPTIndirectObj *obj = NULL;
	if(token->classType() == PPTN_INDIRECTOBJ) {
		obj = (PPTIndirectObj *)token;
		obj->_objNum = obj_num; // ???
	}
	else  {
		obj = (PPTIndirectObj *)_parser->ObjectForNumber(obj_num);
		if(!obj) {
			obj = new PPTIndirectObj(_parser, obj_num, 0);
		}
		obj->AddObj(token);  // 이 부분은 바로 위 if문안으로 들어가야 되는거 아닌지...
	}
	obj->addRefObj(ref);
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
	string key(keyname);
    PPToken *ret_token = _dict[key];
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

PPTIndirectObj *PPTDictionary::indirectObjectForKey(string &keyname)
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
    return NULL; //(PPTIndirectObj *)ret;
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
    retstr += tabStr(level) + "<Dict>\xa";
    map <string, PPToken *> ::iterator it_token_objs;
    for(it_token_objs = _dict.begin(); it_token_objs != _dict.end(); it_token_objs++) {
        string name = it_token_objs->first;
        retstr += tabStr(level+1) + "<Key>" + name + "</Key>\xa";
        
        PPToken *token = valueObjectForKey(name);
        retstr += token->internalXmlString(level+1);
    }
    retstr += tabStr(level) + "</Dict>\xa";
    
    return retstr;
}

string PPTDictionary::xmlString(int level)
{
    string retstr;
    retstr += tabStr(level) + "<Dict>\xa";
    map <string, PPToken *> ::iterator it_token_objs;
    for(it_token_objs = _dict.begin(); it_token_objs != _dict.end(); it_token_objs++) {
        string name = it_token_objs->first;
        retstr += tabStr(level+1) + "<Key>" + name + "</Key>\xa";
        
        PPToken *token = (PPToken *)(it_token_objs->second);
        retstr += token->xmlString(level+1);
    }
    retstr += tabStr(level) + "</Dict>\xa";
    
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
		if(name == "Length") {
			cout << "Length = " << token->pdfString() << ";" << PP_ENDL;
		}
        retstr += token->pdfString();
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

void PPTDictionary::SetParser(PPParser *parser)
{
	PPToken::SetParser(parser);

	map <string, PPToken *> ::iterator it_token_objs;
    for(it_token_objs = _dict.begin(); it_token_objs != _dict.end(); it_token_objs++) {
        PPToken *token = (PPToken *)(it_token_objs->second);
		if(parser != token->_parser)
			token->SetParser(parser);
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
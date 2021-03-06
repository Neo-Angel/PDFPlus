﻿
#include <sstream>
#include "PPEShading.h"
#include "PPTName.h"
#include "PPTDictionary.h"
#include "PPTIndirectRef.h"



//
//  PPEShading def
//
///////////////////////////////////////////////////////
PPEShading::PPEShading(PPContext *gcontext) : PPElement(gcontext)
{
	_sh_res = NULL;
	_name = NULL;
}

PPEShading::PPEShading()
{
	_sh_res = NULL;
	_name = NULL;
}

void PPEShading::CopyMembersTo(PPBase *obj)
{
	PPElement::CopyMembersTo(obj);
	PPEShading *tar_obj = (PPEShading *)obj;

	if(_name)
		tar_obj->_name = (PPTName *)_name->Copy();
	if(_sh_res)
		tar_obj->_sh_res = (PPTIndirectObj *)_sh_res->Copy();

	PPDocument *tar_doc = tar_obj->_parentForm->_document;
	PPDocument *this_doc = this->_parentForm->_document;
	if(tar_doc !=  this_doc && _name->_name->length() > 0) {
		PPTIndirectObj *rsc_obj = _parentForm->ResourceObjForName(*_name->_name, "Shading");
		if(rsc_obj) {
			// this->Document() 에 있는 rsc_obj를 tar_doc에 복사해 넣음
			// 복사할 때 rsc_obj의 objNum를 tar_doc에 맞춰서 변경함.
			PPTIndirectObj *new_obj = tar_doc->MoveObjFrom(rsc_obj, this->Document());
			PPTIndirectRef *new_ref = tar_obj->_parentForm->AddResourceRef(new_obj->_objNum, *_name->_name, "Shading");
			if(new_ref) {
				new_obj->AddRefObj(new_ref);
			}
			tar_obj->_sh_res = new_obj;
		}
	}

}

void PPEShading::SetDocument(PPDocument *doc)
{
	if (_name)
		_name->_document = doc;
	if (_sh_res)
		_sh_res->_document = doc;
}

string PPEShading::MakeCommandString()
{
    string retstr;
	if(_name) {
		retstr = _name->PDFString();
		retstr += " sh\xa";
	}
    return retstr;
}

string PPEShading::XMLString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr << PPTabStr(level) << "<Element type='Shading' name='" << *_name->_name << "'>" << PP_ENDL;
    if (_sh_res) {
        ostr << _sh_res->XMLString(level+1);
    }
//    if (_dict) {
//        ostr << _dict->XMLString(level+1);
//    }
    ostr << PPTabStr(level) << "</Element>" << PP_ENDL;
    retstr = ostr.str();
    return retstr;
}

void PPEShading::WillAddToParent(PPFormBase *form)
{
    PPElement::WillAddToParent(form);
	_sh_res = (PPTIndirectObj *)PPElement::ResourceObjectFor(PPRT_SHADING);
    if (!_sh_res) {
        cout << "Shading Resource Object not found..." << PP_ENDL;
        return;
	}
}


bool PPEShading::HasResource()
{
	return true;
}

/////////////////////////////////////////////////////  Multi Resource Handling
vector <const char *> PPEShading::ResourceTypeList()
{
	vector <const char *> rsc_list = PPElement::ResourceTypeList();
	rsc_list.push_back(PPRT_SHADING);
	return rsc_list;
}

string PPEShading::ResourceKeyFor(const char *rsc_type)
{
	string key = PPElement::ResourceKeyFor(rsc_type);
	if(key.length() > 0) {
		return key;
	}
	return *_name->_name;
}


PPToken *PPEShading::ResourceObjectFor(const char *rsc_type)
{
	if(rsc_type == PPRT_SHADING) {
		if(_sh_res)
			return _sh_res;
	}

	return PPElement::ResourceObjectFor(rsc_type);
}

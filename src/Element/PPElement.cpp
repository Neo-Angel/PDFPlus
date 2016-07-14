//
//  PPElement.cpp
//  PDFPlusLib
//
//  Created by Neo Angel on 2014. 9. 11..
//  Copyright (c) 2014년 #CORP#. All rights reserved.
//


//#include "StdAfx.h"

#include "PPBase.h"
#include "PPDocument.h"
#include "PPElement.h"
#include "PPTStream.h"
#include "PPCommandParser.h"
#include "PPContext.h"
#include "PPFormBase.h"
#include "PPTDictionary.h"
#include "PPTIndirectRef.h"

#include <sstream>

// ExtGState, ColorSpace, Pattern, Shading, XObject, Font, ProcSet, Properties
const char *PPRT_EXTSTATE = "ExtGState";
const char *PPRT_COLORSPC = "ColorSpace";
const char *PPRT_PATTERN = "Pattern";
const char *PPRT_SHADING = "Shading";
const char *PPRT_XOBJECT = "XObject";
const char *PPRT_FONT = "Font";
const char *PPRT_PROSET = "ProcSet";
const char *PPRT_PROPERTIES = "Properties";


//////////////////////////////////////////////////////////
//
//    PPElement
//
//////////////////////////////////////////////////////////

PPElement::PPElement(PPContext *gcontext)
{
    _gstate = gcontext->NewGState();
	_parentForm = NULL;
	_gflag = PPGF_NONE;
	_bounds = PPRect(0,0,0,0);
}

PPElement::PPElement(PPGState *gstate)
{
	_gstate = (PPGState *)gstate->Copy();
	_parentForm = NULL;
	_gflag = PPGF_NONE;
	_bounds = PPRect(0,0,0,0);

}

PPElement::PPElement()
{
	 _gstate = NULL;
	_parentForm = NULL;
	_gflag = PPGF_NONE;
	_bounds = PPRect(0,0,0,0);
}

PPElement::~PPElement()
{
    delete _gstate;
}

string PPElement::CommandString()
{
    string cmd_str = "";
	if(_gstate) {
		cmd_str = _gstate->MakeCommandString();
	}
	string cmd_wd = MakeCommandString();
    cmd_str += cmd_wd;
    return cmd_str;
}

void PPElement::WillAddToParent(PPFormBase *form)
{
    _parentForm = form;
    
}

void PPElement::CopyMembersTo(PPBase *obj)
{
	PPBase::CopyMembersTo(obj);
	PPElement *ret_el = (PPElement *)obj;

	if(_gstate)
		ret_el->_gstate = (PPGState *)_gstate->Copy();
    ret_el->_gflag = _gflag;
	ret_el->_bounds = _bounds;
}

bool PPElement::HasResource()
{
	if(_gstate){
		if(_gstate->_gflag & PPGF_DICTNAME) {
			return true;
		}
		else if(_gstate->_gflag & PPGF_COLORSPACE) {
			return true;
		}
	}
	
	return false;
}

/////////////////////////////////////////////////////  Multi Resource Handling
vector <const char *> PPElement::ResourceTypeList()
{
	vector <const char *> rsc_list;
	if(_gstate){
		if(_gstate->_gflag & PPGF_DICTNAME) {
			rsc_list.push_back(PPRT_EXTSTATE);
		}
		if(_gstate->_gflag & PPGF_COLORSPACE) {
			rsc_list.push_back(PPRT_COLORSPC);
		}
	}
	return rsc_list;
}

string PPElement::ResourceKeyFor(const char *rsc_type)
{
	string key;
	if(rsc_type == PPRT_EXTSTATE) {
		key = _gstate->DictName();
	}
	else if(rsc_type == PPRT_COLORSPC) {
		if(_gstate->_gflag & PPGF_STROKECOLORSPC) {
			key = _gstate->_strokeColor.UserColorSpaceName();
		}
		else if(_gstate->_gflag & PPGF_FILLCOLORSPC) {
			key = _gstate->_fillColor.UserColorSpaceName();
		}
	}
	return key;
}

int PPElement::ResourceObjNum(const char *rsc_type)
{
	PPTIndirectObj *rsc_obj = (PPTIndirectObj *)ResourceObjectFor(rsc_type);
	if(rsc_obj == NULL) {
		cout << " Resource Object is NULL." << PP_ENDL;
		return 0;
	}

	// 동시에 두개 이상의 도큐먼트를 열어놓고 작업 할때를 대비해서
	// ResourceObjNum 는 도큐먼트 ID 와 조합으로 만들어 낸다.
	int ret_num = _parentForm->_document->_docID << 24;
	ret_num += rsc_obj->_objNum;

	return ret_num;
}

PPToken *PPElement::ResourceObjectFor(const char *rsc_type)
{
	PPToken *ret_res;

	if(_parentForm == NULL) {
        cout << "No _parentForm..." << PP_ENDL;
		return NULL;
	}
    PPTIndirectRef *res_ref = (PPTIndirectRef *)_parentForm->ResourceForKey(rsc_type);
    if (!res_ref) {
        cout << "Resource IndirectRef for type '"<< rsc_type << "' not found..." << PP_ENDL;
        return NULL;
    }
	PPTDictionary *res_dict = NULL;
	if(res_ref->ClassType() == PPTN_INDIRECTREF) {
		res_dict = (PPTDictionary *)res_ref->ValueObject();
		if (!res_dict) {
			cout << "Resource Dictionary not found..." << PP_ENDL;
			return NULL;
		}
	}
	else if(res_ref->ClassType() == PPTN_DICTIONARY) {
		res_dict = (PPTDictionary *)res_ref;
	}

	string rsc_key = ResourceKeyFor(rsc_type);
    res_ref = (PPTIndirectRef *)res_dict->ObjectForKey(rsc_key);
    if (!res_ref) {
        cout << "Resource IndirectRef for key'" << rsc_key << "'not found..." << PP_ENDL;
        return NULL;
    }
	ret_res = (PPTIndirectObj *)res_ref->TargetObject();
    if (!ret_res) {
        cout << "Resource Resource Object not found..." << PP_ENDL;
        return NULL;
	}
	return ret_res;
}


PPDocument *PPElement::Document()
{
	return _parentForm->_document;
}


void PPElement::SetTransform(float a, float b, float c, float d, float x, float y)
{
	PPMatrix mtx(a, b, c, d, x, y); 
	_gstate->SetMatrix(mtx);
}

void PPElement::SetTransform(PPMatrix *mtx)
{
	_gstate->SetMatrix(*mtx);
}
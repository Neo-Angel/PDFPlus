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
    _gstate = gcontext->newGState();
}

PPElement::PPElement(PPGState *gstate)
{
	_gstate = (PPGState *)gstate->Copy();
}

PPElement::PPElement()
{
	 _gstate = NULL;

}

PPElement::~PPElement()
{
    delete _gstate;
}

string PPElement::commandString()
{
    string cmd_str = "";
	if(_gstate) {
		cmd_str = _gstate->makeCommandString();
	}
	string cmd_wd = makeCommandString();
    cmd_str += cmd_wd;
    return cmd_str;
}

void PPElement::willAddToParent(PPFormBase *form)
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

string PPElement::ResourceType()
{
	string type;
	if(_gstate) {
		if(_gstate->_gflag & PPGF_DICTNAME) {
			type = PPRT_EXTSTATE; 
		}
		else if(_gstate->_gflag & PPGF_COLORSPACE) {
			type = PPRT_COLORSPC; 
		}
	}
	return type;
}


string PPElement::ResourceKey()
{
	string key;
	if(_gstate) {
		if(_gstate->_gflag & PPGF_DICTNAME) {
			key = _gstate->dictName(); 
		}
		else if(_gstate->_gflag & PPGF_STROKECOLORSPC) {
			key = _gstate->_strokeColor.UserColorSpaceName();
		}
		else if(_gstate->_gflag & PPGF_FILLCOLORSPC) {
			key = _gstate->_fillColor.UserColorSpaceName();
		}
	}
	return key;
}
int PPElement::ResourceObjNum()
{
	string rsc_type = ResourceType();
	PPTDictionary *rsc_keys = (PPTDictionary *)_parentForm->_resourceDict->ObjectForKey(rsc_type);
	if(rsc_keys == NULL) {
		return 0;
	}

	string rsc_key = ResourceKey();
	PPTIndirectRef *rsc_ref = (PPTIndirectRef *)rsc_keys->ObjectForKey(rsc_key);
	if(rsc_ref == NULL) {
		return 0;
	}

	int ret_num = _parentForm->_document->_docID << 24;
	ret_num += rsc_ref->_objNum;

	return ret_num;
}

PPToken *PPElement::GetResource()
{
	string rsc_type = ResourceType();
	PPTDictionary *rsc_keys = (PPTDictionary *)_parentForm->_resourceDict->ObjectForKey(rsc_type);
	if(rsc_keys == NULL) {
		return 0;
	}

	string rsc_key = ResourceKey();
	PPTIndirectRef *rsc_ref = (PPTIndirectRef *)rsc_keys->ObjectForKey(rsc_key);
	if(rsc_ref == NULL) {
		return 0;
	}

	PPToken *rcs = rsc_ref->targetObject();
	return rcs;
}

/////////////////////////////////////////////////////  Multi Resource Handling
vector <const char *> PPElement::ResourceList()
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
		key = _gstate->dictName();
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
	PPTDictionary *rsc_keys = (PPTDictionary *)_parentForm->_resourceDict->ObjectForKey(rsc_type);
	if(rsc_keys == NULL) {
		return 0;
	}

	string rsc_key = ResourceKeyFor(rsc_type);
	PPTIndirectRef *rsc_ref = (PPTIndirectRef *)rsc_keys->ObjectForKey(rsc_key);
	if(rsc_ref == NULL) {
		return 0;
	}

	int ret_num = _parentForm->_document->_docID << 24;
	ret_num += rsc_ref->_objNum;

	return ret_num;
}

PPToken *PPElement::GetResource(const char *rsc_type)
{
	PPTDictionary *rsc_keys = (PPTDictionary *)_parentForm->_resourceDict->ObjectForKey(rsc_type);
	if(rsc_keys == NULL) {
		return 0;
	}

	string rsc_key = ResourceKeyFor(rsc_type);
	PPTIndirectRef *rsc_ref = (PPTIndirectRef *)rsc_keys->ObjectForKey(rsc_key);
	if(rsc_ref == NULL) {
		return 0;
	}

	PPToken *rcs = rsc_ref->targetObject();
	return rcs;
}

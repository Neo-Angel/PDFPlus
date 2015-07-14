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

#include <sstream>


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
			type = "ExtGState"; 
		}
		else if(_gstate->_gflag & PPGF_COLORSPACE) {
			type = "ColorSpace"; 
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

PPToken *PPElement::GetResource()
{
	PPToken *rcs = _parentForm->ResourceForKey(ResourceType(), ResourceKey());
	return rcs;
}



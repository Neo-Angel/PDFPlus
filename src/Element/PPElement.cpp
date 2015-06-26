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

PPElement::~PPElement()
{
    delete _gstate;
}

string PPElement::commandString()
{
    string cmd_str = _gstate->makeCommandString();
    cmd_str += makeCommandString();
    return cmd_str;
}

void PPElement::willAddToParent(PPFormBase *form)
{
    _parentForm = form;
    
}

void PPElement::CopyMembers(PPBase *obj)
{
	PPBase::CopyMembers(obj);
	PPElement *ret_el = (PPElement *)obj;

	ret_el->_gstate = (PPGState *)_gstate->Copy();
    ret_el->_gflag = _gflag;
	ret_el->_bounds = _bounds;
}




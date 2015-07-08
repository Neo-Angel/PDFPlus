//
//  PPBase.h
//  PDFPlusLib
//
//  Created by Neo Angel on 2014. 9. 11..
//  Copyright (c) 2014년 #CORP#. All rights reserved.
//

#ifndef __PDFPlusLib__PPBase__
#define __PDFPlusLib__PPBase__

#include <iostream>
#include "PPDefines.h"

using namespace std;

static const char	*PP_ENDL = "\x0a";
static const char	*PPTN_BASE = "PPBase";

class PPBase {
public:
    virtual ~PPBase();

template<class T>
     bool isKindOfClass();
    
    virtual string description();
    virtual string typeName();   // deprecated
    virtual const char *classType();
	virtual PPBase *Create() {return new PPBase();}
	virtual PPBase *Copy();
	virtual void CopyMembersTo(PPBase *);
};


// Util Fuctions

string stringFromInt(int n);
string *newStringFromInt(int n); // needs to delete

string stringFromFloat(float n);
string *newStringFromFloat(float n); // needs to delete



#endif /* defined(__PDFPlusLib__PPBase__) */


#include "PPTName.h"


// PPTName //////////////////////////////////
PPTName::PPTName(PPParser *parser, string *str):PPToken(parser)
{
    _name = str;
}
PPTName::~PPTName()
{
    delete _name;
}
//string *PPTName::getName()
//{
//    return _name;
//}

string PPTName::description()
{
    return *_name;
}

string PPTName::xmlString(int level)
{
    string retstr;
    retstr += tabStr(level) + "<Name>" +*_name + "</Name>\xa";
    return retstr;
}


string PPTName::pdfString()
{
    string retstr = "/";
	int i, icnt = _name->length();
	for(i=0;i<icnt;i++) {
		unsigned short ch = (*_name)[i];
		if(ch == ' ') {
			retstr += "#20";
		}
		else {
			retstr += ch;
		}
	}
    return retstr;
}


void PPTName::CopyMembersTo(PPBase *obj) 
{
	PPToken::CopyMembersTo(obj);

	PPTName *name = (PPTName *)obj;
	name->_name = new string(*_name);
}

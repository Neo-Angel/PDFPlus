
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
    retstr += tapStr(level) + "<Name>" +*_name + "</Name>\xa";
    return retstr;
}


string PPTName::pdfString()
{
    string retstr = "/";
    retstr += *_name;
    return retstr;
}



#include "PPTString.h"



//  ////////////////////////////////// PPTString

PPTString::PPTString(PPParser *parser, string *str):PPToken(parser)
{
    _string = str;
}

PPTString::~PPTString()
{
    delete _string;
}

string PPTString::utf8String()
{
    string retstr;
    PPstringToUTF8String(retstr, *_string);
    return retstr;
}

string PPTString::xmlString(int level)
{
    string retstr;
    wstring wstr;
    stringToWString(wstr, *_string);
    string utf8_str = PPwstrToUtf8(wstr);
    retstr += tapStr(level) + "<String><![CDATA[" + utf8_str + "]]></String>\xa";
    return retstr;
}
string PPTString::pdfString()
{
    string retstr = "(";
    retstr += *_string;
    retstr += ")";
    return retstr;
}

void PPTString::CopyMembersTo(PPBase *obj)
{
	PPToken::CopyMembersTo(obj);

	PPTString *str = (PPTString *)obj;
	str->_string = new string(*_string);
}
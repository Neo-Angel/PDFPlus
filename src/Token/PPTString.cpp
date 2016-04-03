

#include "PPTString.h"



//  ////////////////////////////////// PPTString

PPTString::PPTString(PPDocument *doc, string *str):PPToken(doc)
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
    PPstringToUTF8String(*_string, retstr);
    return retstr;
}

string PPTString::XMLString(int level)
{
    string retstr;
    wstring wstr;
    PPstringToWString(*_string, wstr);
    string utf8_str = PPwstrToUtf8(wstr);
    retstr += PPTabStr(level) + "<String><![CDATA[" + utf8_str + "]]></String>\xa";
    return retstr;
}
string PPTString::pdfString()
{
    string retstr = "<";
	retstr += PPToHexStr(*_string);
    retstr += ">";
    return retstr;

	/*
    string retstr = "(";
    retstr += *_string;
    retstr += ")";
    return retstr;
	*/
}

void PPTString::CopyMembersTo(PPBase *obj)
{
	PPToken::CopyMembersTo(obj);

	PPTString *str = (PPTString *)obj;
	str->_string = new string(*_string);
}


void PPTString::AppendString(PPTString *str)
{
	_string->append(*str->_string);
}

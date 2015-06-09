

#include "PPTComment.h"




//  ////////////////////////////////// PPTComment
PPTComment::PPTComment(PPParser *parser, string *str) : PPToken(parser)
{
    _comment = str;
}

PPTComment::~PPTComment()
{
    delete _comment;
}

string PPTComment::xmlString(int level)
{
    string retstr;
    //    retstr += tapStr(level) + "<String><![CDATA[" +*_string + "]]></String>\xa";
//    string nom_str = toNomalASCIIString(*_comment);
    
    wstring wstr;
    stringToWString(wstr, *_comment);
    string utf8_str = PPwstrToUtf8(wstr);
    
    retstr += tapStr(level) + "<Comment>" +utf8_str + "</Comment>\xa";
    //    retstr += tapStr(level) + "<String>" +"*_string" + "</String>\xa";
    return retstr;
}

string PPTComment::pdfString()
{
    string retstr = "%";
    retstr += *_comment;
    return retstr;
    
}


#include "PPTComment.h"




//  ////////////////////////////////// PPTComment
PPTComment::PPTComment(PPDocument *doc, string *str) : PPToken(doc)
{
    _comment = str;
}

PPTComment::~PPTComment()
{
    delete _comment;
}

string PPTComment::XMLString(int level)
{
    string retstr;
    
    wstring wstr;
    PPstringToWString(*_comment, wstr);
    string utf8_str = PPwstrToUtf8(wstr);
    
    retstr += PPTabStr(level) + "<Comment>" +utf8_str + "</Comment>\xa";
    //    retstr += PPTabStr(level) + "<String>" +"*_string" + "</String>\xa";
    return retstr;
}

string PPTComment::pdfString()
{
    string retstr = "%";
    retstr += *_comment;
    return retstr;
    
}

void PPTComment::CopyMembersTo(PPBase *obj) 
{
	PPToken::CopyMembersTo(obj);

	PPTComment *comment = (PPTComment *)obj;
	comment->_comment = new string(*_comment);

//	string *comment = (PPTName *)obj;
//	_comment = new string(*comment->_name);
}
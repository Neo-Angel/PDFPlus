
#include "PPTBool.h"



//  ////////////////////////////////// PPTBool
string boolStr(bool flag)
{
    if (flag) {
        return "true";
    }
    return "false";
}

PPTBool::PPTBool(PPDocument *doc, string &str):PPToken(doc)
{
    _bool = (str.compare("true") == 0);
}

PPTBool::PPTBool()
{
	_bool = false;
}

string PPTBool::XMLString(int level)
{
    string retstr;
    retstr += PPTabStr(level) + "<Bool>" + boolStr(_bool) + "</Bool>\xa";
    return retstr;
}

string PPTBool::PDFString()
{
    return boolStr(_bool);
}

void PPTBool::CopyMembersTo(PPBase *obj) 
{
	PPToken::CopyMembersTo(obj);

	PPTBool *bool_obj =(PPTBool *)obj;
	bool_obj->_bool = _bool;
}
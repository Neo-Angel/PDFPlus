
#include "PPTBool.h"



//  ////////////////////////////////// PPTBool
string boolStr(bool flag)
{
    if (flag) {
        return "true";
    }
    return "false";
}

PPTBool::PPTBool(PPParser *parser, string &str):PPToken(parser)
{
    _bool = (str.compare("true") == 0);
}

string PPTBool::xmlString(int level)
{
    string retstr;
    retstr += tapStr(level) + "<Bool>" + boolStr(_bool) + "</Bool>\xa";
    return retstr;
}

string PPTBool::pdfString()
{
    return boolStr(_bool);
}


#include "PPTNull.h"


//  ////////////////////////////////// PPTNull
string PPTNull::xmlString(int level)
{
    string retstr = tabStr(level) + "<Null/>";
    return retstr;
}

string PPTNull::pdfString() {
    return "null";
}

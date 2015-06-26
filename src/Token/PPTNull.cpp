
#include "PPTNull.h"


//  ////////////////////////////////// PPTNull
string PPTNull::xmlString(int level)
{
    string retstr = tapStr(level) + "<Null/>";
    return retstr;
}

string PPTNull::pdfString() {
    return "null";
}

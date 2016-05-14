
#include "PPTNull.h"


//  ////////////////////////////////// PPTNull
string PPTNull::XMLString(int level)
{
    string retstr = PPTabStr(level) + "<Null/>";
    return retstr;
}

string PPTNull::PDFString() {
    return "null";
}

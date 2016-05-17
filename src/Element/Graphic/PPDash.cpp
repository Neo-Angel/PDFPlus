
#include "PPDash.h"
#include "PPToken.h"
#include "PPTNumber.h"
#include "PPTArray.h"

string PPDash::PDFString()
{
    string retstr;
    retstr = _array->PDFString() + " " + _phase->PDFString();
    return retstr;
}


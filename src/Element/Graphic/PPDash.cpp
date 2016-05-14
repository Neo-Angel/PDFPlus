
#include "PPDash.h"
#include "PPToken.h"
#include "PPTNumber.h"
#include "PPTArray.h"

string PPDash::pdfString()
{
    string retstr;
    retstr = _array->PDFString() + " " + _phase->PDFString();
    return retstr;
}


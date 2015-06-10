
#include "PPDash.h"
#include "PPToken.h"
#include "PPTNumber.h"
#include "PPTArray.h"

string PPDash::pdfString()
{
    string retstr;
    retstr = _array->pdfString() + " " + _phase->pdfString();
    return retstr;
}


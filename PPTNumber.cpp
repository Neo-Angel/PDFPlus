
#include "PPTNumber.h"



// PPTNumber //////////////////////////////////
PPTNumber::PPTNumber(PPParser *parser, string *str):PPToken(parser)
{
    _numstr = str;
    _number = stod(*str);
}

PPTNumber::PPTNumber(PPParser *parser, int num)
{
    _numstr = newStringFromInt(num);
    _number = (double)num;
}

PPTNumber::PPTNumber(PPParser *parser, float num)
{
    _numstr = newStringFromFloat(num);
    _number = (double)num;
}
PPTNumber::~PPTNumber()
{
    delete _numstr;
}

string PPTNumber::stringValue()
{
    return *_numstr;
    //
    //    ostringstream ostr;
    //    ostr << _number;
    //    string retstr = ostr.str();
    //    return retstr;
}

float PPTNumber::floatValue()
{
    return (float)_number;
}

int PPTNumber::intValue()
{
    return (int)_number;
}

long long PPTNumber::longlongValue()
{
    return (long long)_number;
}

long PPTNumber::longValue()
{
    return (long)_number;
}

string PPTNumber::xmlString(int level)
{
    string retstr;
    retstr += tapStr(level) + "<Number>" +stringValue() + "</Number>\xa";
    return retstr;
}
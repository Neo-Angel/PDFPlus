
#include "PPTNumber.h"



// PPTNumber //////////////////////////////////
PPTNumber::PPTNumber(PPDocument *doc, string *str):PPToken(doc)
{
    _numstr = str;
	long double v = stod(*str);;
	long long i = (long long)(v * 10000);
	v = i / 10000.0;
    _number = (double)v;

}

PPTNumber::PPTNumber(PPDocument *doc, int num)
{
    _numstr = PPNewStringFromInt(num);
    _number = (double)num;
}

PPTNumber::PPTNumber(PPDocument *doc, uint num)
{
    _numstr = PPNewStringFromInt(num);
    _number = (double)num;
}

PPTNumber::PPTNumber(PPDocument *doc, float num)
{
    _numstr = PPNewStringFromFloat(num);
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

string PPTNumber::XMLString(int level)
{
    string retstr;
    retstr += PPTabStr(level) + "<Number>" +stringValue() + "</Number>\xa";
    return retstr;
}

void PPTNumber::CopyMembersTo(PPBase *obj)
{
	PPToken::CopyMembersTo(obj);

	PPTNumber *num = (PPTNumber *)obj;
	num->_numstr = new string(*_numstr);
	num->_number = _number;
}
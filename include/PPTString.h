

#ifndef __PDFPlusLib__PPTString__
#define __PDFPlusLib__PPTString__

#include <PPToken.h>



///////////////////////////////////////// PPTString
class PPTString : public PPToken {
public:
    string *_string;

	PPTString() {_string = NULL;}
    PPTString(PPDocument *doc, string *str);
    ~PPTString();
    string XMLString(int level);
    string utf8String();
    inline PPClassType ClassType(){return PPTN_STRING;};
    string PDFString();

	PPBase *Create() {return new PPTString();}
	void CopyMembersTo(PPBase *obj);

	bool IsEqualToString(string str);
	void AppendString(PPTString *str);

};
/////////////////////////////////////////


#endif

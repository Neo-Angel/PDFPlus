

#ifndef __PDFPlusLib__PPTString__
#define __PDFPlusLib__PPTString__

#include <PPToken.h>



///////////////////////////////////////// PPTString
class PPTString : public PPToken {
public:
    string *_string;

	PPTString() {_string = NULL;}
    PPTString(PPParser *parser, string *str);
    ~PPTString();
    string xmlString(int level);
    string utf8String();
    inline const char *classType(){return PPTN_STRING;};
    string pdfString();

	PPBase *Create() {return new PPTString();}
	void CopyMembersTo(PPBase *obj);

	void AppendString(PPTString *str);

};
/////////////////////////////////////////


#endif

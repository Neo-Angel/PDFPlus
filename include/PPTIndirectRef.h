
#ifndef __PDFPlusLib__PPTIndirectRef__
#define __PDFPlusLib__PPTIndirectRef__

#include <PPToken.h>



///////////////////////////////////////// PPTIndirectRef

class PPTIndirectObj;
class PPTIndirectRef : public PPToken {
public:
    int _objNum;
    int _genNum;
    
	PPTIndirectRef(){_objNum = 0; _genNum = 0;}
    PPTIndirectRef(PPParser *parser, int num1, int num2);
    string description();

    string xmlString(int level);
    string pdfString();
    string internalXmlString(int level);
    PPTIndirectObj *targetObject();
    PPToken *valueObject();

	PPBase *Create() {return new PPTIndirectRef();}
	void CopyMembersTo(PPBase *obj) ;

    inline const char *classType(){return PPTN_INDIRECTREF;};
};
/////////////////////////////////////////

#endif

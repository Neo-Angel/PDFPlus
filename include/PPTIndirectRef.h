
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
    string Description();

    string XMLString(int level);
    string pdfString();
    PPTIndirectObj *targetObject();
    PPToken *valueObject();

	PPBase *Create() {return new PPTIndirectRef();}
	void CopyMembersTo(PPBase *obj) ;

    inline const char *ClassType(){return PPTN_INDIRECTREF;};

	void MoveInto(PPDocument *doc);
};
/////////////////////////////////////////

#endif

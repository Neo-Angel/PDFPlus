
#ifndef __PDFPlusLib__PPEMarkedContent__
#define __PDFPlusLib__PPEMarkedContent__

#include <PPElement.h>


//
//      PPEMarkedContent
//
///////////////////////////////////////////////////////////////
class PPEMarkedContent : public PPElement {
    PPTDictionary *_property;
    PPTName * _tag;
    
public:
    PPEMarkedContent(PPTName *tag, PPTDictionary *property, PPContext *gcontext);
	PPEMarkedContent();
	PPBase *Create(){return new PPEMarkedContent;}

	void CopyMembersTo(PPBase *);
	void SetParser(PPParser *parser);

    string makeCommandString();
    string xmlString(int level);
	PPElementType getType() {return PPET_MARKED_CONTENT;}
};


#endif

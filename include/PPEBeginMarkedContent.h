
#ifndef __PDFPlusLib__PPEBeginMarkedContent__
#define __PDFPlusLib__PPEBeginMarkedContent__

#include <PPElement.h>



//
//      PPEBeginMarkedContent
//
///////////////////////////////////////////////////////////////
class PPEBeginMarkedContent : public PPElement {
    PPTDictionary *_property;
    PPTName * _tag;
    
public:
    PPEBeginMarkedContent(PPTName *tag, PPTDictionary *property, PPContext *gcontext);
	PPEBeginMarkedContent();
	PPBase *Create(){return new PPEBeginMarkedContent;}

	void CopyMembersTo(PPBase *);
	void SetParser(PPParser *parser);

    string makeCommandString();
    string xmlString(int level);
	PPElementType getType() {return PPET_BEGIN_MARKED_CONTENT;}
};


#endif


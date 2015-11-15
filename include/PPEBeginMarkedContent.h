


#ifndef __PDFPlusLib__PPEBeginMarkedContent__
#define __PDFPlusLib__PPEBeginMarkedContent__

#include <PPElement.h>

//
//      PPEBeginMarkedContent
//
///////////////////////////////////////////////////////////////
class PPEBeginMarkedContent : public PPElement {
    PPToken *_properties;
    PPTName * _tag;
    PPTIndirectObj *_OCGInfo;
public:
    PPEBeginMarkedContent(PPTName *tag, PPToken *properties, PPContext *gcontext);
    PPEBeginMarkedContent(PPParser *parser, string properties, PPContext *gcontext);

	PPEBeginMarkedContent();
	~PPEBeginMarkedContent();
	PPBase *Create(){return new PPEBeginMarkedContent;}

	void CopyMembersTo(PPBase *);
	void SetParser(PPParser *parser);

    string makeCommandString();
    string xmlString(int level);
	PPElementType getType() {return PPET_BEGIN_MARKED_CONTENT;}
	void willAddToParent(PPFormBase *form);

	bool HasResource();
	vector <const char *> ResourceTypeList();
	string ResourceKeyFor(const char *rsc_type);
	PPToken *GetResource(const char *rsc_type);
};


#endif


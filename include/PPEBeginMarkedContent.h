


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
    PPEBeginMarkedContent(PPDocument *doc, string properties, PPContext *gcontext);

	PPEBeginMarkedContent();
	~PPEBeginMarkedContent();
	PPBase *Create(){return new PPEBeginMarkedContent;}

	void CopyMembersTo(PPBase *);
	void SetDocument(PPDocument *doc);

    string MakeCommandString();
    string XMLString(int Slevel);
	PPElementType Type() {return PPET_BEGIN_MARKED_CONTENT;}
	void WillAddToParent(PPFormBase *form);

	bool HasResource();
	vector <const char *> ResourceTypeList();
	string ResourceKeyFor(const char *rsc_type);
	PPToken *ResourceObjectFor(const char *rsc_type);
};


#endif


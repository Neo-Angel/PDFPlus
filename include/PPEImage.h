
#ifndef __PDFPlusLib__PPEImage__
#define __PDFPlusLib__PPEImage__

#include <PPElement.h>





//
//      PPEImage
//
///////////////////////////////////////////////////////////////
class PPTName;
class PPTDictionary;
class PPTStream;

class PPEImage : public PPElement {
public:
    PPTName *_name;
	PPTIndirectObj *_xobj;
	string _subtype; // "Form", "Image"
//    PPTDictionary *_dict;
//    PPTStream *_stream;
    
public:
	PPEImage(PPTName *name, PPContext *gcontext);
    PPEImage(PPTDictionary *dict, PPContext *gcontext);
	PPEImage();
	PPBase *Create(){return new PPEImage;}

	void CopyMembersTo(PPBase *);
	void SetParser(PPParser *parser);

    string makeCommandString();
    string xmlString(int level);
	PPElementType getType() {return PPET_IMAGE;}

	void willAddToParent(PPFormBase *form);

	bool HasResource();
	string ResourceType();
	string ResourceKey();
	PPToken *GetResource();

	vector <const char *> ResourceList();
	string ResourceKeyFor(const char *rsc_type);
	PPToken *GetResource(const char *rsc_type);

};




#endif
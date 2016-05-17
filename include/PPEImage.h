
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
class PPImage;

class PPEImage : public PPElement {
public:
    string _name;
	PPTIndirectObj *_xobj;
	string _subtype; // "Form", "Image"
	PPImage *_image;
	char *_image_path;
    
public:
	PPEImage(string name, PPContext *gcontext);
	PPEImage(PPTName *name, PPContext *gcontext);
	PPEImage(char *image_path, PPContext *gcontext);
    PPEImage(PPTDictionary *dict, PPContext *gcontext);
	PPEImage();
	PPBase *Create(){return new PPEImage;}

	void CopyMembersTo(PPBase *);
	void SetDocument(PPDocument *doc);

    string MakeCommandString();
    string XMLString(int level);
	PPElementType Type() {return PPET_IMAGE;}

	void WillAddToParent(PPFormBase *form);

	bool HasResource();

	vector <const char *> ResourceTypeList();
	string ResourceKeyFor(const char *rsc_type);
	PPToken *ResourceObjectFor(const char *rsc_type);

};




#endif
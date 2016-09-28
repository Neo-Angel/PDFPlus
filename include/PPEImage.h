
#ifndef __PDFPlusLib__PPEImage__
#define __PDFPlusLib__PPEImage__

#include <PPElement.h>
#include <PPImage.h>

//
//      PPEImage
//
///////////////////////////////////////////////////////////////
class PPTName;
class PPTDictionary;
class PPTStream;


class PPEImage : public PPElement {
public:
    string _name;
	PPTIndirectObj *_xobj;
	string _subtype; // "Form", "Image"
	PPImage *_image;
	string _image_path;
    
public:
	PPEImage(string image_path, PPContext *gcontext);
	PPEImage(PPTName *name, PPContext *gcontext);
	PPEImage(char *image_path, PPContext *gcontext);
    PPEImage(PPTDictionary *dict, PPContext *gcontext);
	PPEImage(string image_path, PPFormBase *form);
	PPEImage();
	PPBase *Create(){return new PPEImage;}

	void CopyMembersTo(PPBase *);
	void SetDocument(PPDocument *doc);

    string MakeCommandString();
    string XMLString(int level);
	PPElementType Type() {return PPET_IMAGE;}

	void WillAddToParent(PPFormBase *form);
	void SetImagePath(string new_path);
	bool HasResource();

	vector <const char *> ResourceTypeList();
	string ResourceKeyFor(const char *rsc_type);
	PPToken *ResourceObjectFor(const char *rsc_type);

	float ImageWidth(){return _image->ImageWidth();}
	float ImageHeight() {return _image->ImageHeight();}

};




#endif
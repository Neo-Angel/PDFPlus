
#ifndef __PDFPlusLib__PPEShading__
#define __PDFPlusLib__PPEShading__

#include <PPElement.h>





//
//      PPEShade
//
///////////////////////////////////////////////////////////////
class PPEShading : public PPElement {
public:
    PPTName * _name;
//    PPTDictionary *_dict;
	PPTIndirectObj *_sh_res;
    
public:
    PPEShading(PPContext *gcontext);
	PPEShading();
	PPBase *Create(){return new PPEShading;}

	void CopyMembersTo(PPBase *);
	void SetDocument(PPDocument *doc);

    string MakeCommandString();
    string XMLString(int level);
    void WillAddToParent(PPFormBase *form);
	PPElementType Type() {return PPET_SHADING;}

	bool HasResource();

	vector <const char *> ResourceTypeList();
	string ResourceKeyFor(const char *rsc_type);
	PPToken *ResourceObjectFor(const char *rsc_type);
};



#endif
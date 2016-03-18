
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
	void SetParser(PPParser *parser);

    string makeCommandString();
    string XMLString(int level);
    void willAddToParent(PPFormBase *form);
	PPElementType getType() {return PPET_SHADING;}

	bool HasResource();

	vector <const char *> ResourceTypeList();
	string ResourceKeyFor(const char *rsc_type);
	PPToken *GetResource(const char *rsc_type);
};



#endif
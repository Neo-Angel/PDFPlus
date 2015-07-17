
#ifndef __PDFPlusLib__PPEForm__
#define __PDFPlusLib__PPEForm__

#include <PPElement.h>


//
//      PPEForm
//
///////////////////////////////////////////////////////////////
class PPPage;
class PPDocument;
class PPTName;
class PPTDictionary;

class PPEForm : public PPElement {  // , public PPFormBase {
public:
    PPTName *_name;
	PPTIndirectObj *_xobj;
	string _subtype; // "Form", "Image"
//    PPTDictionary *_dict;
//    PPTStream *_stream;
    
public:
//	PPEForm(PPEForm *form);
	PPEForm(PPPage *page, PPDocument *doc);
//    PPEForm(PPTDictionary *dict, PPContext *gcontext);
    PPEForm(PPTName *name, PPContext *gcontext); //  PPFormBase::BuildElement() 에서 쓰임.
	PPEForm();
	PPBase *Create(){return new PPEForm;}

	void CopyMembersTo(PPBase *);
	void SetParser(PPParser *parser);

//    void setStream(PPTStream *stream);
    string makeCommandString();
    string xmlString(int level);
	PPTIndirectObj *GetXObject();
	PPFormBase *GetFormObj();
	PPElementType getType() {return PPET_FORM;}

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

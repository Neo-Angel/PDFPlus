
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
	void SetDocument(PPDocument *doc);

//    void setStream(PPTStream *stream);
    string makeCommandString();
    string XMLString(int level);
	PPTIndirectObj *XObject();
	PPFormBase *GetFormObj();
	PPElementType getType() {return PPET_FORM;}

	void WillAddToParent(PPFormBase *form);

	bool HasResource();

	vector <const char *> ResourceTypeList();
	string ResourceKeyFor(const char *rsc_type);
	PPToken *GetResource(const char *rsc_type);

};


#endif

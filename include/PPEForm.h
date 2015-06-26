
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
    PPTDictionary *_dict;
//    PPTStream *_stream;
    
public:
//	PPEForm(PPEForm *form);
	PPEForm(PPPage *page, PPDocument *doc);
    PPEForm(PPTDictionary *dict, PPContext *gcontext);
    PPEForm(PPTName *name, PPContext *gcontext);
	PPEForm(){}
	PPBase *Create(){return new PPEForm;}

	void CopyMembers(PPBase *);
	void SetParser(PPParser *parser);

//    void setStream(PPTStream *stream);
    string makeCommandString();
    string xmlString(int level);
	PPTIndirectObj *GetXObject();
	PPFormBase *GetFormObj();
	PPElementType getType() {return PPET_FORM;}

};


#endif

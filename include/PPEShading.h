
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
    PPTDictionary *_dict;
    
public:
    PPEShading(PPContext *gcontext) : PPElement(gcontext){_dict = NULL;_name = NULL;}
	PPEShading(){}
	PPBase *Create(){return new PPEShading;}

	void CopyMembers(PPBase *);
	void SetParser(PPParser *parser);

    string makeCommandString();
    string xmlString(int level);
    void willAddToParent(PPFormBase *form);
	PPElementType getType() {return PPET_SHADING;}
};



#endif
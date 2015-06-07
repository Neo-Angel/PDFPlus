
#ifndef __PDFPlusLib__PPEImage__
#define __PDFPlusLib__PPEImage__

#include <PPElement.h>





//
//      PPEImage
//
///////////////////////////////////////////////////////////////
class PPEImage : public PPElement {
public:
    PPTName *_name;
    PPTDictionary *_dict;
    PPTStream *_stream;
    
public:
    PPEImage(PPTDictionary *dict, PPContext *gcontext) : PPElement(gcontext){_dict = dict;}
	PPEImage(){}
	PPBase *Create(){return new PPEImage;}

	void CopyMembers(PPBase *);
	void SetParser(PPParser *parser);

    string makeCommandString();
    string xmlString(int level);
	PPElementType getType() {return PPET_IMAGE;}
};




#endif
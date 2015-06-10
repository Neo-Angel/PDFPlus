
#ifndef __PDFPlusLib__PPTIndirectRef__
#define __PDFPlusLib__PPTIndirectRef__

#include <PPToken.h>



///////////////////////////////////////// PPTIndirectRef

class PPTIndirectObj;
class PPTIndirectRef : public PPToken {
public:
    int _objNum;
    int _genNum;
    
    PPTIndirectRef(PPParser *parser, int num1, int num2);
    string description();

    string xmlString(int level);
    string pdfString();
    string internalXmlString(int level);
    PPTIndirectObj *targetObject();
    PPToken *valueObject();

    inline const char *classType(){return PPTN_INDIRECTREF;};
};
/////////////////////////////////////////

#endif

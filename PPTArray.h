
#ifndef __PDFPlusLib__PPTArray__
#define __PDFPlusLib__PPTArray__

#include <PPToken.h>



///////////////////////////////////////// PPTArray
class PPTArray : public PPToken {
public:
    vector <PPToken *> _array;
    
	PPTArray(PPParser *parser) : PPToken(parser) {};
    PPTArray(PPParser *parser, vector<PPToken *> token_list);
    ~PPTArray();
    string internalXmlString(int level);
    string xmlString(int level);
    inline size_t size() {return _array.size();}
    inline PPToken *objectAtIndex(int idx) {return _array.at(idx);}
    inline const char *classType() {return PPTN_ARRAY;}
    string pdfString();
	void AddToken(PPToken *token) {_array.push_back(token);}
	void AddToken(int num);

};
/////////////////////////////////////////

#endif

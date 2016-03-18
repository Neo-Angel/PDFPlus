
#ifndef __PDFPlusLib__PPTArray__
#define __PDFPlusLib__PPTArray__

#include <PPToken.h>



///////////////////////////////////////// PPTArray
class PPTArray : public PPToken {
public:
    vector <PPToken *> _array;
    
	PPTArray(PPParser *parser) : PPToken(parser) {};
    PPTArray(PPParser *parser, vector<PPToken *> token_list);
	PPTArray();
    ~PPTArray();

    string XMLString(int level);
    inline size_t size() {return _array.size();}
    inline PPToken *objectAtIndex(int idx) {return _array.at(idx);}
    inline const char *ClassType() {return PPTN_ARRAY;}
    string pdfString();
	void AddToken(PPToken *token) {_array.push_back(token);}
	void AddToken(int num);
	void Reorder(int from_idx, int to_idx);
	void RemoveAtIndex(int idx);

	PPBase *Create() {return new PPTArray;}
	void CopyMembersTo(PPBase *obj) ;
	void SetParser(PPParser *parser);

	void MoveInto(PPDocument *doc);

};
/////////////////////////////////////////

#endif


#ifndef __PDFPlusLib__PPTXRef__
#define __PDFPlusLib__PPTXRef__

#include <PPToken.h>



///////////////////////////////////////// PPTXRef
typedef struct {
    unsigned long long  offset;
    unsigned int        generation;
    char                type;
} XRefItem;

class PPTTrailer;

class PPTXRef : public PPToken {
public:
    map <unsigned int, XRefItem *> _xreflist;
    PPTTrailer *_trailerObj;

	PPTXRef() {_trailerObj = NULL;}
    PPTXRef(PPParser *parser);
    ~PPTXRef();
    void addXRef(unsigned int objnum, unsigned long long offset, int gennum, char type);
    size_t numberOfItems();
    string XMLString(int level);
    string pdfString();
    inline const char *ClassType() {return PPTN_XREF;}
    void Write(std::ostream &os);
    
    void merge(PPTXRef *trailer);
    
	PPBase *Create() {return new PPTXRef();}
	void CopyMembersTo(PPBase *obj);

};
/////////////////////////////////////////

#endif

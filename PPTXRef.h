
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

    PPTXRef(PPParser *parser);
    ~PPTXRef();
    void addXRef(unsigned int objnum, unsigned long long offset, int gennum, char type);
    size_t numberOfItems();
    string xmlString(int level);
    string pdfString();
    inline const char *classType() {return PPTN_XREF;}
    void write(std::ostream &os);
    
    void merge(PPTXRef *trailer);
    
};
/////////////////////////////////////////

#endif


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


// ������Ʈ���� ���ϻ��� ��ġ ������ ���� ������ ������ �ִ�.
class PPTXRef : public PPToken {
public:
    map <unsigned int, XRefItem *> _xreflist;
    PPTTrailer *_trailerObj;

	PPTXRef() {_trailerObj = NULL;}
    PPTXRef(PPDocument *doc);
    ~PPTXRef();

    string XMLString(int level);
    string PDFString();
    inline const char *ClassType() {return PPTN_XREF;}
    void Write(std::ostream &os);
    
	PPBase *Create() {return new PPTXRef();}
	void CopyMembersTo(PPBase *obj);

    void AddXRef(unsigned int objnum, unsigned long long offset, int gennum, char type);
    size_t NumberOfItems();
    void Merge(PPTXRef *trailer);
    
};
/////////////////////////////////////////

#endif

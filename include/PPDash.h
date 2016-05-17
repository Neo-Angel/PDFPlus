
#ifndef __PDFPlusLib__PPDash__
#define __PDFPlusLib__PPDash__

#include <PPBase.h>


class PPTArray;
class PPTNumber;

//
//      PPDash
//
///////////////////////////////////////////////////////////////
class PPDash : public PPBase {
    
public:
    PPTArray *_array;
    PPTNumber *_phase;

public:
    ~PPDash();
    string PDFString();
};




#endif

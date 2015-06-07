
#ifndef __PDFPlusLib__PPContext__
#define __PDFPlusLib__PPContext__

#include <stack>
#include <PPBase.h>
#include <PPGState.h>

//
//      PPContext
//
///////////////////////////////////////////////////////////////
class PPContext : public PPGState {
    
public:
    stack <PPGState *> _gstats;

	PPContext();
//    PPContext();
    PPGState *newGState();
    void saveGState();
    void restoreGState();
    void setGState(PPGState *gstate);
    
};


#endif
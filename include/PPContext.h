
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

public:
	PPContext();

	PPGState *NewGState(); // == Copy();
    void SaveGState();
    void RestoreGState();
    void SetGState(PPGState *gstate);
    
};


#endif
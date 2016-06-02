
#ifndef __PDFPlusLib__PPContext__
#define __PDFPlusLib__PPContext__

#include <stack>
#include <PPBase.h>
#include <PPGState.h>

//
//      PPContext
//
///////////////////////////////////////////////////////////////

// PPContext 는 PPGState의 서브클래스로 기본적으로 자기 자신의 값을 가지며
// 스택인 _gstats 에 현재값을 push 해서 저장하거나, pop해서 현재값을 대치시킨다.
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
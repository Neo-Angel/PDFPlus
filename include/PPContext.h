
#ifndef __PDFPlusLib__PPContext__
#define __PDFPlusLib__PPContext__

#include <stack>
#include <PPBase.h>
#include <PPGState.h>

//
//      PPContext
//
///////////////////////////////////////////////////////////////
class PPFormBase;
// PPContext �� PPGState�� ����Ŭ������ �⺻������ �ڱ� �ڽ��� ���� ������
// ������ _gstats �� ���簪�� push �ؼ� �����ϰų�, pop�ؼ� ���簪�� ��ġ��Ų��.
class PPContext : public PPGState {
public:
	PPFormBase *_parentForm;
    stack <PPGState *> _gstats;

public:
	PPContext(PPFormBase *form);

	PPGState *NewGState(); // == Copy();
    void SaveGState();
    void RestoreGState();
    void SetGState(PPGState *gstate);
    
};


#endif



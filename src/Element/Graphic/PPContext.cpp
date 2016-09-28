
#include "PPContext.h"


//////////////////////////////////////////////////////////
//
//    PPContext
//
//////////////////////////////////////////////////////////
// =========================================================== PPContext - B

PPContext::PPContext(PPFormBase *form)
{
	_parentForm = form;
}

PPGState *PPContext::NewGState()
{
	PPGState *ret_gstate = (PPGState *)Copy();
    return ret_gstate;
}

// ������ gstate ������ ��ġ��Ų��.
void PPContext::SetGState(PPGState *gstate)
{
    _lineWidth = gstate->LineWidth();
    _intent = gstate->Intent();
    _flatness = gstate->Flatness();
    _miterLimit = gstate->MiterLimit();
    _lineCap = gstate->LineCap();
    _lineJoin = gstate->LineJoin();
    _dictName = gstate->DictName();
    _dash = gstate->Dash();
    _matrix = gstate->Matrix();
    _strokeColor = gstate->StrokeColor();
    _fillColor = gstate->FillColor();
}

// ������ gstate���� ������ ���ÿ� �����Ѵ�.
void PPContext::SaveGState()
{
    PPGState *new_gstate = NewGState();
    _gstats.push(new_gstate);
}

// ������ ���� ������ ������ ���� ��ġ��Ų��.
void PPContext::RestoreGState()
{
    PPGState *new_gstate = _gstats.top();
    SetGState(new_gstate);
    _gstats.pop();
    delete new_gstate;
}

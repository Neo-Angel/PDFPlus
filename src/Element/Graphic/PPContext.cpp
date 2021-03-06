
#include "PPContext.h"


//////////////////////////////////////////////////////////
//
//    PPContext
//
//////////////////////////////////////////////////////////
// =========================================================== PPContext - B

PPContext::PPContext(PPFormBase *form):PPGState()
{
	_parentForm = form;
}

PPGState *PPContext::NewGState()
{
	PPGState *ret_gstate = (PPGState *)Copy();
    return ret_gstate;
}

// 현재의 gstate 값들을 대치시킨다.
void PPContext::SetGState(PPGState *gstate)
{
    _lineWidth = gstate->LineWidth();
    _intent = gstate->Intent();
    _flatness = gstate->Flatness();
    _miterLimit = gstate->MiterLimit();
	_stroke_opacity = gstate->StrokeOpacity();
	_fill_opacity = gstate->FillOpacity();
    _lineCap = gstate->LineCap();
    _lineJoin = gstate->LineJoin();
    _dictName = gstate->DictName();
    _dash = gstate->Dash();
    _matrix = gstate->Matrix();
    _strokeColor = gstate->StrokeColor();
    _fillColor = gstate->FillColor();
	_gflag = gstate->GFlags();
	_gflag_save = gstate->_gflag_save;
}

// 현재의 gstate값을 복사해 스택에 저장한다.
void PPContext::SaveGState()
{
    PPGState *new_gstate = NewGState();
    _gstats.push(new_gstate);
}

// 스택의 값을 가져와 현재의 값을 대치시킨다.
void PPContext::RestoreGState()
{
	if(_gstats.size() > 0) {
		PPGState *new_gstate = _gstats.top();
		SetGState(new_gstate);
		_gstats.pop();
	
		delete new_gstate;
	}
	else {
		cout << "Error : GState stack pairing error." << PP_ENDL;
	}
}

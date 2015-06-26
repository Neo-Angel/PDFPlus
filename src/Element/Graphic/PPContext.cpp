
#include "PPContext.h"


//////////////////////////////////////////////////////////
//
//    PPContext
//
//////////////////////////////////////////////////////////
// =========================================================== PPContext - B

PPContext::PPContext()
{
}

PPGState *PPContext::newGState()
{
	PPGState *ret_gstate = (PPGState *)Copy();
    return ret_gstate;
}

void PPContext::setGState(PPGState *gstate)
{
    _lineWidth = gstate->lineWidth();
    _intent = gstate->intent();
    _flatness = gstate->flatness();
    _miterLimit = gstate->miterLimit();
    _lineCap = gstate->lineCap();
    _lineJoin = gstate->lineJoin();
    _dictName = gstate->dictName();
    _dash = gstate->dash();
    _matrix = gstate->matrix();
    _strokeColor = gstate->getStrokeColor();
    _fillColor = gstate->getFillColor();
}

void PPContext::saveGState()
{
    PPGState *new_gstate = newGState();
    _gstats.push(new_gstate);
}


void PPContext::restoreGState()
{
    PPGState *new_gstate = _gstats.top();
    setGState(new_gstate);
    _gstats.pop();
    delete new_gstate;
}


#ifndef __PDFPlusLib__PPPathItem__
#define __PDFPlusLib__PPPathItem__

#include <PPElement.h>


enum PPPathOperator {
    PPPO_MoveTo = PPC_MoveTo,
	PPPO_LineTo = PPC_LineTo,
	PPPO_CurveTo = PPC_CurveTo,
	PPPO_CurveTo1 = PPC_CurveTo1,
	PPPO_CurveTo2 = PPC_CurveTo2,
	PPPO_Rectangle = PPC_Rectangle,
	PPPO_ClosePath = PPC_ClosePath
};




class PPPathItem : public PPBase {
    
public:
    PPPathOperator _op; //  == PPCommandType
    PPPoint _pt1, _pt2, _pt3;

public:
	PPPathItem() {}
	PPBase *Create() {return new PPPathItem;}

	void CopyMembersTo(PPBase *obj);

    string makeCommand();
    string xmlString(int level);
};



#endif
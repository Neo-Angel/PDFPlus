
#ifndef __PDFPlusLib__PPMatrix__
#define __PDFPlusLib__PPMatrix__

#include <PPBase.h>


//
//      PPMatrix
//
///////////////////////////////////////////////////////////////
class PPMatrix : public PPBase {
    
public:
    float _a, _b, _c, _d, _x, _y;

public:
	PPMatrix(){{_a=1;_b=0;_c=0;_d=1;_x=0;_y=0;}}
	PPMatrix(float a, float b, float c, float d, float x, float y){_a=a;_b=b;_c=c;_d=d;_x=x;_y=y;}
	void Scale(float x, float y);
	void Rotate(float angle);
	void Translate(float x, float y);
    
	void Multiply(float &x, float &y);
	void Multiply(PPMatrix mtx);

	string PDFString();
};


#endif /* defined(__PDFPlusLib__PPMatrix__) */
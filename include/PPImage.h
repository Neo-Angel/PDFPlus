



#ifndef __PDFPlusLib__PPImage__
#define __PDFPlusLib__PPImage__

#include <PPBase.h>


//
//      PPGState
//
///////////////////////////////////////////////////////////////

class PPTDictionary;
class PPDocument;
class PPTStream;
class PPTIndirectObj;

class PPImage : public PPBase {
	PPTIndirectObj *_image_obj;
//	PPTDictionary *_dict;
//	PPTStream *_stream;

public:
	PPImage(string path, PPDocument *doc);

	PPTIndirectObj *MakeIndirectObj(int obj_num);
};


#endif  // __PDFPlusLib__PPImage__
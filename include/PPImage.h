



#ifndef __PDFPlusLib__PPImage__
#define __PDFPlusLib__PPImage__

#include <PPBase.h>


//
//      PPImage
//
///////////////////////////////////////////////////////////////

class PPTDictionary;
class PPDocument;
class PPTStream;
class PPTIndirectObj;

class PPImage : public PPBase {
	PPTIndirectObj *_image_obj;

public:
	PPImage(string path, PPDocument *doc);

	PPTIndirectObj *MakeIndirectObj(int obj_num);
};


#endif  // __PDFPlusLib__PPImage__
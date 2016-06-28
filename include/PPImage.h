



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
	string _path;
	PPTIndirectObj *_image_obj;

public:
	PPImage(string path, PPDocument *doc);

	PPTIndirectObj *MakeIndirectObj(int obj_num);
	string ImagePath(){return _path;}
};


#endif  // __PDFPlusLib__PPImage__
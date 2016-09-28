
#include "PPImage.h"
#include "PPTDictionary.h"
#include "PPTName.h"
#include "PPTStream.h"
#include "PPTIndirectObj.h"
#include <jpeglib.h>

const char *ColorSpaceStrings[8] = {
	"Unknown",
	"DeviceGray",
	"DeviceRGB",
	"YCBCr",
	"DeviceCMYK",
	"YCCK",
	"BG_RGB",
	"BG_YCC"
};

PPTName *nameFromColorSpace(jpeg_decompress_struct *cinfo, PPDocument *doc)
{
	//J_COLOR_SPACE
	string *csstr = new string(ColorSpaceStrings[cinfo->out_color_space]);
	PPTName *ret_name = new PPTName(doc, csstr);
	return ret_name;
}


PPImage::PPImage(string path, PPDocument *doc)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	//JSAMPARRAY buffer;	
	_path = path;

   // Error handler
   cinfo.err = jpeg_std_error(&jerr);

	FILE * infile;
	const char *filename = path.c_str();
	if ((infile = fopen(filename, "rb")) == NULL) {
	    fprintf(stderr, "can't open %s\n", filename);
	    exit(1);
	}
	/*
	char data[100];
	memset(data, 0, 100);
	size_t s = fread(data, 1,100,infile);
	cout << "Read Size : " << s << PP_ENDL;
	*/
	jpeg_create_decompress(&cinfo);

	jpeg_stdio_src(&cinfo, infile);


	//jpeg_create_decompress(&cinfo);
	jpeg_read_header(&cinfo, TRUE);
	fseek(infile, 0L,SEEK_END);
	long file_length = ftell(infile);
	_image_obj = new PPTIndirectObj();
	PPTName *color_space = nameFromColorSpace(&cinfo, doc);	

	PPTDictionary *dict = new PPTDictionary();
	dict->SetTokenAndKey(8, "BitsPerComponent");
	dict->SetTokenAndKey(color_space, "ColorSpace");  // "DeviceRGB"
	dict->SetTokenAndKey("DCTDecode", "Filter");
	dict->SetTokenAndKey(cinfo.X_density, "HRes");
	dict->SetTokenAndKey(cinfo.image_height, "Height");
	dict->SetTokenAndKey(file_length, "Length");
	dict->SetTokenAndKey("Image", "Subtype");
	dict->SetTokenAndKey("XObject", "Type");
	dict->SetTokenAndKey(cinfo.Y_density, "VRes");
	dict->SetTokenAndKey(cinfo.image_width, "Width"); 

	_image_obj->_array.push_back(dict);

	PPTStream *stream = new PPTStream(doc, file_length);
	stream->SetDictionary(dict);

	fseek(infile, 0L, SEEK_SET);
	fread(stream->_streamData, file_length, 1, infile);

	fclose(infile);

	_image_obj->_array.push_back(stream);
	stream->_parentObj = _image_obj;
}

PPTIndirectObj *PPImage::MakeIndirectObj(int obj_num)
{
	_image_obj->_objNum = obj_num;
	return _image_obj;
}

float PPImage::ImageWidth()
{
	PPTDictionary *dict = (PPTDictionary *)_image_obj->_array[0];
	
	float value = (float) dict->FloatForKey("Width");
	return value;
}

float PPImage::ImageHeight()
{
	PPTDictionary *dict = (PPTDictionary *)_image_obj->_array[0];
	
	float value = (float) dict->FloatForKey("Height");
	return value;

}
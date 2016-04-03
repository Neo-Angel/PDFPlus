
#include "PPImage.h"
#include "PPTDictionary.h"
#include "PPTName.h"
#include "PPTStream.h"
#include "PPTIndirectObj.h"
#include <jpeglib.h>


/*
		<Dict>
			<Key>BitsPerComponent</Key>
			<Number>8</Number>
			<Key>ColorSpace</Key>
			<Ref ObjID='25' Gen='0'/>
			<Key>Filter</Key>
			<Name>DCTDecode</Name>
			<Key>HRes</Key>
			<Number>287</Number>
			<Key>Height</Key>
			<Number>600</Number>
			<Key>Length</Key>
			<Number>222405</Number>
			<Key>Subtype</Key>
			<Name>Image</Name>
			<Key>Type</Key>
			<Name>XObject</Name>
			<Key>VRes</Key>
			<Number>287</Number>
			<Key>Width</Key>
			<Number>400</Number>
		</Dict>
		<Stream><![CDATA[			FFD8FFE000104A46494600010201011F011F0000FFE10F234578696600004D4D002A000000080009

*/


//typedef enum {
//	JCS_UNKNOWN,		/* error/unspecified */
//	JCS_GRAYSCALE,		/* monochrome */
//	JCS_RGB,		/* red/green/blue, standard RGB (sRGB) */
//	JCS_YCbCr,		/* Y/Cb/Cr (also known as YUV), standard YCC */
//	JCS_CMYK,		/* C/M/Y/K */
//	JCS_YCCK,		/* Y/Cb/Cr/K */
//	JCS_BG_RGB,		/* big gamut red/green/blue, bg-sRGB */
//	JCS_BG_YCC		/* big gamut Y/Cb/Cr, bg-sYCC */
//} J_COLOR_SPACE;
//


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

	PPTDictionary *_dict = new PPTDictionary();
	_dict->SetTokenAndKey(8, "BitsPerComponent");
	_dict->SetTokenAndKey(color_space, "ColorSpace");  // "DeviceRGB"
	_dict->SetTokenAndKey("DCTDecode", "Filter");
	_dict->SetTokenAndKey(cinfo.X_density, "HRes");
	_dict->SetTokenAndKey(cinfo.image_height, "Height");
	_dict->SetTokenAndKey(file_length, "Length");
	_dict->SetTokenAndKey("Image", "Subtype");
	_dict->SetTokenAndKey("XObject", "Type");
	_dict->SetTokenAndKey(cinfo.Y_density, "VRes");
	_dict->SetTokenAndKey(cinfo.image_width, "Width"); 

	_image_obj->_array.push_back(_dict);

	PPTStream *_stream = new PPTStream(doc, file_length);
	_stream->SetDictionary(_dict);

	fseek(infile, 0L, SEEK_SET);
	fread(_stream->_streamData, file_length, 1, infile);

	fclose(infile);

	_image_obj->_array.push_back(_stream);
}

PPTIndirectObj *PPImage::MakeIndirectObj(int obj_num)
{
	_image_obj->_objNum = obj_num;
	return _image_obj;
}

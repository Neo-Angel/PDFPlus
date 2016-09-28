// PPElementBuilderTest.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include <PDFPlus.h>
#include <iostream>



int main(int argc, char* argv[])
{
	int ret = 0;
	string input_path =  "..\\..\\TestFiles\\";
	string output_path = "..\\..\\TestFiles\\Output\\";

	PDFPlusInit();
	PPDocument doc;

	PPRect page_rect(0, 0, 595, 842); // A4
	PPPage *page = doc.NewPage(page_rect);

	string image_path = input_path + "peppers.jpg";
	PPEImage *image_element = new PPEImage(image_path, page->ContextRef());
	float img_width = image_element->ImageWidth();
	float img_height = image_element->ImageHeight();
	image_element->SetTransform(img_width / 2, -145, 20, img_height/2, 200, 150);
	page->WritePlacedElement(image_element);

	image_element->SetTransform(200, 0, 0, 300, 50, 450);
	page->WritePlacedElement(image_element);

	image_element->SetTransform(200, 0, 0, -150, 300, 600);
	page->WritePlacedElement(image_element);

	string pdf_path = output_path + "element_build.pdf";
	delete image_element;
	doc.Save(pdf_path);

	return ret;
}

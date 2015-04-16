typedef struct {
	char *name;
	Image *address;
} IMAQ_Image;

__declspec(dllexport) int area(int width, int height);
__declspec(dllexport) int area_from_image(IMAQ_Image *imageSrc);
__declspec(dllexport) int show_blank_image(IMAQ_Image *imageDst, int width, int height);
__declspec(dllexport) int image_blur(IMAQ_Image *imageSrc, IMAQ_Image *imageDst);
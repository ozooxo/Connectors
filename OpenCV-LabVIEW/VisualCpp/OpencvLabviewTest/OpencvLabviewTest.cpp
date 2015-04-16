// OpencvLabviewTest.cpp : Defines the exported functions for the DLL application.

#include "stdafx.h"

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <nivision.h>

#include "OpencvLabviewTest.h" 

using namespace std;
using namespace cv;

/*
 * Use openCV function "size_t Mat::total() const" to calculate the area
 * of a blank image.
 */
__declspec(dllexport) int area(int width, int height) {
	
	Mat img(height, width, CV_16U);
	return img.total();
}

/*
 * Use NI Vision function "IMAQ GetImageSize" to get the height and width 
 * of the input image, and calculate the image area from them.
 */
__declspec(dllexport) int area_from_image(IMAQ_Image *imageSrc) {

	ImageInfo* src;
	int width, height;

	if (!imageSrc) return ERR_NOT_IMAGE;

	src = (ImageInfo*)imageSrc->address;
	imaqGetImageSize((Image*)src, &width, &height);
	
	return width*height;
}

/*
 * Make a blank image with size "width"/"height", and change its x-direction
 * intensity to gradient.
 */
__declspec(dllexport) int show_blank_image(IMAQ_Image *imageDst, int width, int height) {

	ImageInfo* dst;
	IplImage *CVImageDst;

	if (!imageDst) return ERR_NOT_IMAGE;

	dst = (ImageInfo*)imageDst->address;
	imaqSetImageSize((Image*)dst, width, height);
	CVImageDst = cvCreateImageHeader(cvSize(width, height), IPL_DEPTH_8U, 1); //create a image header but does not allocate the image data
	CVImageDst->imageData = (char*)dst->imageStart;
	CVImageDst->widthStep = dst->pixelsPerLine;

	for (int y = 0; y < height; ++y) {
		uchar* ptr = (uchar*)(CVImageDst->imageData + y*CVImageDst->widthStep);
		for (int x = 0; x < width; ++x) {
			ptr[x] = x % 255;
		}
	}

	return 0;
}

/*
 * Gaussian blur the input image.
 */
__declspec(dllexport) int image_blur(IMAQ_Image *imageSrc, IMAQ_Image *imageDst) {

	ImageInfo* src;
	ImageInfo* dst;
	IplImage *CVImageSrc;
	IplImage *CVImageDst;
	int width, height;

	if (!imageSrc) return ERR_NOT_IMAGE;

	src = (ImageInfo*)imageSrc->address;
	imaqGetImageSize((Image*)src, &width, &height);
	CVImageSrc = cvCreateImageHeader(cvSize(width, height), IPL_DEPTH_8U, 1);
	CVImageSrc->imageData = (char*)src->imageStart;
	CVImageSrc->widthStep = src->pixelsPerLine;

	dst = (ImageInfo*)imageDst->address;
	imaqSetImageSize((Image*)dst, width, height);
	CVImageDst = cvCreateImageHeader(cvSize(width, height), IPL_DEPTH_8U, 1);
	CVImageDst->imageData = (char*)dst->imageStart;
	CVImageDst->widthStep = dst->pixelsPerLine;

	Mat matSrc(CVImageSrc, false);
	Mat matDst = matSrc.clone();	

	/*
	 * "blur" or "GaussianBlur" will cause momery error when running this piece
	 * of code in LabVIEW. Reason unknown. Filter works fine.
	 *
	int MAX_KERNEL_LENGTH = 31;
	for (int i = 0; i < MAX_KERNEL_LENGTH; i = i + 2) {
		blur(matSrc, matDst, Size(i, i));
	}
	*/
	Mat kernel = Mat::ones(5, 5, CV_8U);
	filter2D(matSrc, matDst, -1, kernel, cv::Point(-1, -1), 0, BORDER_DEFAULT);

	/*
	 * "memcpy" seems cannot copy the whole image (the last 1/5 part is missing).
	 * Probably the size (=width*height) is not right. Will check later.
	 *
	memcpy(CVImageDst->imageData, matDst.data, width*height);
	*/
	for (int y = 0; y < height; ++y) {
		uchar* ptr = (uchar*)(CVImageDst->imageData + y*CVImageDst->widthStep);
		for (int x = 0; x < width; ++x) {
			ptr[x] = matDst.at<uchar>(y, x);
		}
	}

	return 0;
}
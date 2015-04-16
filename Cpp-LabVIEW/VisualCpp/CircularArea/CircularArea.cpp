// CircularArea.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "math.h"
#include "CircularArea.h"

__declspec(dllexport) double area(double radius) {
	return M_PI * radius * radius;
}
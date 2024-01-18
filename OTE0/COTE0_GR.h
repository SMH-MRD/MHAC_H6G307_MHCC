#pragma once

#include <windows.h>
#include <math.h>

class COTE0_GR
{
public:
	COTE0_GR() {};
	~COTE0_GR() {};

	POINT* rotate_rect0(POINT* p,double rad) {
		double w = (double)((p + 1)->x - p->x);
		double h = (double)((p + 2)->y - p->y);
		LONG x1 = (LONG)(w * cos(rad)), y1 = -(LONG)(w * sin(rad));
		LONG x2 = (LONG)(h * sin(rad)), y2 = -(LONG)(h * cos(rad));

		(p + 1)->x = x1 + p->x; (p + 1)->y = y1 + p->y;
		(p + 2)->x = x2 + p->x; (p + 2)->y = y2 + p->y;

		double b = sin(1.5);
		return p;
	}

};


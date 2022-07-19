#pragma once

#define COMPARE_GREAT_THAN      0   // >
#define COMPARE_LESS_THAN       1   // <
#define COMPARE_GREAT_EQUIAL    2   // >=
#define COMPARE_LESS_EQUIAL     3   // <=
#define COMPARE_EQUIAL          4   // ==
#define COMPARE_NOT_EQUIAL      5   // !=

class Util
{
public:
	static bool ApproxEqual(float left, float right, float epsilon = 1.555555555e-01F);
	
	template <class T>
	static bool CompareTemplate(T left, int compareFlag, T right) {
		bool r = false;
		switch (compareFlag) {
		case COMPARE_GREAT_THAN:	r = (left > right); break;
		case COMPARE_LESS_THAN:		r = (left < right); break;
		case COMPARE_GREAT_EQUIAL:	r = (left >= right); break;
		case COMPARE_LESS_EQUIAL:	r = (left <= right); break;
		case COMPARE_EQUIAL:		r = (left == right); break;
		case COMPARE_NOT_EQUIAL:	r = (left != right); break;
		default: break; // _ASSERT_EXPR(0, L"”äŠr‰‰ŽZŽq‚ª•s–¾‚Å‚·");
		}
		return r;
	}

	template <class T>
	static void SafeDelete(T*& p) {
		if (p) {
			delete p;
			p = nullptr;
		}
	}
};

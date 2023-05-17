#pragma once
#include "stdafx.h"

enum class COLOR {
	RED              = RGB(255.f, 0.f, 0.f),
	GREEN            = RGB(0.f, 255.f, 0.f),
	DARK_OLIVE_GREEN = RGB(85.f, 107.f, 47.f),
	OLIVE_DRAB       = RGB(107, 142, 35),
	BLUE             = RGB(0.f, 0.f, 255.f),
	NAVY             = RGB(0, 0, 128),
	WHITE            = RGB(255.f, 255.f, 255.f),
	BLACK            = RGB(0.f, 0.f, 0.f),
	LIGHT_GREY       = RGB(204.f, 204.f, 204.f),
	DEEP_GREY        = RGB(051.f, 051.f, 051.f),
	MIDDLE_GREY      = RGB(153.f, 153.f, 153.f),
	YELLOW           = RGB(255.f, 255.f, 0.f),
	BROWN            = RGB(153, 102.f, 0.f),
	ORANGE           = RGB(255.f, 153.f, 0.f),
	DEEP_RED         = RGB(153.f, 0.f, 0.f),
	SKY_BLUE         = RGB(052.f, 255.f, 255.f),
	
	END,
};

enum class COMPONENT_TYPE {
	TRANSFORM,
	CAMERA,
	END,

};

enum class SCRIPT_TYPE {
	PLAYER,
	BULLET,
	EXPLOSIVE,
	TANK_MONSTER,
	END,

};

enum class PROJECTION_TYPE {
	PERSPECTIVE,
	ORTHOGRAPHIC,

	END,

};

enum class SCRIPT_LIST {
	PLAYER,


};

enum class KEY
{
	Q, W, E, R, T, Y, U, I, O, P,
	A, S, D, F, G, H, J, K, L,
	Z, X, C, V, B, N, M,
	LEFT, RIGHT, UP, DOWN,
	_0, _1, _2, _3, _4, _5, _6, _7, _8, _9,
	NUM0, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9,
	LSHFT, LALT, LCTRL, SPACE, ENTER, BACK, ESC, TAB, DEL,
	LBTN, RBTN, MBTN,
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

	KEY_END,
};




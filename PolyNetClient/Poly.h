#pragma once
#include <Windows.h>

//
// It's important that everything in this file ALWAYS be inlined, 
// or else implementation details will leak.
//

//
// A modification to apply to bytes.
//
// May want to randomize these.
//
enum class BYTE_MOD : UINT8 {
	C = 0,
	S,
	A,
	I
};

//
// To.
//
__forceinline UINT8 ToC(UINT8 v) {
	return -v;
}

__forceinline UINT8 ToS(UINT8 v) {
	return 128 - v;
}

__forceinline UINT8 ToA(UINT8 v) {
	return v + 128;
}

__forceinline UINT8 ToI(UINT v) {
	return ~v;
}

//
// From.
//
__forceinline UINT8 FromC(UINT8 v) {
	return -v;
}

__forceinline UINT8 FromS(UINT8 v) {
	return 128 - v;
}

__forceinline UINT8 FromA(UINT8 v) {
	return v - 128;
}

__forceinline UINT8 FromI(UINT v) {
	return ~v;
}

//
// Transform a byte.
//
__forceinline UINT8 TransformTo(UINT8 v, BYTE_MOD Mod) {
	if (Mod == BYTE_MOD::C) {
		return ToC(v);
	} else if (Mod == BYTE_MOD::S) {
		return ToS(v);
	} else if (Mod == BYTE_MOD::A) {
		return ToA(v);
	} else if (Mod == BYTE_MOD::I) {
		return ToI(v);
	} else {
		return v;
	}
}

//
// Transform a byte.
//
__forceinline UINT8 TransformFrom(UINT8 v, BYTE_MOD Mod) {
	if (Mod == BYTE_MOD::C) {
		return FromC(v);
	} else if (Mod == BYTE_MOD::S) {
		return FromS(v);
	} else if (Mod == BYTE_MOD::A) {
		return FromA(v);
	} else if (Mod == BYTE_MOD::I) {
		return FromI(v);
	} else {
		return v;
	}
}

//
// Transforms a short.
//
__forceinline UINT16 TransformTo(UINT16 v, BYTE_MOD ModA, BYTE_MOD ModB) {
	auto Right = TransformTo((UINT8)(v & 0xfful), ModA);
	auto Left = TransformTo((UINT8)((v >> 8) & 0xfful), ModB);
	return (UINT16)Right | ((UINT16)Left << 8);
}

//
// Transforms a short.
//
__forceinline UINT16 TransformFrom(UINT16 v, BYTE_MOD ModA, BYTE_MOD ModB) {
	auto Right = TransformFrom((UINT8)(v & 0xfful), ModA);
	auto Left = TransformFrom((UINT8)((v >> 8) & 0xfful), ModB);
	return (UINT16)Right | ((UINT16)Left << 8);
}


//
// Transforms an int.
//
__forceinline UINT32 TransformTo(UINT32 v, BYTE_MOD ModA, BYTE_MOD ModB, BYTE_MOD ModC, BYTE_MOD ModD) {
	auto a = TransformTo((UINT8)(v & 0xfful), ModA);
	auto b = TransformTo((UINT8)((v >> 8) & 0xfful), ModB);
	auto c = TransformTo((UINT8)((v >> 16) & 0xfful), ModC);
	auto d = TransformTo((UINT8)((v >> 24) & 0xfful), ModD);
	return (UINT32)a | ((UINT32)b << 8) | ((UINT32)c << 16) | ((UINT32)d << 24);
}

//
// Transforms an int.
//
__forceinline UINT32 TransformFrom(UINT32 v, BYTE_MOD ModA, BYTE_MOD ModB, BYTE_MOD ModC, BYTE_MOD ModD) {
	auto a = TransformFrom((UINT8)(v & 0xfful), ModA);
	auto b = TransformFrom((UINT8)((v >> 8) & 0xfful), ModB);
	auto c = TransformFrom((UINT8)((v >> 16) & 0xfful), ModC);
	auto d = TransformFrom((UINT8)((v >> 24) & 0xfful), ModD);
	return (UINT32)a | ((UINT32)b << 8) | ((UINT32)c << 16) | ((UINT32)d << 24);
}
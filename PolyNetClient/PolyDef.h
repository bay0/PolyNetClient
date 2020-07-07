#pragma once
#include "Poly.h"

//
// Simple transform macros that use our pre-defined mods.
//
// These should be randomly generated each compile.
//
#define TRANSFORM_UI8(x) TransformTo(x, BYTE_MOD::C)
#define TRANSFORM_UI16(x) TransformTo(x, BYTE_MOD::S, BYTE_MOD::I)
#define TRANSFORM_UI32(x) TransformTo(x, BYTE_MOD::C, BYTE_MOD::C, BYTE_MOD::C, BYTE_MOD::C)
#define TRANSFORM_UI64(x) TransformTo(x, BYTE_MOD::A, BYTE_MOD::A, BYTE_MOD::I, BYTE_MOD::C, BYTE_MOD::S, BYTE_MOD::A, BYTE_MOD::I, BYTE_MOD::C)

#define DETRANSFORM_UI8(x) TransformFrom(x, BYTE_MOD::C)
#define DETRANSFORM_UI16(x) TransformFrom(x, BYTE_MOD::S, BYTE_MOD::I)
#define DETRANSFORM_UI32(x) TransformFrom(x, BYTE_MOD::C, BYTE_MOD::C, BYTE_MOD::C, BYTE_MOD::C)
#define DETRANSFORM_UI64(x) TransformFrom(x, BYTE_MOD::A, BYTE_MOD::A, BYTE_MOD::I, BYTE_MOD::C, BYTE_MOD::S, BYTE_MOD::A, BYTE_MOD::I, BYTE_MOD::C)
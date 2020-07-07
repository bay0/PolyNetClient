#pragma once
#include <Windows.h>
#pragma comment (lib, "Ws2_32.lib")

//
// Packet config.
//
#define PACKET_LEN 0x200

//
// Packet opcodes.
//
#define OP_C2S_TEST 1

#pragma pack(push, 1)
//
// A test packet.
//
struct PacketC2STest {
	UINT8 U8;
	UINT16 U16;
	UINT32 U32;
	UINT64 U64;
};
#pragma pack(pop)
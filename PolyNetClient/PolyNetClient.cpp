#include "Config.h"
#include "Client.h"
#include "Logging.h"
#include "PolyDef.h"

#include <iostream>

//
// Our client.
//
static Client C;

//
// Starts the client.
//
VOID StartClient(VOID) {
	C.Ip = REMOTE_IP;
	C.Port = REMOTE_PORT;

	//
	// Register our packet handlers.
	//
	// C.RegisterHandler(OP_S2C_INIT, OnInitPacket, NULL);
	//

	//
	// Init WSA.
	//
	C.Init();

	//
	// Receive/respond to packets.
	//
	if (!C.Connect()) {
		LOG("Failed to connect!");
		return;
	}

	//
	// Send the test packet.
	//
	// Inspect the results for validity on the server side.
	//
	PacketC2STest Test;
	Test.U8 = TRANSFORM_UI8(0x69);
	Test.U16 = TRANSFORM_UI16(0x69);
	Test.U32 = TRANSFORM_UI32(0x69);
	//Test.U64 = TRANSFORM_UI64(0x69);
	C.SendWrapped(OP_C2S_TEST, Test);

	LOG("Ticking");
	while (C.Connected) {
		C.Tick();
		Sleep(1);
	}
}

//
// Runs all tests. They're pretty straight-forward, no unit tests required.
//
// This entire thing (besides the logging) should be optimized out by the compiler. If it is optimized out,
// then the compiler has tested our code for us. :)
//
static BOOLEAN Tests(VOID) {
	LOG("Running U8 encode test");
	UINT8 U8 = 0x69u;
	UINT8 U8E = TRANSFORM_UI8(U8);
	UINT8 U8D = DETRANSFORM_UI8(U8E);
	if (U8 != U8D) {
		LOG("  Failed!");
		return FALSE;
	}

	LOG("Running U16 encode test");
	UINT16 U16 = 0x6969u;
	UINT16 U16E = TRANSFORM_UI16(U16);
	UINT16 U16D = DETRANSFORM_UI16(U16E);
	if (U16 != U16D) {
		LOG("  Failed!");
		return FALSE;
	}

	LOG("Running U32 encode test");
	UINT32 U32 = 0x69696969ul;
	UINT32 U32E = TRANSFORM_UI32(U32);
	UINT32 U32D = DETRANSFORM_UI32(U32E);
	if (U32 != U32D) {
		LOG("  Failed!");
		return FALSE;
	}

	LOG("Finished running all tests");
	return TRUE;
}

int main(VOID) {
	LOG("Running tests");
	if (!Tests()) {
		LOG("Failed to run tests");
		return 1;
	}

	LOG("Starting client");
	StartClient();

	LOG("Finished running!");
	std::cin.get();
	return 0;
}

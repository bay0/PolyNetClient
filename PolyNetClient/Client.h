#pragma once
#include <vector>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <cstdint>

#include "Net.h"

#pragma pack(push, 1)
class Client;

//
// A packet.
//
struct Packet {
	//
	// The opcode of the packet.
	//
	UINT8 Opcode = 0;
	//
	// The body of the packet.
	//
	PVOID Body = NULL;
	//
	// The length of the body.
	//
	UINT32 BodyLength = 0;

	//
	// Nulls the packet body.
	//
	inline VOID NullBody(VOID) {
		Body = NULL;
		BodyLength = 0;
	}
};

//
// A packet fragment.
//
struct PacketFragment {
	//
	// The total size of the packet.
	//
	UINT32 TotalSize;
	//
	// The total number of parts in the packet.
	//
	UINT32 TotalParts;
	//
	// The size of this part.
	//
	UINT16 part_size;
	//
	// The index of this part.
	//
	UINT32 Part;
	//
	// The opcode of the packet.
	//
	UINT8 Opcode;
	//
	// The id of the packet.
	//
	UINT16 Id;
	//
	// The body of this fragment.
	//
	UINT8 Body[PACKET_LEN];
};
#pragma pack(pop)

//
// A packet trace.
//
struct PacketTrace {
	//
	// All tracked fragments for this trace.
	//
	std::vector<PacketFragment> Fragments;
	//
	// The root fragment.
	//
	PacketFragment Root;
	//
	// The total number of parts assembled.
	//
	UINT32 PartsAssembled = 0;

	//
	// Assemblies a piece of the packet being traced.
	//
	inline VOID Assemble(PacketFragment Fragment) {
		if (Fragment.TotalParts >= Fragments.size()) {
			Fragments.resize(Fragment.TotalParts);
		}

		if (Fragment.Part == 0) {
			Root = Fragment;
		}

		Fragments[Fragment.Part] = Fragment;
		PartsAssembled += 1;
	}

	//
	// Retrieves the id of this trace.
	//
	// @return The id of this packet.
	//
	inline UINT16 GetId(VOID) {
		return Root.Id;
	}

	//
	// Retrieves the opcode of this trace.
	//
	// @return The opcode of this packet.
	//
	inline UINT8 GetOpcode(VOID) {
		return Root.Opcode;
	}

	//
	// Determines if this trace is completed.
	//
	// @return If this packet is complete.
	//
	inline BOOLEAN IsComplete(VOID) {
		if (Fragments.size() < Root.TotalParts) {
			return FALSE;
		}

		auto Valid = 0u;
		auto SigCounter = 0;
		for (auto i = 0u; i < Root.TotalParts; i++) {
			auto Fragment = &Fragments[i];
			if (Fragment->Part == SigCounter) {
				Valid += 1;
			}
			SigCounter += 1;
		}

		return Valid >= Root.TotalParts;

	}

	//
	// Combines this trace into a packet.
	//
	// This function can fail due to being incomplete, please ensure validity
	// and not to expect a valid packet.
	//
	// The returned packet body must be freed using the 'free' API.
	//
	// @param packet  The packet to update with the results.
	// @return If a packet was successfully combined.
	//
	inline BOOLEAN Combine(Packet *Packet) {
		if (!IsComplete()) {
			return FALSE;
		}

		auto Buf = (PCHAR)malloc(Root.TotalSize);
		auto Offset = 0;
		for (auto i = 0u; i < Root.TotalParts; i++) {
			auto fragment = &Fragments[i];
			memcpy(Buf + Offset, fragment->Body, fragment->part_size);
			Offset += fragment->part_size;
		}

		Packet->Opcode = GetOpcode();
		Packet->Body = (PVOID)Buf;
		Packet->BodyLength = Root.TotalSize;
		return TRUE;
	}
};

//
// A packet handler function.
//
typedef VOID(*FnHandleClientPacket)(PVOID Ctx, Client *Client, Packet *Packet);

//
// A packet handler.
//
struct ClientPacketHandler {
	//
	// The handler function.
	//
	FnHandleClientPacket Func;
	//
	// The context to pass to the function.
	//
	PVOID Ctx = NULL;
};

//
// A client.
//
class Client {
private:
	//
	// WSA init data.
	//
	WSADATA WsaData;
	//
	// The socket connection to the server.
	//
	SOCKET ClientSocket = INVALID_SOCKET;
	//
	// The packet send id.
	//
	UINT16 SendId = 0;
	//
	// The packet recv id.
	//
	UINT16 RecvId = 0;
	//
	// A map of packet handlers.
	//
	ClientPacketHandler PacketHandlers[0xFF] = { };
	//
	// The currently decoding part.
	//
	PacketFragment CurrentPart;
	//
	// The receive fragment offset.
	//
	UINT64 FragmentOff = 0;
	//
	//  A map of packet traces.
	//
	std::vector<PacketTrace> Traces = std::vector<PacketTrace>(0xffff);
public:
	//
	// If we're connected to the server.
	//
	BOOLEAN Connected = FALSE;
	//
	// The IP to connect to.
	//
	UINT32 Ip = 0;
	//
	// The port to connect to.
	//
	UINT16 Port = 0;

private:
	//
	// Attempts to receive a packet frame.
	//
	BOOLEAN AttemptRecv(VOID);
	//
	// Handles an incoming packet.
	//
	VOID HandlePacket(Packet *incoming);

private:
	//
	// Sends a packet fragment to the host.
	//
	VOID SendFragment(Packet *packet, UINT32 SendId, UINT32 parts, UINT32 part_idx);
	
public:
	//
	// Connects to the server.
	//
	BOOLEAN Connect(VOID);
	//
	// Initializes the network manager.
	//
	BOOLEAN Init(VOID);

public:
	//
	// Handles a network manager tick.
	//
	// Will reconnect if disconnected.
	//
	BOOLEAN SmartTick(VOID);
	//
	// Handles a network manager tick.
	//
	VOID Tick(VOID);

public:
	//
	// Registers a packet handler to this network manager.
	//
	VOID RegisterHandler(UINT8 Opcode, FnHandleClientPacket handler, PVOID Ctx);
	//
	// Sends a packet to the host.
	//
	VOID Send(Packet *Packet);
	//
	// Sends a wrapped packet.
	//
	template<typename T>
	inline VOID SendWrapped(UINT8 Opcode, T &t) {
		Packet NP;
		NP.Opcode = Opcode;
		NP.Body = &t;
		NP.BodyLength = sizeof(t);
		Send(&NP);
	}

public:
	//
	// Stops the networking.
	//
	VOID Stop(VOID);
};

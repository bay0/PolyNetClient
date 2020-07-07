#include "Client.h"
#include "Logging.h"
#include "Config.h"

NOINLINE VOID Client::HandlePacket(Packet *Incoming) {
	auto Handler = PacketHandlers[Incoming->Opcode];
	if (auto Func = Handler.Func) {
		Func(Handler.Ctx, this, Incoming);
	}
}

NOINLINE BOOLEAN Client::Connect(VOID) {
	LOG("Opening socket");
	ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ClientSocket == INVALID_SOCKET) {
		LOG("Failed to open socket");
		return FALSE;
	}

	SOCKADDR_IN ServerAddr;
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(Port);
	ServerAddr.sin_addr.s_addr = Ip;

	LOG("Connecting");
	auto Ret = ::connect(ClientSocket, (SOCKADDR *)&ServerAddr, sizeof(ServerAddr));
	if (Ret != 0) {
		LOG("Failed to connect");
		return FALSE;
	}

	LOG("Socket IOCTL");
	u_long Ul = 1;
	ioctlsocket(ClientSocket, FIONBIO, &Ul);

	Connected = TRUE;
	return TRUE;
}

NOINLINE BOOLEAN Client::Init(VOID) {
	LOG("WSAStartup");
	WSAStartup(MAKEWORD(2, 2), &WsaData);
	return TRUE;
}

NOINLINE BOOLEAN Client::AttemptRecv(VOID) {
	auto Decoded = FALSE;
	if (Connected) {
		auto RecvAmt = 0;
		auto HeaderSize = sizeof(PacketFragment) - PACKET_LEN;
		if (HeaderSize > FragmentOff) {
			RecvAmt = (int)(HeaderSize - FragmentOff);
		} else {
			auto Rel = FragmentOff - HeaderSize;
			RecvAmt = (int)(CurrentPart.part_size - Rel);
		}

		if (RecvAmt) {
			auto Received = recv(ClientSocket, (PCHAR)&CurrentPart + FragmentOff, RecvAmt, 0);
			if (WSAGetLastError() != WSAEWOULDBLOCK && (Received == 0 || Received == -1)) {
				Connected = FALSE;
				return FALSE;
			}

			if (Received > 0) {
				FragmentOff += Received;
				RecvAmt -= Received;
				Decoded = TRUE;
			}
		} else {
			auto &Trace = Traces[CurrentPart.Id];
			Trace.Assemble(CurrentPart);

			if (Trace.IsComplete()) {
				auto Pkt = Packet();
				if (Trace.Combine(&Pkt)) {
					HandlePacket(&Pkt);
				}
				Traces[CurrentPart.Id] = PacketTrace();
			}

			FragmentOff = 0;
			Decoded = TRUE;
		}
	}

	return Decoded;
}

NOINLINE BOOLEAN Client::SmartTick(VOID) {
	auto JustConnected = FALSE;
	if (!Connected) {
		LOG("Connecting");
		Connect();
		JustConnected = TRUE;
	}

	if (Connected) {
		for (auto i = 0u; i < 10000 && AttemptRecv(); i++);
	}

	return JustConnected;
}

NOINLINE VOID Client::Tick(VOID) {
	if (Connected) {
		for (auto i = 0; i < 10000 && AttemptRecv(); i++);
	}
}

NOINLINE VOID Client::SendFragment(Packet *Packet, UINT32 SendId, UINT32 Parts, UINT32 PartIdx) {
	auto BufBegin = PartIdx * PACKET_LEN;
	auto BufLen = min((UINT32)PACKET_LEN, Packet->BodyLength - BufBegin);

	auto Fragment = PacketFragment();
	Fragment.TotalSize = Packet->BodyLength;
	Fragment.TotalParts = Parts;
	Fragment.part_size = BufLen;
	Fragment.Id = SendId;
	Fragment.Part = PartIdx;
	Fragment.Opcode = Packet->Opcode;
	memcpy(Fragment.Body, (PCHAR)Packet->Body + BufBegin, BufLen);

	auto SizeNBody = sizeof(Fragment) - PACKET_LEN;
	::send(ClientSocket, (PCHAR)&Fragment, (int)(SizeNBody + BufLen), 0);
}

NOINLINE VOID Client::Send(Packet *packet) {
	auto count = packet->BodyLength / PACKET_LEN;
	if (packet->BodyLength % PACKET_LEN) {
		count += 1;
	}

	auto PktId = SendId++;
	for (auto i = 0u; i < count; i++) {
		SendFragment(packet, PktId, count, i);
	}
}

NOINLINE VOID Client::Stop(VOID) {
	closesocket(ClientSocket);
}

NOINLINE VOID Client::RegisterHandler(UINT8 Opcode, FnHandleClientPacket Func, PVOID Ctx) {
	ClientPacketHandler Handler;
	Handler.Ctx = Ctx;
	Handler.Func = Func;

	PacketHandlers[Opcode] = Handler;
}
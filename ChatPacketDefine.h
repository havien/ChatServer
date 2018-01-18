#pragma once
#include "../Aurora/Utility/AuroraDefine.h"
#include "../Aurora/Network/PacketBase.h"

#include "ChatDefine.h"

using namespace Aurora;

#pragma pack(push, 1)
	class CPacketRegisterReq : public Network::ClientPacket
	{
	public:
		CPacketRegisterReq():
		ClientPacket( (Int16)ENormalPacketOperation::RegisterReq )
		{

		}

		virtual ~CPacketRegisterReq( void )
		{

		}
	};

	class CPacketRegisterAck : public Network::ServerPacket
	{
	public:
		CPacketRegisterAck():
			ServerPacket( (Int16)ENormalPacketOperation::RegisterAck ),
			uniqueID( 0 )
		{

		}

		virtual ~CPacketRegisterAck( void )
		{

		}

		UInt32 uniqueID;
	};
#pragma pack(pop)
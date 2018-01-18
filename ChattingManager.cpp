#pragma once
#include "../Aurora/Utility/StringManager.h"
#include "../Aurora/Network/NetworkManager.h"

#include "ChattingManager.h"
#include "ChatPacketDefine.h"

using namespace Aurora;
using namespace Aurora::Network;

ChattingManager::ChattingManager( void )
{
}

ChattingManager::~ChattingManager( void )
{
}

bool ChattingManager::SendSignupPacket( WCHAR* pAccountID, WCHAR* pPassword, WCHAR* pNickName )
{
	if( pAccountID && pPassword && pNickName )
	{
		CPacketRegisterReq RegisterReq;

		/*AuroraStringManager->ClearAndCopy( pAccountID, RegisterReq.AccountID, (MAX_ACCOUNT_ID_LEN - 1) );
		AuroraStringManager->ClearAndCopy( pPassword, RegisterReq.Password, (MAX_PASSWORD_LEN - 1) );
		AuroraStringManager->ClearAndCopy( pNickName, RegisterReq.NickName, (MAX_NICKNAME_LEN - 1) );
		RegisterReq.CalculateSize();

		Int32 sendBytes = 0;
		bool sendResult = AuroraNetworkManager->SendToServer( reinterpret_cast<const char*>(&RegisterReq),
															  RegisterReq.GetSize(), 
															  OUT sendBytes );
		if( false == sendResult )
		{
			return false;
		}

		if( 0 < sendBytes )
		{
			return true;
		}*/
	}

	return false;
}
//
//bool ChattingManager::SendLoginPacket( WCHAR* pAccountID, WCHAR* pPassword )
//{
//	if( pAccountID && pPassword )
//	{
//		CPacketLoginReq PacketLoginReq;
//		AuroraStringManager->ClearAndCopy( pAccountID, PacketLoginReq.AccountID, (MAX_ACCOUNT_ID_LEN - 1) );
//		AuroraStringManager->ClearAndCopy( pPassword, PacketLoginReq.Password, (MAX_PASSWORD_LEN - 1) );
//		PacketLoginReq.CalculateSize();
//
//		Int32 sendBytes = 0;
//		bool sendResult = AuroraNetworkManager->SendToServer( reinterpret_cast<const char*>(&PacketLoginReq), 
//														   PacketLoginReq.GetSize(), 
//														   sendBytes );
//		if( false == sendResult )
//		{
//			return false;
//		}
//
//		if( 0 < sendBytes )
//		{
//			return true;
//		}
//	}
//
//	return false;
//}
//
//bool ChattingManager::SendLogoutPacket( UInt32 AccountUniqueID )
//{
//	CPacketLogoutReq PacketLogoutReq;
//	PacketLogoutReq.AccountUniqueID = AccountUniqueID;
//	PacketLogoutReq.CalculateSize();
//
//	Int32 sendBytes = 0;
//	bool sendResult = AuroraNetworkManager->SendToServer( reinterpret_cast<const char*>(&PacketLogoutReq), 
//													   PacketLogoutReq.GetSize(), 
//													   sendBytes );
//
//	if( false == sendResult )
//	{
//		return false;
//	}
//
//	if( 0 < sendBytes )
//	{
//		return true;
//	}
//
//	return false;
//}
//bool ChattingManager::SendNormalChat( /*WCHAR* pNickName,*/ WCHAR* pChatText )
//{
//	if( /*pNickName &&*/ pChatText )
//	{
//		CPacketNormalChatReq ChatReq;
//		ChatReq.SetChatText( pChatText );
//		ChatReq.CalculateSize();
//
//		Int32 sendBytes = 0;
//		bool sendResult = AuroraNetworkManager->SendToServer( reinterpret_cast<const char*>(&ChatReq), 
//																 ChatReq.GetSize(), 
//																 sendBytes );
//		if( false == sendResult )
//		{
//			return false;
//		}
//
//		if( 0 < sendBytes )
//		{
//			return true;
//		}
//	}
//
//	return false;
//}
//
//bool ChattingManager::SendWhisperPacket( WCHAR* pSenderAccountID, WCHAR* pTargetAccountID, WCHAR* pChatText )
//{
//	if( pTargetAccountID && pChatText )
//	{
//		CPacketWhisperChatReq PacketWhisperChatReq;
//		AuroraStringManager->ClearAndCopy( pSenderAccountID, PacketWhisperChatReq.SenderAccountID, ( MAX_ACCOUNT_ID_LEN-1 ) );
//		AuroraStringManager->ClearAndCopy( pTargetAccountID, PacketWhisperChatReq.TargetAccountID, ( MAX_ACCOUNT_ID_LEN-1 ) );
//		AuroraStringManager->ClearAndCopy( pChatText, PacketWhisperChatReq.ChatText, (MAX_CHAT_TEXT_LEN-1) );
//		PacketWhisperChatReq.CalculateSize();
//
//		Int32 sendBytes = 0;
//		bool sendResult = AuroraNetworkManager->SendToServer( reinterpret_cast<const char*>(&PacketWhisperChatReq), 
//														   PacketWhisperChatReq.GetSize(), 
//														   sendBytes );
//		if( false == sendResult )
//		{
//			return false;
//		}
//
//		if( 0 < sendBytes )
//		{
//			return true;
//		}
//	}
//
//	return false;
//}
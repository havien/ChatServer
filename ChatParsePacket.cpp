#pragma once
#include "../AuroraUtility/StringManager.h"
#include "../AuroraUtility/MiscManager.h"

#include "../AuroraNetwork/PacketBase.h"
#include "../AuroraNetwork/WinsockManager.h"
#include "../AuroraNetwork/NetworkManager.h"

#include "ChatUserInfo.h"
#include "ChatParsePacket.h"

PacketProcessor::PacketProcessor( void ) :
_IOCPDatas( NORMAL_QUEUE_SIZE )
{
	_parsePacketEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	if( _parsePacketEvent == NULL )
	{
		//printf( "CreateEvent failed (%d)\n", GetLastError() );
	}

	_runningParsePacketThread = true;
}
PacketProcessor::~PacketProcessor( void )
{
	userTables.erase( userTables.begin(), userTables.end() );

	for ( auto counter = 0; counter < _IOCPDatas.GetSize(); ++counter )
	{
		IOCPData* pIOCPData = _IOCPDatas.Dequeue();
		if( pIOCPData )
		{
			SAFE_DELETE( pIOCPData );
		}
	}

	CloseHandle( _parsePacketEvent );
}

UInt32 __stdcall PacketProcessor::ParsePacket( void* pArgs )
{
	if( pArgs )
	{
		auto pThis = (PacketProcessor *)pArgs;
		auto pBufferQueue = pThis->GetIOCPDataQueue();
		while( true == pThis->_runningParsePacketThread )
		{
			auto WaitResult = WaitForSingleObject( pThis->_parsePacketEvent, INFINITE );
			if( WAIT_OBJECT_0 == WaitResult )
			{
				if( 0 == pBufferQueue->ItemCount() )
				{
					// Sleep.
					pThis->ResetWaitEvent();
				}

				if( 0 < pBufferQueue->ItemCount() )
				{
					auto pIOCPData = pBufferQueue->Dequeue();
					if( pIOCPData )
					{
						bool isValidPacket = false;
						BasePacket* pPacket = reinterpret_cast<BasePacket*>(pIOCPData->buffer);
						if( pPacket )
						{
							if( pPacket->GetSize() > MAX_BUFFER_LEN )
							{
								PRINT_NORMAL_LOG( L"Current Size Bigger than MaxNetworkBufferLength : %d, %d\n",
												  pPacket->GetSize(), MAX_BUFFER_LEN );
							}

							auto type = static_cast<EPacketOperation>(pPacket->GetType());
							switch ( type )
							{
								case EPacketOperation::RegisterReq:
								{
									auto pRegisterReq = reinterpret_cast<ClientPacketRegisterReq*>(pPacket);
									if( true == pRegisterReq->CheckSize() )
									{
										isValidPacket = true;
										PRINT_NORMAL_LOG( L"Vaild Packet true!, RegisterReq\n" );
										pThis->ProcessRegister( pIOCPData->socket, pRegisterReq->AccountID, 
																pRegisterReq->Password, pRegisterReq->NickName );
									}
									break;
								}
								/*case EPacketOperation::LoginReq:
								{
									CPacketLoginReq *pLoginReq = (CPacketLoginReq *)pPacket;
									if( true == pLoginReq->CheckSize() )
									{
										isValidPacket = true;
										PRINT_NORMAL_LOG( L"Vaild Packet true!, LoginReq\n" );
										pThis->ProcessLogin( pIOCPData->socket, pLoginReq->AccountID, pLoginReq->Password );
									}
									break;
								}
								case EPacketOperation::LogoutReq:
								{
									CPacketLogoutReq *pLogoutReq = (CPacketLogoutReq *)pPacket;
									if( true == pLogoutReq->CheckSize() )
									{
										isValidPacket = true;
										PRINT_NORMAL_LOG( L"Vaild Packet true!, LogoutReq\n" );
										pThis->ProcessLogout( pIOCPData->socket, pLogoutReq->AccountUniqueID );
									}
									 break;
								}
								case EPacketOperation::ChatReq:
								{
									CPacketNormalChatReq *pChatReq = (CPacketNormalChatReq *)pPacket;
									if( true == pChatReq->CheckSize() )
									{
										isValidPacket = true;
										PRINT_NORMAL_LOG( L"Vaild Packet true!, ChatReq\n" );
										pThis->ProcessNormalChat( pIOCPData->socket, pChatReq->GetSenderAccountID(), pChatReq->GetChatText() );
									}
									break;
								}
								case EPacketOperation::WhisperChatReq:
								{
									CPacketWhisperChatReq *pWhisperChatReq = (CPacketWhisperChatReq *)pPacket;
									if( true == pWhisperChatReq->CheckSize() )
									{
										isValidPacket = true;
										PRINT_NORMAL_LOG( L"Vaild Packet true!, WhisperChatReq\n" );
										pThis->ProcessWhisperNormalChat( pIOCPData->socket,
																		 pWhisperChatReq->m_SenderAccountID,
																		 pWhisperChatReq->m_TargetAccountID,
																		 pWhisperChatReq->m_ChatText );

									}
									break;
								}*/
								default:
								{
									PRINT_NORMAL_LOG( L"Unknown Packet! Operation : %d\n", 
													  pPacket->GetType() );
									break;
								}
							}
						}
					}
				}

				pThis->ResetWaitEvent();
			}
			else
			{
				switch ( WaitResult )
				{
					case WAIT_ABANDONED_0:
					case WAIT_FAILED:
					{
						break;
					}
				}
			}
		}
	}

	return 0;
}

void PacketProcessor::RequestSend( SOCKET Socket, const void* pPacket, const size_t PacketSize )
{
	IOCPData data( Socket );
	memcpy( data.buffer, &pPacket, PacketSize );
	AuroraWinsockManager->RequestSendToIOCP( &data, PacketSize );
}

void PacketProcessor::RequestSendAllUser( const void* pPacket, const size_t PacketSize )
{
	for ( auto iter = userTables.begin(); iter != userTables.end(); ++iter )
	{
		auto pCurrentUser = iter->second;
		if( pCurrentUser )
		{
			RequestSend( pCurrentUser->GetSocket(), pPacket, PacketSize );
		}
	}
}

void PacketProcessor::RequestSendAllUserExcept( SOCKET Socket, const void* pPacket, const size_t PacketSize )
{
	for ( auto iter = userTables.begin(); iter != userTables.end(); ++iter )
	{
		auto pCurrentUser = iter->second;
		if( pCurrentUser )
		{
			if( pCurrentUser->GetSocket() == Socket )
			{
				continue;
			}

			RequestSend( pCurrentUser->GetSocket(), pPacket, PacketSize );
		}
	}
}

bool PacketProcessor::ProcessRegister( SOCKET socket, WCHAR* pAccountID, WCHAR* pPassword, WCHAR* pNickName )
{
	socket;
	if( pAccountID && pPassword && pNickName )
	{
		/*AccountDBWork DBWork;

		bool RegisterProcessResult = false;

		CPacketRegisterAck AckPacket;

		UInt32 AccountUniqueID = 0;
		bool DBResult = DBWork.NewAccount( pAccountID, pPassword, AccountUniqueID );
		if( true == DBResult )
		{
			DBResult = DBWork.NewPlayer( AccountUniqueID, pNickName );

			CPacketRegisterAck AckPacket;
			AckPacket.SetUniqueID( AccountUniqueID );
			AckPacket.SetAccountID( pAccountID );
			AckPacket.CalculateSize();

			const void* pSendPacket = const_cast<void*>( static_cast<void *>( &AckPacket ) );
			RequestSend( socket, pSendPacket, AckPacket.GetSize() );
			return RegisterProcessResult;
		}*/
	}
	
	return false;
}

bool PacketProcessor::ProcessLogin( SOCKET Socket, WCHAR* pAccountID, WCHAR* pPassword )
{
	if( pAccountID && pPassword )
	{
		//AccountDBWork DBWork;
		//UInt32 AccountUniqueID = 0;

		//CPacketLoginAck PacketLoginAck;
	
		//bool LoginProcessResult = false;
		//bool DBResult = DBWork.SelectAccountUniqueID( pAccountID, pPassword, AccountUniqueID );
		//if( true == DBResult && 0 < AccountUniqueID )
		//{	
		//	// Login DB Success!!
		//	PRINT_NORMAL_LOG( L"Logged-in User : %d\n", AccountUniqueID );

		//	// Already Login?
		//	if( true == FindUser( AccountUniqueID ) )
		//	{
		//		PRINT_NORMAL_LOG( L"Already-Logged-in User : %d\n", AccountUniqueID );
		//		LoginProcessResult = false;
		//		PacketLoginAck.SetResult( AuroraNetwork::ePacketResult_AlreadyExists );
		//	}
		//	else
		//	{
		//		PRINT_NORMAL_LOG( L"Login OK : %d\n", AccountUniqueID );

		//		LoginProcessResult = true;
		//		PacketLoginAck.SetAccountID( pAccountID );
		//		PacketLoginAck.SetResult( AuroraNetwork::ePacketResult_OK );

		//		ChatUserInfo *pLoginUser = new ChatUserInfo( AccountUniqueID, pAccountID, L"12345", socket );

		//		userTables.insert( std::make_pair( AccountUniqueID, pLoginUser ) );
		//		userTablesByAccountID.insert( std::make_pair( pAccountID, pLoginUser ) );

		//		/*userTables.insert( std::initializer_list<UInt32, ChatUserInfo *>::value_type( AccountUniqueID, pLoginUser ) );
		//		userTablesByAccountID.insert( stdext::hash_map<WCHAR* , ChatUserInfo *>::value_type( pAccountID, pLoginUser ) );*/
		//	}
		//}
		//else
		//{
		//	PRINT_NORMAL_LOG( L"Failed to Login, DBResult is false : [%s][%s][%d]\n", pAccountID, pPassword, AccountUniqueID );
		//}

		//PacketLoginAck.SetAccountUniqueID( AccountUniqueID );
		//PacketLoginAck.CalculateSize();

		//const void* pSendPacket = const_cast<void*>( static_cast<void *>( &PacketLoginAck ) );
		//RequestSend( socket, pSendPacket, PacketLoginAck.GetSize() );

		//if( ePacketResult_OK == PacketLoginAck.GetResult() )
		//{
		//	// notify to all user.
		//	CBroadcastPacketNotifyEvent BroadcastEvent;
		//	wsprintf( BroadcastEvent.GetNotifyMessage(), L"[Logged-In-User] %s", PacketLoginAck.GetAccountID() );
		//	BroadcastEvent.CalculateSize();

		//	RequestSendAllUser( (const void *)&BroadcastEvent, BroadcastEvent.GetSize() );
		//}

		//return LoginProcessResult;
	}

	return false;	
}

bool PacketProcessor::ProcessLogout( SOCKET Socket, UInt32 AccountUniqueID )
{
	//CPacketLogoutAck PacketLogoutAck;
	//PacketLogoutAck.SetAccountUniqueID( AccountUniqueID );

	//// Already Login?
	//bool LogoutProcessResult = false;
	//ChatUserInfo *pLogoutUser = GetUserInfo( AccountUniqueID );
	//if( nullptr != pLogoutUser && 0 < EraseUser( AccountUniqueID ) )
	//{
	//	// Logout!
	//	EraseUser( pLogoutUser->GetAccountID() );

	//	LogoutProcessResult = true;
	//	PacketLogoutAck.SetResult( AuroraNetwork::ePacketResult_OK );
	//}
	//else
	//{
	//	LogoutProcessResult = false;
	//	PacketLogoutAck.SetResult( AuroraNetwork::ePacketResult_NotFound );
	//}

	//PacketLogoutAck.CalculateSize();

	//const void* pSendPacket = const_cast<void*>(static_cast<void *>(&PacketLogoutAck));
	//RequestSend( socket, pSendPacket, PacketLogoutAck.GetSize() );

	//if( ePacketResult_OK == PacketLogoutAck.GetResult() )
	//{
	//	// notify to all user.
	//	CBroadcastPacketNotifyEvent BroadcastEvent;
	//	wsprintf( BroadcastEvent.GetNotifyMessage( ), L"[Logged-Out-User] UniqueID : %d", PacketLogoutAck.GetUniqueID() );
	//	BroadcastEvent.CalculateSize();

	//	RequestSendAllUser( (const void *)&BroadcastEvent, BroadcastEvent.GetSize() );
	//}

	//return LogoutProcessResult;

	return false;
}

bool PacketProcessor::ProcessNormalChat( SOCKET Socket, const WCHAR* pSenderAccountID, const WCHAR* pChatText )
{
	/*CBroadcastPacketNormalChat BroadcastChatPacket;
	BroadcastChatPacket.SetAccountID( pSenderAccountID );
	BroadcastChatPacket.SetChatText( pChatText );
	BroadcastChatPacket.CalculateSize();

	const void* pSendPacket = const_cast<void*>( static_cast<void *>( &BroadcastChatPacket ) );
	RequestSendAllUser( pSendPacket, BroadcastChatPacket.GetSize() );*/
	return true;
}

bool PacketProcessor::ProcessWhisperNormalChat( SOCKET Socket, const WCHAR* pSenderAccountID, const WCHAR* pTargetAccountID, const WCHAR* pChatText )
{
	//CPacketWhisperChatAck PacketWhisperChatAck;

	//// Already Login?
	//bool WhisperProcessResult = false;
	//ChatUserInfo *pTargetUser = GetUserInfo( const_cast<WCHAR* >( pTargetAccountID ) );
	//if( nullptr != pTargetUser )
	//{
	//	WhisperProcessResult = true;
	//	PacketWhisperChatAck.SetResult( AuroraNetwork::ePacketResult_OK );
	//}
	//else
	//{
	//	WhisperProcessResult = false;
	//	PacketWhisperChatAck.SetResult( AuroraNetwork::ePacketResult_NotFound );
	//}

	//PacketWhisperChatAck.CalculateSize();

	//const void* pSendPacket = const_cast<void*>( static_cast<void *>( &PacketWhisperChatAck ) );
	//RequestSend( socket, pSendPacket, PacketWhisperChatAck.GetSize( ) );

	//if( ePacketResult_OK == PacketWhisperChatAck.GetResult() )
	//{
	//	CBroadcastPacketNormalChat BroadcastChatPacket;
	//	BroadcastChatPacket.SetAccountID( pSenderAccountID );
	//	BroadcastChatPacket.SetChatText( pChatText );
	//	BroadcastChatPacket.CalculateSize();

	//	const void* pSendPacket = const_cast<void*>( static_cast<void *>( &BroadcastChatPacket ) );
	//	RequestSend( pTargetUser->GetSocket(), pSendPacket, BroadcastChatPacket.GetSize( ) );
	//}

	return true;
}

bool PacketProcessor::FindUser( UInt32 AccountUniqueID )
{
	auto findIter = userTables.find( AccountUniqueID );
	if( userTables.end() != findIter )
	{
		return true;
	}

	return false;
}

ChatUserInfo * PacketProcessor::GetUserInfo( UInt32 AccountUniqueID )
{
	auto findIter = userTables.find( AccountUniqueID );

	if( userTables.end() != findIter )
	{
		return findIter->second;
	}

	return nullptr;
}

bool PacketProcessor::FindUserByAccountID( WCHAR* pAccountID )
{
	for ( auto iter = userTablesByAccountID.begin(); iter != userTablesByAccountID.end(); ++iter )
	{
		if( iter->second )
		{
			auto pCurrentUser = iter->second;
			if( 0 == wcscmp( pAccountID, pCurrentUser->GetAccountID() ) )
			{
				return true;
			}
		}
	}

	return false;
}

ChatUserInfo* PacketProcessor::GetUserInfo( WCHAR* pAccountID )
{
	for ( auto iter = userTablesByAccountID.begin(); iter != userTablesByAccountID.end(); ++iter )
	{
		if( iter->second )
		{
			auto pCurrentUser = iter->second;
			if( 0 == wcscmp( pAccountID, pCurrentUser->GetAccountID() ) )
			{
				return pCurrentUser;
			}
		}
	}

	return nullptr;
}

size_t PacketProcessor::EraseUser( UInt32 AccountUniqueID )
{
	return userTables.erase( AccountUniqueID );
}

size_t PacketProcessor::EraseUser( WCHAR* pAccountID )
{
	for( auto iter = userTablesByAccountID.begin(); iter != userTablesByAccountID.end(); ++iter )
	{
		if( iter->second )
		{
			auto pCurrentUser = iter->second;
			if( 0 == wcscmp( pAccountID, pCurrentUser->GetAccountID() ) )
			{
				userTablesByAccountID.erase( iter );
				break;
			}
		}
	}

	return userTablesByAccountID.size();
}
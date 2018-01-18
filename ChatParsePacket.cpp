#pragma once
#include "../Aurora/Utility/StringManager.h"
#include "../Aurora/Utility/LogManager.h"

#include "../Aurora/Network/PacketBase.h"
#include "../Aurora/Network/IOCPManager.h"
#include "../Aurora/Network/NetworkManager.h"

#include "ChatUserInfo.h"
#include "ChatParsePacket.h"
#include "ChatPacketDefine.h"

PacketProcessor::PacketProcessor():
_IOCPDatas( NORMAL_QUEUE_SIZE ),
_runningParsePacketThread( true )
{
}

PacketProcessor::~PacketProcessor( void )
{
	userTables.erase( userTables.begin(), userTables.end() );

	for ( auto counter = 0; counter < _IOCPDatas.GetSize(); ++counter )
	{
		auto pIOCPData = _IOCPDatas.Dequeue();
		if( pIOCPData )
		{
			SafeDelete( pIOCPData );
		}
	}

	for( auto counter = 0; counter < _waitEventCount; ++counter )
	{
		CloseHandle( _parsePacketEvents[counter] );
		delete _parsePacketEvents[counter];
	}
}

void PacketProcessor::CreateEventHandles( UInt16 waitEventCount )
{
	_waitEventCount = waitEventCount;
	_parsePacketEvents = new HANDLE[_waitEventCount];
	for( auto i = 0; i < _waitEventCount; ++i )
	{
		_parsePacketEvents[i] = CreateEvent( NULL, TRUE, FALSE, NULL );
		if( _parsePacketEvents[i] == NULL )
		{
			//printf( "CreateEvent failed (%d)", GetLastError() );
		}
	}
}

UInt32 __stdcall PacketProcessor::ParsePacket( void* pArgs )
{
	if( pArgs )
	{
		auto pThis = reinterpret_cast<PacketProcessor*>(pArgs);
		auto pBufferQueue = pThis->GetIOCPDataQueue();

		while( true == pThis->_runningParsePacketThread )
		{
			int threadIndex = 0;
			auto WaitResult = WaitForMultipleObjects( pThis->_waitEventCount, pThis->_parsePacketEvents, 
													  FALSE, INFINITE );

			switch( WaitResult )
			{
				case WAIT_OBJECT_0:
					threadIndex = 0;
					break;
				case WAIT_OBJECT_0 + 1:
					threadIndex = 1;
					break;
				case WAIT_OBJECT_0 + 2:
					threadIndex = 2;
					break;
				case WAIT_OBJECT_0 + 3:
					threadIndex = 3;
					break;
				case WAIT_ABANDONED_0:
				case WAIT_FAILED:
				{
					continue;
				}
			}

			if( 0 == pBufferQueue->ItemCount() )
			{
				// Sleep.
				pThis->ResetWaitEvent( threadIndex );
			}

			if( 0 < pBufferQueue->ItemCount() )
			{
				auto pIOCPData = pBufferQueue->Dequeue();
				if( pIOCPData )
				{
					//bool isValidPacket = false;
					auto pPacket = reinterpret_cast<BasePacket*>(pIOCPData->buffer);
					if( pPacket )
					{
						if( pPacket->GetSize() > MAX_BUFFER_LEN )
						{
							AuroraLogManager->Error( L"Current Size Bigger than MaxNetworkBufferLength : %d, %d",
													 pPacket->GetSize(), MAX_BUFFER_LEN );
						}

						auto type = static_cast<EPacketOperation>(pPacket->GetType());
						switch( type )
						{
							case EPacketOperation::RegisterReq:
							{
								auto pRegisterReq = reinterpret_cast<CPacketRegisterReq*>(pPacket);
								if( true == pRegisterReq->CheckSize() )
								{
									AuroraLogManager->Trace( L"Vaild Packet true!, RegisterReq" );

									CPacketRegisterAck response;
									response.CalculateSize();

									if( 0 == (std::rand() % 2) )
									{
										Sleep( 10000 );
										pThis->RequestSend( pIOCPData->socket, (const void*)&response, response.GetSize() );
									}
								}
								break;
							}
							default:
							{
								AuroraLogManager->Error( L"Unknown Packet! Operation : %d", pPacket->GetType() );
								break;
							}
						}
					}
				}

				pThis->ResetWaitEvent( threadIndex );
			}
		}
	}

	return 0;
}

void PacketProcessor::RequestSend( SOCKET Socket, const void* pPacket, const size_t PacketSize )
{
	IOCPData data( Socket );
	memcpy( data.buffer, &pPacket, PacketSize );
	AuroraIOCPManager->RequestSendToIOCP( &data, PacketSize );
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
	Socket; pAccountID; pPassword;
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
		//	PRINT_NORMAL_LOG( L"Logged-in User : %d", AccountUniqueID );

		//	// Already Login?
		//	if( true == FindUser( AccountUniqueID ) )
		//	{
		//		PRINT_NORMAL_LOG( L"Already-Logged-in User : %d", AccountUniqueID );
		//		LoginProcessResult = false;
		//		PacketLoginAck.SetResult( AuroraNetwork::ePacketResult_AlreadyExists );
		//	}
		//	else
		//	{
		//		PRINT_NORMAL_LOG( L"Login OK : %d", AccountUniqueID );

		//		LoginProcessResult = true;
		//		PacketLoginAck.SetAccountID( pAccountID );
		//		PacketLoginAck.SetResult( AuroraNetwork::ePacketResult_OK );

		//		ChatUserInfo* pLoginUser = new ChatUserInfo( AccountUniqueID, pAccountID, L"12345", socket );

		//		userTables.insert( std::make_pair( AccountUniqueID, pLoginUser ) );
		//		userTablesByAccountID.insert( std::make_pair( pAccountID, pLoginUser ) );

		//		/*userTables.insert( std::initializer_list<UInt32, ChatUserInfo *>::value_type( AccountUniqueID, pLoginUser ) );
		//		userTablesByAccountID.insert( stdext::hash_map<WCHAR* , ChatUserInfo *>::value_type( pAccountID, pLoginUser ) );*/
		//	}
		//}
		//else
		//{
		//	PRINT_NORMAL_LOG( L"Failed to Login, DBResult is false : [%s][%s][%d]", pAccountID, pPassword, AccountUniqueID );
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
	Socket; AccountUniqueID;

	//CPacketLogoutAck PacketLogoutAck;
	//PacketLogoutAck.SetAccountUniqueID( AccountUniqueID );

	//// Already Login?
	//bool LogoutProcessResult = false;
	//ChatUserInfo* pLogoutUser = GetUserInfo( AccountUniqueID );
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
	Socket; pSenderAccountID; pChatText;

	/*CBroadcastPacketNormalChat BroadcastChatPacket;
	BroadcastChatPacket.SetAccountID( pSenderAccountID );
	BroadcastChatPacket.SetChatText( pChatText );
	BroadcastChatPacket.CalculateSize();

	const void* pSendPacket = const_cast<void*>( static_cast<void *>( &BroadcastChatPacket ) );
	RequestSendAllUser( pSendPacket, BroadcastChatPacket.GetSize() );*/
	return true;
}

bool PacketProcessor::ProcessWhisperNormalChat( SOCKET Socket, const WCHAR* pSenderAccountID, 
												const WCHAR* pTargetAccountID, const WCHAR* pChatText )
{
	Socket; pSenderAccountID; pTargetAccountID; pChatText;

	//CPacketWhisperChatAck PacketWhisperChatAck;

	//// Already Login?
	//bool WhisperProcessResult = false;
	//ChatUserInfo* pTargetUser = GetUserInfo( const_cast<WCHAR* >( pTargetAccountID ) );
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
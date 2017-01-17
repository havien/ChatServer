#pragma once

#include "../AuroraUtility/AuroraDefine.h"
#include "../AuroraUtility/Queue.h"

#include "ChatServerDBWork.h"

using namespace Aurora::Network;

class ChatUserInfo;

class PacketProcessor : public Singleton<PacketProcessor>
{
	friend class Singleton<PacketProcessor>;
private:
	PacketProcessor( void );
	NON_COPYABLE( PacketProcessor );
public:
	virtual ~PacketProcessor( void );

	static UInt32 __stdcall ParsePacket( void* pArgs );

	void RequestSend( SOCKET Socket, const void* pPacket, const size_t PacketSize );
	void RequestSendAllUser( const void* pPacket, const size_t PacketSize );
	void RequestSendAllUserExcept( SOCKET Socket, const void* pPacket, const size_t PacketSize );

	bool ProcessRegister( SOCKET Socket, WCHAR* pAccountID, WCHAR* pPassword, WCHAR* pNickName );
	bool ProcessLogin( SOCKET Socket, WCHAR* pAccountID, WCHAR* pPassword );
	bool ProcessLogout( SOCKET Socket, UInt32 AccountUniqueID );
	bool ProcessNormalChat( SOCKET Socket, const WCHAR* pSenderAccountID, const WCHAR* pChatText );
	bool ProcessWhisperNormalChat( SOCKET Socket, const WCHAR* pSenderAccountID, const WCHAR* pTargetAccountID, const WCHAR* pChatText );

	bool FindUser( UInt32 AccountUniqueID );
	ChatUserInfo * GetUserInfo( UInt32 AccountUniqueID );

	bool FindUserByAccountID( WCHAR* pAccountID );
	ChatUserInfo* GetUserInfo( WCHAR* pAccountID );

	size_t EraseUser( UInt32 AccountUniqueID );
	size_t EraseUser( WCHAR* pAccountID );

	inline const HANDLE* GetThreadEventHandle( void )
	{
		return &_parsePacketEvent;
	}

	inline void WakeupParsePacketThread( void )
	{
		SetEvent( _parsePacketEvent );
	}

	inline void ResetWaitEvent( void )
	{
		ResetEvent( _parsePacketEvent );
	}

	inline void StopParsePacketThread( void )
	{
		_runningParsePacketThread = false;
	}

	CQueue<IOCPData*>* GetIOCPDataQueue( void ) { return &_IOCPDatas; }
private:
	bool _runningParsePacketThread;
	HANDLE _parsePacketEvent;
	CQueue<IOCPData*> _IOCPDatas;

	std::unordered_map<UInt32, ChatUserInfo*> userTables;
	std::unordered_map<WCHAR*, ChatUserInfo*> userTablesByAccountID;
};

#define ChatPacketProcessor PacketProcessor::Instance()
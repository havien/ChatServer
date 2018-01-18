#pragma once

#include "../Aurora/Utility/AuroraDefine.h"
#include "../Aurora/Utility/Queue.h"

#include "ChatServerDBWork.h"

using namespace Aurora::Network;

class ChatUserInfo;

class PacketProcessor : public Singleton<PacketProcessor>
{
	friend class Singleton<PacketProcessor>;
private:
	PacketProcessor();
public:
	void CreateEventHandles( UInt16 waitEventCount );
	virtual ~PacketProcessor();
	static UInt32 __stdcall ParsePacket( void* pArgs );

	void RequestSend( SOCKET Socket, const void* pPacket, const size_t PacketSize );
	void RequestSendAllUser( const void* pPacket, const size_t PacketSize );
	void RequestSendAllUserExcept( SOCKET Socket, const void* pPacket, const size_t PacketSize );

	bool ProcessRegister( SOCKET Socket, WCHAR* pAccountID, WCHAR* pPassword, WCHAR* pNickName );
	bool ProcessLogin( SOCKET Socket, WCHAR* pAccountID, WCHAR* pPassword );
	bool ProcessLogout( SOCKET Socket, UInt32 AccountUniqueID );
	bool ProcessNormalChat( SOCKET Socket, const WCHAR* pSenderAccountID, const WCHAR* pChatText );
	bool ProcessWhisperNormalChat( SOCKET Socket, const WCHAR* pSenderAccountID, 
								   const WCHAR* pTargetAccountID, const WCHAR* pChatText );

	bool FindUser( UInt32 AccountUniqueID );
	ChatUserInfo * GetUserInfo( UInt32 AccountUniqueID );

	bool FindUserByAccountID( WCHAR* pAccountID );
	ChatUserInfo* GetUserInfo( WCHAR* pAccountID );

	size_t EraseUser( UInt32 AccountUniqueID );
	size_t EraseUser( WCHAR* pAccountID );

	inline const HANDLE* GetThreadEventHandle( void )
	{
		return _parsePacketEvents;
	}

	inline void WakeupParsePacketThread( int index )
	{
		SetEvent( _parsePacketEvents[index] );
	}

	inline void ResetWaitEvent( int index )
	{
		ResetEvent( _parsePacketEvents[index] );
	}

	inline void StopParsePacketThread( void )
	{
		_runningParsePacketThread = false;
	}

	CQueue<IOCPData*>* GetIOCPDataQueue(){ return &_IOCPDatas; }
private:
	bool _runningParsePacketThread;
	UInt16 _waitEventCount;
	HANDLE* _parsePacketEvents;
	CQueue<IOCPData*> _IOCPDatas;

	std::unordered_map<UInt32, ChatUserInfo*> userTables;
	std::unordered_map<WCHAR*, ChatUserInfo*> userTablesByAccountID;
};

#define ChatPacketProcessor PacketProcessor::Instance()
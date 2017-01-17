#pragma once
#include "../AuroraUtility/Includes.h"
#include "../AuroraUtility/AuroraDefine.h"
#include "../AuroraUtility/AuroraSingleton.h"

using namespace Aurora;

class ChattingManager : public Singleton<ChattingManager>
{
	friend class Singleton<ChattingManager>;
public:
	ChattingManager( void );
	virtual ~ChattingManager( void );

	bool SendSignupPacket( WCHAR* pAccountID, WCHAR* pPassword, WCHAR* pNickName );
	bool SendLoginPacket( WCHAR* pAccountID, WCHAR* pPassword );
	bool SendLogoutPacket( UInt32 accountUniqueID );
	bool SendNormalChat( WCHAR* pChatText );
	bool SendWhisperPacket( WCHAR* pSenderAccountID, WCHAR* pTargetAccountID, WCHAR* pChatText );
};

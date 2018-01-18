#pragma once
#include "../Aurora/Utility/Includes.h"
#include "../Aurora/Utility/AuroraDefine.h"
#include "../Aurora/Utility/AuroraSingleton.h"

using namespace Aurora;

class ChattingManager : public Singleton<ChattingManager>
{
	friend class Singleton<ChattingManager>;
public:
	ChattingManager();
	virtual ~ChattingManager();

	bool SendSignupPacket( WCHAR* pAccountID, WCHAR* pPassword, WCHAR* pNickName );
	bool SendLoginPacket( WCHAR* pAccountID, WCHAR* pPassword );
	bool SendLogoutPacket( UInt32 accountUniqueID );
	bool SendNormalChat( WCHAR* pChatText );
	bool SendWhisperPacket( WCHAR* pSenderAccountID, WCHAR* pTargetAccountID, WCHAR* pChatText );
};

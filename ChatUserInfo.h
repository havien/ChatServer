#pragma once

#include "../AuroraUtility/AuroraDefine.h"
#include "../AuroraNetwork/Includes.h"
#include "ChatDefine.h"

using namespace Aurora;

class ChatUserInfo
{
public:
	ChatUserInfo( const Int64 AccountUniqueID, const WCHAR* pAccountID, 
				  const WCHAR* pNickName, const SOCKET Socket );

	~ChatUserInfo( void );
			
	void Reset( void );

	inline const Int64 GetAccountUniqueID( void ) const { accountUniqueID; }
	inline const WCHAR* GetAccountID( void ) { return accountID; }
	inline const WCHAR* GetNickName( void ) { return nickName; }
	inline const SOCKET GetSocket( void ) const { return socket; }
private:
	Int64 accountUniqueID;
	WCHAR accountID[MAX_ACCOUNT_ID_LEN];
	WCHAR nickName[MAX_NICKNAME_LEN];
	SOCKET socket;
};
#pragma once

#include "../Aurora/Utility/AuroraDefine.h"
#include "../Aurora/Network/Includes.h"
#include "ChatDefine.h"

using namespace Aurora;

class ChatUserInfo
{
public:
	ChatUserInfo( const Int64 AccountUniqueID, const WCHAR* pAccountID, 
				  const WCHAR* pNickName, const SOCKET Socket );

	~ChatUserInfo();
			
	void Reset();

	inline const Int64 GetAccountUniqueID() const { return accountUniqueID; }
	inline const WCHAR* GetAccountID() const { return accountID; }
	inline const WCHAR* GetNickName() const { return nickName; }
	inline const SOCKET GetSocket() const { return socket; }
private:
	Int64 accountUniqueID;
	WCHAR accountID[MAX_ACCOUNT_ID_LEN];
	WCHAR nickName[MAX_NICKNAME_LEN];
	SOCKET socket;
};
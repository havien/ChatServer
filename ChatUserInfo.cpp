#pragma once
#include "../AuroraUtility/StringManager.h"
#include "ChatUserInfo.h"

using namespace Aurora;
using namespace Aurora::Network;

ChatUserInfo::ChatUserInfo( const Int64 AccountUniqueID, const WCHAR* pAccountID, 
							const WCHAR* pNickName, const SOCKET pSocket ) :
accountUniqueID( AccountUniqueID ), 
socket( pSocket )
{
	AuroraStringManager->ClearAndCopy( pAccountID, accountID, (MAX_ACCOUNT_ID_LEN - 1) );
	AuroraStringManager->ClearAndCopy( pNickName, nickName, (MAX_NICKNAME_LEN - 1) );
}

ChatUserInfo::~ChatUserInfo( void )
{

}

void ChatUserInfo::Reset( void )
{
	accountUniqueID = 0;
	socket = INVALID_SOCKET;

	AuroraStringManager->Clear( accountID, MAX_ACCOUNT_ID_LEN );
	AuroraStringManager->Clear( nickName, MAX_NICKNAME_LEN );
}
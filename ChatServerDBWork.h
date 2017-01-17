#pragma once

#include "../AuroraDatabase/DBWork.h"
#include "../AuroraDatabase/SQLiteManager.h"

using namespace Aurora;
using namespace Database;

class AccountDBWork 
{
	const UInt16 DBAccountIDLength = 20 + 1;
	const UInt16 DBPasswordLength = 16 + 1;
	const UInt16 DBNickNameLength = 12 + 1;
public:
	AccountDBWork( void );
	~AccountDBWork( void );

	bool NewAccount( WCHAR* pAccountID, WCHAR* pPassword, OUT Int64 &newAccountID );
	bool NewPlayer( UInt32 AccountUniqueID, WCHAR* pNickName );
	bool SelectAccountUniqueID( WCHAR* pAccountID, WCHAR* pPassword, UInt32 &accountID );
private:
	WCHAR _workTableName[MAX_WORK_TABLE_LEN];
};
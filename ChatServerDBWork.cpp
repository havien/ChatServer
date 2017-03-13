#pragma once

#include "../AuroraUtility/StringManager.h"
#include "../AuroraUtility/MiscManager.h"
#include "../AuroraDatabase/SQLiteManager.h"

#include "ChatServerDBWork.h"

AccountDBWork::AccountDBWork( void )
{
	AuroraStringManager->ClearAndCopy( L"GT_Account", _workTableName, MAX_WORK_TABLE_LEN );
}

AccountDBWork::~AccountDBWork( void )
{
}

bool AccountDBWork::NewAccount( WCHAR* pAccountID, WCHAR* pPassword, OUT Int64 &newAccountUniqueID )
{
	if( pAccountID && pPassword )
	{
		WCHAR wideBuffer[MAX_SQL_STRING_LEN] = {0};
		char multibyteBuffer[MAX_SQL_STRING_LEN] = {0};

		wsprintf( wideBuffer, L"INSERT INTO GT_Account( AccountID, Password ) VALUES( \'%s\', \'%s\' )", 
				  pAccountID, pPassword );

		AuroraStringManager->WideToMultiByte( wideBuffer, multibyteBuffer );

		bool result = AuroraSQLiteManager->Prepare( EQueryType::Insert, multibyteBuffer );
		if( true == result )
		{
			newAccountUniqueID = 0;
			result = AuroraSQLiteManager->ExecuteNonSelect();
			if( true == result )
			{
				newAccountUniqueID = AuroraSQLiteManager->GetLastInsertID();
			}

			PRINT_NORMAL_LOG( L"NewAccount Last Insert ID : %d\n", newAccountUniqueID );
			return result;
		}
	}

	return false;
}

bool AccountDBWork::NewPlayer( UInt32 AccountUniqueID, WCHAR* pNickName )
{
	if( pNickName )
	{
		WCHAR wideBuffer[MAX_SQL_STRING_LEN] = {0};
		char multiByteBuffer[MAX_SQL_STRING_LEN] = {0};

		wsprintf( wideBuffer, L"INSERT INTO GT_Player( AccountID, NickName ) VALUES( \'%d\', \'%s\' )", 
				  AccountUniqueID, pNickName );

		AuroraStringManager->WideToMultiByte( wideBuffer, multiByteBuffer );

		bool result = AuroraSQLiteManager->Prepare( EQueryType::Insert, multiByteBuffer );
		if( true == result )
		{
			result = AuroraSQLiteManager->ExecuteNonSelect();
			PRINT_NORMAL_LOG( L"NewPlayer Last Insert ID : %d\n", AuroraSQLiteManager->GetLastInsertID() );
			return result;
		}

	}
	return false;
}

bool AccountDBWork::SelectAccountUniqueID( WCHAR* pAccountID, WCHAR* pPassword, UInt32 &AccountUniqueID )
{
	if( pAccountID && pPassword )
	{
		WCHAR wideBuffer[MAX_SQL_STRING_LEN] = { 0 };
		char multiByteBuffer[MAX_SQL_STRING_LEN] = { 0 };

		wsprintf( wideBuffer, L"SELECT UniqueID FROM GT_Account WHERE AccountID = \'%s\' AND Password = \'%s\'", 
				  pAccountID, pPassword );

		AuroraStringManager->WideToMultiByte( wideBuffer, multiByteBuffer );

		bool TryPrepare = AuroraSQLiteManager->Prepare( EQueryType::Select, multiByteBuffer );
		if( true == TryPrepare )
		{
			AccountUniqueID = AuroraSQLiteManager->FetchSingleDataInt();
			return true;
		}

		return false;
	}

	return false;
}
#pragma once

#include "../AprilUtility/CstringManager.h"
#include "../AprilDatabase/SQLiteManager.h"

#include "stdafx.h"
#include "DBWork.h"

CAccountDBWork::CAccountDBWork( void )
{
	AprilUtility::CCstringManager::GetInstance()->MemsetAndCopyStringW( m_WorkTableName, MaxWorkTableLength, L"GT_Account" );
	m_pSQLiteManager = CSQLiteManager::GetInstance();
}

CAccountDBWork::~CAccountDBWork( void )
{
}

bool CAccountDBWork::Select( wchar_t *pExecuteString, void *pFetchData )
{
	if ( pExecuteString )
	{
		CCstringManager::GetInstance()->WCHARToCHAR( pExecuteString, m_TempChar );
		return m_pSQLiteManager->ExecuteSelect( m_TempChar );
	}
	
	return false;
}

bool CAccountDBWork::Update( wchar_t *pExecuteString, int &AffectedRowCount )
{
	return false;
}

bool CAccountDBWork::Insert( wchar_t *pExecuteString, int &AffectedRowCount )
{
	if ( pExecuteString )
	{
		bool DBResult = m_pSQLiteManager->Prepare( eDBWork_Insert, "INSERT INTO GT_Account( AccountID, Password ) VALUES( ?, ? )" );
		if ( true == DBResult )
		{
			DBResult = m_pSQLiteManager->BindParameter( eSQLiteBind_NVARCHAR, (void *)m_AccountID, strlen( m_AccountID ) );
			DBResult = m_pSQLiteManager->BindParameter( eSQLiteBind_NVARCHAR, (void *)m_Password, strlen( m_Password ) );
			return m_pSQLiteManager->Execute();
		}

	}
	return false;
}

bool CAccountDBWork::NewAccount( wchar_t *pAccountID, wchar_t *pPassword )
{
	if ( pAccountID && pPassword )
	{
		CCstringManager::GetInstance()->WCHARToCHAR( pAccountID, m_TempChar );
		CCstringManager::GetInstance()->MemsetAndCopyStringA( m_AccountID, AccountIDLength, m_TempChar );

		CCstringManager::GetInstance()->WCHARToCHAR( pPassword, m_TempChar );
		CCstringManager::GetInstance()->MemsetAndCopyStringA( m_Password, PasswordLength, m_TempChar );

		int AffectedCount = 0;
		Insert( pAccountID, AffectedCount );
	}

	return false;
}

bool CAccountDBWork::NewPlayer( wchar_t *pNickName )
{
	if ( pNickName )
	{

	}
	return false;
}
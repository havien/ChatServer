#pragma once

#include "../AprilDatabase/DBWork.h"

using namespace AprilDatabase;
class CAccountDBWork : public CBaseDBWork
{
private:
	virtual bool Select( wchar_t *pExecuteString, void *pFetchData );
	virtual bool Update( wchar_t *pExecuteString, int &AffectedRowCount );
	virtual bool Insert( wchar_t *pExecuteString, int &AffectedRowCount );
public:
	CAccountDBWork( void );
	~CAccountDBWork( void );

	bool NewAccount( wchar_t *pAccountID, wchar_t *pPassword );
	bool NewPlayer( wchar_t *pNickName );
private:
	CSQLiteManager *m_pSQLiteManager;
	char m_TempChar[NormalStringLen];

	char m_AccountID[AccountIDLength];
	char m_Password[PasswordLength];
};
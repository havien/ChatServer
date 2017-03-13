#pragma once
#include "../AuroraUtility/AuroraDefine.h"
#include "../AuroraUtility/MiscManager.h"
#include "../AuroraUtility/ThreadManager.h"

#include "../AuroraDatabase/ODBCManager.h"
#include "../AuroraDatabase/SQLiteManager.h"

#include "../AuroraNetwork/NetworkManager.h"
#include "../AuroraNetwork/WinsockManager.h"
#include "../AuroraNetwork/AuroraIOCPWorker.h"

#include "ChatServerDBWork.h"
#include "ChatParsePacket.h"

using namespace Aurora;
using namespace Aurora::Database;
using namespace Aurora::Network;

int _tmain( int argc, _TCHAR* argv[] )
{
	argc; argv;

	std::srand( (unsigned int)std::time( 0 ) );

	WCHAR localeBuffer[100] = { 0 };
	GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SENGLANGUAGE, localeBuffer, 100 );
	_tsetlocale( LC_ALL, localeBuffer );

	AuroraMiscManager->SetLogFile( L"..\\logs\\ChatServerLog.txt" );

	bool isConnected = AuroraSQLiteManager->ConnectToDB( "..\\Database\\ChatServer.db" );
	if( false == isConnected )
	{
		PRINT_FILE_LOG( L"Failed to Connect DB Server..!\n" );
		getchar();
		return 0;
	}

	AuroraWinsockManager->SetNetworkMode( ENetworkRunMode::Server );
	AuroraNetworkManager->SetNetworkMode( ENetworkRunMode::Server );
	AuroraNetworkManager->InitServerNetwork( 15541 );
	if( false == AuroraNetworkManager->StartServerNetwork() )
	{
		getchar();
		return 0;
	}
	
	auto processCount = AuroraMiscManager->GetProcessorCountUInt16();
	bool bInit = AuroraWinsockManager->InitIOCP( processCount );
	if( true == bInit )
	{
		ChatPacketProcessor->CreateEventHandles( (processCount / 2) );

		for( auto i = 0; i < (processCount / 2); ++i )
		{
			AuroraThreadManager->BeginThread( ChatPacketProcessor->ParsePacket, 
											  ChatPacketProcessor, 
											  nullptr );
		}
		

		PRINT_FILE_LOG( L"Success to Start Server..!\n" );

		AuroraWinsockManager->StartIOCPWorker( ChatPacketProcessor->GetIOCPDataQueue(),
											   ChatPacketProcessor->GetThreadEventHandle(),
											   (processCount/2),
											   false );
					

		//AuroraNetworkManager->AcceptClient();
		getchar();
			
		/*WaitForSingleObject( waitHandle, INFINITE );				
		CloseHandle( waitHandle );*/

		PRINT_NORMAL_LOG( L"Success to Terminate Server..!\n" );
	}
	else
	{
		PRINT_NORMAL_LOG( L"Failed to Start Server..\n" );
		return -1;
	}

	getchar();
	return 0;
}
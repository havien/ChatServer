#pragma once
#include <jsoncpp\json.h>
#pragma warning(disable:4099)
#include <fstream>

#include "../Aurora/Utility/AuroraDefine.h"
#include "../Aurora/Utility/MiscManager.h"
#include "../Aurora/Utility/WindowsConsoleManager.h"

#include "../Aurora/Utility/LogManager.h"
#include "../Aurora/Utility/TimeManager.h"
#include "../Aurora/Utility/ThreadManager.h"
#include "../Aurora/Utility/StringManager.h"

#include "../Aurora/Database/ODBCManager.h"
#include "../Aurora/Database/SQLiteManager.h"

#include "../Aurora/Network/NetworkManager.h"
#include "../Aurora/Network/WinsockManager.h"
#include "../Aurora/Network/IOCPManager.h"

#include "ChatServerDBWork.h"
#include "ChatParsePacket.h"

using namespace Aurora;
using namespace Aurora::Database;
using namespace Aurora::Network;

using namespace std;

volatile int tid = 0;
static UInt32 __stdcall LogTest( void* pArgs )
{
	::InterlockedIncrement( (LPLONG) &tid );
	for( auto i = 0; i < 200000; ++i )
	{
		AuroraLogManager->Trace( L"[LogTest Thread id :%d]", tid );
		AuroraLogManager->Error( L"[LogTest Thread id :%d]", tid );
		AuroraLogManager->Warning( L"[LogTest Thread id :%d]", tid );

		Sleep( 500 );
	}

	return 0;
}

int _tmain()
{
	AuroraTimeManager->InitRand();

	std::ifstream file( "ChatServer.json" );

	Json::Value root;
	Json::Reader reader;
	bool parsingSuccessful = reader.parse( file, root );
	if( !parsingSuccessful )
	{
		AuroraLogManager->Critical( L"Failed to parse configuration file [%S]",
									reader.getFormattedErrorMessages().c_str() );
		getchar();
		return 1;
	}

	EProtocol protocol = EProtocol::TCP;
	UInt16 port = static_cast<UInt16>(root["Port"].asInt());
	UInt16 maxClientCount = static_cast<UInt16>(root["MaxClientCount"].asInt());
	auto strProtocol = root["Protocol"].asString();
	if( "UDP" == strProtocol )
	{
		protocol = EProtocol::UDP;
	}

	AuroraConsoleManager->Init();
	AuroraConsoleManager->DisableEdit();

	WCHAR title[DEFAULT_NORMAL_STR_LEN] = { 0 };
	AuroraStringManager->FormatString( title, DEFAULT_NORMAL_STR_LEN,
									   L"Chat Server [%s]", AuroraTimeManager->GetNowString() );

	AuroraConsoleManager->SetTitle( title );

	/*WCHAR localeBuffer[100] = { 0 };
	GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SENGLANGUAGE, localeBuffer, 100 );
	_tsetlocale( LC_ALL, localeBuffer );*/

	/*bool isConnected = AuroraSQLiteManager->ConnectToDB( "..\\Database\\ChatServer.db" );
	if( false == isConnected )
	{
		AuroraLogManager->Critical( L"Failed to Connect DB Server..!" );
		getchar();
		return 0;
	}*/

	if( false == AuroraWinsockManager->InitWinsock() )
	{
		AuroraLogManager->Critical( L"error occured during in Initialize Winsock!" );
		return 1;
	}

	/*for( volatile int i = 0; i < 3; ++i )
	{
		AuroraThreadManager->BeginThread( LogTest,
										(PVOID)&i,
										  nullptr );
	}
*/
	UInt16 preSocketCount = 1;
	auto processCount = AuroraMiscManager->GetProcessorCountU16();
	auto init = AuroraIOCPManager->PrepareAccept( EProtocol::TCP, port, 1, preSocketCount );
	if( true == init )
	{
		AuroraLogManager->Trace( L"Success to PrepareAccept..!" );
		AuroraIOCPManager->PreAcceptSockets( preSocketCount );

		const UInt16 sendRecvThreadCount = (processCount / 2);
		if( false == AuroraIOCPManager->PrepareSendRecv( sendRecvThreadCount, maxClientCount ) )
		{
			AuroraLogManager->Critical( L"error occured during in PrepareSendRecv!" );
			return 1;
		}

		ChatPacketProcessor->CreateEventHandles( 1 );

		HANDLE acceptor = INVALID_HANDLE_VALUE;
		AuroraIOCPManager->BeginIOCPAcceptWorker( OUT acceptor, nullptr, true );

		bool useExFunc = true;
		if( true == useExFunc )
		{
			AuroraWinsockManager->LoadAcceptExFunc( AuroraIOCPManager->GetListenSocket() );
			AuroraWinsockManager->LoadAcceptExSockaddrFunc( AuroraIOCPManager->GetListenSocket() );
			AuroraWinsockManager->LoadDisconnectExFunc( AuroraIOCPManager->GetListenSocket() );
		}
		
		auto pGQCSHandle = new HANDLE[sendRecvThreadCount];
		AuroraIOCPManager->BeginSendRecvWorkers( ChatPacketProcessor->GetIOCPDataQueue(),
												 pGQCSHandle,
												 sendRecvThreadCount,
												 false );

		AuroraThreadManager->BeginThread( ChatPacketProcessor->ParsePacket,
										  ChatPacketProcessor,
										  nullptr );

		WaitForSingleObject( acceptor, INFINITE );
		WaitForMultipleObjects( sendRecvThreadCount, pGQCSHandle, TRUE, INFINITE );

		for( auto counter = 0; counter < sendRecvThreadCount; ++counter )
		{
			CloseHandle( pGQCSHandle[counter] );
		}

		SafeDeleteArray( pGQCSHandle );

		AuroraLogManager->Trace( L"Success to Terminate Server..!" );
	}
	else
	{
		AuroraLogManager->Critical( L"Failed to Start Server.." );
		return -1;
	}

	getchar();
	return 0;
}
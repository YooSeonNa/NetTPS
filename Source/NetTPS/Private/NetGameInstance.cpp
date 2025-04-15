// Fill out your copyright notice in the Description page of Project Settings.


#include "NetGameInstance.h"
#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/OnlineSubsystem.h"
#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/OnlineSessionSettings.h"
#include "NetTPS.h"
#include "../../../../Plugins/Online/OnlineBase/Source/Public/Online/OnlineSessionNames.h"

void UNetGameInstance::Init()
{
	Super::Init();

	if( auto subsys = IOnlineSubsystem::Get() )
	{
		// 서브시스템으로부터 세션인터페이스 가져오기
		sessionInterface = subsys->GetSessionInterface();

		sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UNetGameInstance::OnCreateSessionComplete);

		sessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UNetGameInstance::OnFindSessionsComplete);

		/*
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle,
		FTimerDelegate::CreateLambda([&]
										{
											CreateMySession(mySessionName, 10);
										}
									), 2.0f, false);
									*/

		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle,
									FTimerDelegate::CreateLambda([&]
									{
										FindOtherSession();
									}
									), 2.0f, false);
	}
}

void UNetGameInstance::CreateMySession(FString roomName, int32 playerCount)
{
	// 세션설정변수
	FOnlineSessionSettings sessionSettings;

	// 1. Dedicated Server 접속여부
	sessionSettings.bIsDedicated = false;

	// 2. 랜선(로컬)매칭을 할지 Steam 매칭을 할지 여부
	FName subsysName = IOnlineSubsystem::Get()->GetSubsystemName();
	sessionSettings.bIsLANMatch = ( subsysName == "NULL" );

	// 3.매칭이 온라인을 통해 노출될지 여부
	// false 이면 초대를 통해서만 입장이 가능
	// SendSessionInviteToFriend() 함수를 통해 친구초대를 할 수 있다
	sessionSettings.bShouldAdvertise = true;

	// 4. 온라인 상태(Presence) 정보를 활용할지 여부
	sessionSettings.bUsesPresence = true;
	sessionSettings.bUseLobbiesIfAvailable = true; // <<=====

	// 5. 게임진행중에 참여 허가할지 여부
	sessionSettings.bAllowJoinViaPresence = true;
	sessionSettings.bAllowJoinInProgress = true;

	// 6. 세션에 참여할 수 있는 공개(public) 연결의 최대 허용 수
	sessionSettings.NumPublicConnections = playerCount;

	// 7. 커스텀 룸네임 설정
	sessionSettings.Set(FName("ROOM_NAME"), roomName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	// 8. 호스트네임 설정
	sessionSettings.Set(FName("HOST_NAME"), mySessionName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	// netID
	FUniqueNetIdPtr netID = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();

	PRINTLOG(TEXT("Create Session Start : %s"), *mySessionName);
	sessionInterface->CreateSession(*netID, FName(mySessionName), sessionSettings);

}

void UNetGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	PRINTLOG(TEXT("SessionName : %s, bWasSuccessful : %d"), *SessionName.ToString(), bWasSuccessful);
}

void UNetGameInstance::FindOtherSession()
{
	sessionSearch = MakeShareable(new FOnlineSessionSearch());

	// 1. 세션 검색 조건 설정
	sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	// 2. Lan 여부
	sessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == FName("NULL");

	// 3. 최대 검색 세션 수
	sessionSearch->MaxSearchResults = 10;

	// 4. 세션검색
	sessionInterface->FindSessions(0, sessionSearch.ToSharedRef());
}

void UNetGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	// 찾기 실패시
	if( bWasSuccessful == false )
	{
		PRINTLOG(TEXT("Session search failed..."));
		return;
	}

	// 세션검색결과 배열
	auto results = sessionSearch->SearchResults;
	PRINTLOG(TEXT("Search Result Count : %d"), results.Num());

	for( int i = 0; i < results.Num(); i++ )
	{
		auto sr = results[i];
		if( sr.IsValid() == false )
		{
			continue;
		}

		// 세션정보 구조체선언
		FSessionInfo sessionInfo;
		sessionInfo.index = i;

		sr.Session.SessionSettings.Get(FName("ROOM_NAME"), sessionInfo.roomName);
		
		sr.Session.SessionSettings.Get(FName("HOST_NAME"), sessionInfo.hostName);

		// 입장가능한 플레이어수
		int32 maxPlayerCount = sr.Session.SessionSettings.NumPublicConnections;
		// 현재 입장한 플레이어 수 ( 최대 - 현재 입장 가능한 수 )
		// NumOpenPublicConnections 스팀에서만 정상적으로 값이 들어온다
		int32 currentPlayerCount = maxPlayerCount - sr.Session.NumOpenPublicConnections;

		sessionInfo.playerCount = FString::Printf(TEXT("(%d/%d)"), currentPlayerCount, maxPlayerCount);

		// 핑 정보( 스팀에서는 9999로 나온다 )
		sessionInfo.pingSpeed = sr.PingInMs;

		PRINTLOG(TEXT("%s"), *sessionInfo.ToString());
	}
	/*
	// 정보를 가져온다
	for( auto sr : results )
	{
		// 정보가 유효한지 체크
		if( sr.IsValid() == false )
		{
			continue;
		}

		FString roomName;
		sr.Session.SessionSettings.Get(FName("ROOM_NAME"), roomName);
		FString hostName;
		sr.Session.SessionSettings.Get(FName("HOST_NAME"), hostName);
		// 세션주인(방장) 이름
		FString userName = sr.Session.OwningUserName;
		// 입장가능한 플레이어수
		int32 maxPlayerCount = sr.Session.SessionSettings.NumPublicConnections;
		// 현재 입장한 플레이어 수 ( 최대 - 현재 입장 가능한 수 )
		// NumOpenPublicConnections 스팀에서만 정상적으로 값이 들어온다
		int32 currentPlayerCount = maxPlayerCount - sr.Session.NumOpenPublicConnections;

		// 핑 정보( 스팀에서는 9999로 나온다 )
		int32 pingSpeed = sr.PingInMs;

		PRINTLOG(TEXT("%s : %s(%s) - (%d/%d), %dms"), *roomName, *hostName, *userName, currentPlayerCount, maxPlayerCount, pingSpeed);
	}
	*/
}

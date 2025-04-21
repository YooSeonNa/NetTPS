// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerController.h"
#include "NetTPSGameMode.h"
#include "GameFramework/SpectatorPawn.h"
#include "NetGameInstance.h"

void ANetPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if( HasAuthority() )
	{
		gm = Cast<ANetTPSGameMode>(GetWorld()->GetAuthGameMode());
	}

	if( IsLocalController() )
	{
		auto gi = Cast<UNetGameInstance>(GetWorld()->GetGameInstance());
		ServerRPC_ChangePlayer(gi->bTypeA);
	}
}

void ANetPlayerController::ServerRPC_RespawnPlayer_Implementation()
{
	auto player = GetPawn();
	UnPossess();
	player->Destroy();

	gm->RestartPlayer(this);
}

void ANetPlayerController::ServerRPC_ChangeToSpectator_Implementation()
{
	// 관전자가 플레이어의 위치에 생성될 수 있도록 플레이어 정보를 가져온다.
	APawn* player = GetPawn();

	if( player )
	{
		// 관전자를 생성
		FActorSpawnParameters params;
		params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		auto spectator = GetWorld()->SpawnActor<ASpectatorPawn>(gm->SpectatorClass, player->GetActorTransform(), params);

		// 빙의(Possess)
		Possess(spectator);

		// 이전 플레이어는 제거
		player->Destroy();

		// 5초후에 리스폰 시키기
		FTimerHandle handle;

		GetWorldTimerManager().SetTimer(handle, this, &ANetPlayerController::ServerRPC_RespawnPlayer_Implementation, 5.0f, false);
	}
}

void ANetPlayerController::ServerRPC_ChangePlayer_Implementation(bool bTypeA)
{
	// TypeA라면 Manny로 교체
	if( bTypeA == false )
	{
		auto oldPawn = GetPawn();
		UnPossess();
		APawn* newPawn = GetWorld()->SpawnActor<APawn>(MannyFactory, oldPawn->GetActorTransform());
		Possess(newPawn);

		oldPawn->Destroy();
	}
}

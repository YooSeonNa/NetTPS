// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerAnimInstance.h"
#include "NetTPSCharacter.h"
#include "MathUtil.h"

void UNetPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	player = Cast<ANetTPSCharacter>(TryGetPawnOwner()); 
}

void UNetPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if( player )
	{
		speed = FVector::DotProduct(player->GetVelocity(), player->GetActorForwardVector());

		direction = FVector::DotProduct(player->GetVelocity(), player->GetActorRightVector());

		// ȸ���� ����
		pitchAngle = -player->GetBaseAimRotation().GetNormalized().Pitch;
		pitchAngle = FMathf::Clamp(pitchAngle, -60.0f, 60.0f);


		// �Ѽ��� ���� ����
		bHasPistol = player->bHasPistol;
	}
}

void UNetPlayerAnimInstance::PlayFireAnimation()
{
	if( bHasPistol && FireMontage )
	{
		Montage_Play(FireMontage);
	}
}



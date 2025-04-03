// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "NetTPSCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ANetTPSCharacter : public ACharacter
{
	GENERATED_BODY()

	// ���� �ڽ����� ���� ������Ʈ
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* GunComp;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* TakePistolAction;

	// �ʿ�Ӽ� : �� ��������
	bool bHasPistol = false;

	// �������� ��
	UPROPERTY()
	AActor* ownedPistol = nullptr;

	// �� �˻� ����
	UPROPERTY(EditAnywhere, Category = Gun)
	float DistanceToGun = 200.0f;

	// ���忡 ��ġ�� �ѵ�
	UPROPERTY()
	TArray<AActor*> pistolActors;


	void TakePistol(const FInputActionValue& Value);

	// ���� ������Ʈ�� ���̱�
	void AttachPistol(AActor* pistolActor);


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ReleaseAction;

	// �� �����Է� ó�� �Լ�
	void ReleasePistol(const FInputActionValue& Value);

	// ���� ������Ʈ���� �и�
	void DetachPistol(AActor* pistolActor);



	// �� ��� �Է¾׼�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* FireAction;

	// �� ��� ó�� �Լ�
	void Fire(const FInputActionValue& Value);

	// �ǰ� ��ƼŬ
	UPROPERTY(EditDefaultsOnly, Category = Gun)
	class UParticleSystem* GunEffect;



	// ����� ���� Ŭ����
	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<class UMainUI> mainUIWidget;

	// mainUIWidget���� ���� ������� �ν��Ͻ�
	UPROPERTY()
	class UMainUI* mainUI;

	// UI �ʱ�ȭ �Լ�
	void InitUIWidget();



public:
	ANetTPSCharacter();
	
	virtual void BeginPlay() override;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			

protected:

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};


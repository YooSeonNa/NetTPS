// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainUI.generated.h"

/**
 * 
 */
UCLASS()
class NETTPS_API UMainUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = UI, meta=(BindWidget))
	class UImage*	img_Crosshair;
	
	// ũ�ν���� on/off ó��
	void ShowCrosshair(bool isShow);
};

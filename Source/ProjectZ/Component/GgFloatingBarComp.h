// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "GgFloatingBarComp.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTZ_API UGgFloatingBarComp : public UWidgetComponent
{
	GENERATED_BODY()

private:
	class AActor* OwningActor;         

	class UUserWidget*  UserWidget;
	class UProgressBar* HpProgressBar;

	float ShowTime;

public:
	// 생성자 함수
	UGgFloatingBarComp();

	// 소멸자 함수
	virtual ~UGgFloatingBarComp();

	// Begin 함수
	virtual void BeginPlay() override;

	// Tick 함수
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	// HP바 업데이트 함수
	void UpdateHpBar( float InPercent );
};

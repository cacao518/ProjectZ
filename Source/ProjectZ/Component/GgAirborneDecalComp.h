// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ETC/GgConst.h"
#include "Components/DecalComponent.h"
#include "GgAirborneDecalComp.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTZ_API UGgAirborneDecalComp : public UDecalComponent
{
	GENERATED_BODY()

private:
	FCharacterPtr OwningCharacter;

public:
	// 생성자 함수
	UGgAirborneDecalComp();

	// 소멸자 함수
	virtual ~UGgAirborneDecalComp();

	// Begin 함수
	virtual void BeginPlay() override;

	// Tick 함수
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;
};

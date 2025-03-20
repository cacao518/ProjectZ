// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../ETC/SDB.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GgMaterialComp.generated.h"


class UMaterialInterface;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTZ_API UGgMaterialComp final : public UActorComponent
{
	GENERATED_BODY()

public:
	// 초기 물질
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay )
	UMaterialInterface* InitMaterial;             

	// 물질 상태
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Gameplay )
	EMaterialState      MatState;       	     

private:
	ACharacter*         OwningCharacter;          // 부모 캐릭터 클래스

public:	
	// 생성자
	UGgMaterialComp();

	// 소멸자
	virtual ~UGgMaterialComp();

	// Begin 함수
	virtual void BeginPlay() override;

	// Tick 함수
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	///////////////////////////////////////////////////////////////////////
	// Setter
	///////////////////////////////////////////////////////////////////////
	
	// 물질을 변경한다.
	void SetMatState( UMaterialInterface* InMatInterface = nullptr, bool InIsInit = false );

	///////////////////////////////////////////////////////////////////////
	// Getter
	///////////////////////////////////////////////////////////////////////

	// 현재 물질 상태를 반환한다.
	EMaterialState GetMatState() { return MatState; };

	// 강도를 반환한다.
	float GetIntensity();

	// 중간 강도 인지 여부를 반환한다.
	bool IsMiddleIntensity();

	// 고 강도 인지 여부를 반환한다.
	bool IsHardIntensity();

private:
	// 초기화 한다.
	void _Init();

	// 머티리얼에 맞는 능력치를 초기화한다.
	void _InitStatus();

	// 물질 변경 이펙트를 실행한다.
	void _PlayChangeEffect();

	// 풀재질 관련 로직을 실행한다.
	void _ProcessGrass( float InDeltaTime );
};

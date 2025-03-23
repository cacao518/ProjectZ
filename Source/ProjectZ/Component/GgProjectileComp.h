// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GgObjectComp.h"
#include "GgProjectileComp.generated.h"


UCLASS( ClassGroup = ( Custom ), meta = ( BlueprintSpawnableComponent ) )
class PROJECTZ_API UGgProjectileComp final : public UGgObjectComp
{
	GENERATED_BODY()

public:
	// 공격 콜리전 정보
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay )
	FCollisionData     AttackCollData;         

	// 지속 시간
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay )
	float LifeTime = 0;                             

	// 투사체 방향
	FVector Direction;                          

private:
	float LifeTimeAcc = 0;

public:
	// 생성자 함수
	UGgProjectileComp();

	// 소멸자 함수
	virtual ~UGgProjectileComp();

	// Begin 함수
	virtual void BeginPlay() override;

	// Tick 함수
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	// 공격 성공 처리를 한다.
	virtual void OnAttackSuccess() override;

private:
	// 초기화 한다.
	void _Init();

	// 이동 관련 로직을 수행한다.
	void _ProcessMove();

	// 시간 관련 로직을 수행한다.
	void _ProcessTime( float InDeltaTime );
};

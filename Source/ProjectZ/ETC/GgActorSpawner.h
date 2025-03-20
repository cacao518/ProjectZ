// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Manager/GgObjectManager.h"
#include "GameFramework/Actor.h"
#include "GgActorSpawner.generated.h"

UCLASS()
class PROJECTZ_API AGgActorSpawner : public AActor
{
	GENERATED_BODY()
	
public:
	///< 스폰할 액터
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Gameplay )
	UClass* ActorClass;              

	///< 스폰 간격 ( 해당 초 간격으로 반복 생성 )
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Gameplay )
	float  SpawnInterval;           

	///< 스폰 최대 수 ( 전체 )
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Gameplay )
	int    SpawnCountMaxTotal;      

	///< 스폰 최대 수 ( 한 맵에서 )
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Gameplay )
	int    SpawnCountMaxInWorld;    

private:
	float  SpawnIntervalCount;     ///< 스폰 간격 카운트
	int    SpawnCountTotal;        ///< 스폰 수 ( 전체 )
	int    SpawnCountInWorld;      ///< 스폰 수 ( 한 맵에서 ) 
	
public:	
	AGgActorSpawner();

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// 스폰 가능한지 여부를 확인한다.
	bool CanSpawn();

	// 소환할 액터를 반환한다.
	UClass* GetActorClass(){ return ActorClass; };

	void AddSpawnIntervalCount( float InDeltaTime ){ SpawnIntervalCount += InDeltaTime; };
	void ResetSpawnIntervalCount(){ SpawnIntervalCount = 0; };
	void AddSpawnCountTotal(){ SpawnCountTotal++; };
	void AddSpawnCountInWorld(){ SpawnCountInWorld++; };
	void SubSpawnCountInWorld(){ SpawnCountInWorld--; };
};

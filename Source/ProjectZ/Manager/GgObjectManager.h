// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ETC/GgActorSpawner.h"
#include "ETC/GgConst.h"
#include "GgSingletonBase.h"
#include "CoreMinimal.h"
#include <unordered_map>
#include <list>


class AGgActorSpawner;
class UNiagaraSystem;


using ActorMap         = TMap< int, FActorPtr >;
using SpawnerMap       = TMap< int, FActorSpawnerPtr >;
using ActorSpawnerList = TArray< FActorSpawnerPtr >;


class FGgObjectManager final : public FGgSingletonBase< FGgObjectManager >
{
private:
	UPROPERTY()
	ActorMap         Objects;      // 모든 오브젝트 리스트

	UPROPERTY()
	ActorSpawnerList SpawnerList;  // 모든 스포너 리스트

	UPROPERTY()
	SpawnerMap       SpawnerMap;   // key:소환된액터 아이디, value:스포너
	
	int ObjectId;                  // 오브젝트 고유 아이디

public:
	FGgObjectManager();
	~FGgObjectManager();

	// 틱 함수
	virtual void Tick( float InDeltaTime ) override;

	///////////////////////////////////////////////////////////////
	/// 오브젝트
	///////////////////////////////////////////////////////////////
	
	// 액터 생성
	AActor* SpawnActor( UClass* InClass, const FVector& InLocation, const FRotator& InRotator, ETeamType InTeamType = ETeamType::MAX, FActorSpawnerPtr InSpawner = nullptr );

	// 파티클 생성
	void SpawnParticle( TSoftObjectPtr<UNiagaraSystem> InEffectPath, const FActorPtr InUseActor, const FVector& InLocation, const FRotator& InRotator, const FVector InParam = FVector() );

	// 파티클 생성
	void SpawnParticle( TSoftObjectPtr<UNiagaraSystem> InEffectPath, USceneComponent* InSceneComp, const FTransform InTrasnform, const FVector InParam = FVector() );

	// 액터 제거
	void DestroyActor( FActorPtr InActor );

	// 리스트를 정리한다.
	void Clear();

	// 에디터에서 소환된 액터를 등록한다.
	void RegisterActorInEditor( FActorPtr InActor );

	///////////////////////////////////////////////////////////////
	/// 스포너
	///////////////////////////////////////////////////////////////
	
	//// 스포너 등록 함수
	void RegisterSpawner( FActorSpawnerPtr InSpawner );

	//// 스포너에서 스폰한다.
	void SpawnActorInSpawner( float InDeltaTime );

};
inline FGgObjectManager& GetGgObjectManager() { return FGgObjectManager::GetInstance(); };
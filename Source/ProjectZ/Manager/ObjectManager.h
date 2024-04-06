// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../ETC/ActorSpawner.h"
#include "../ETC/SDB.h"
#include "SingletonBase.h"
#include "CoreMinimal.h"
#include <unordered_map>
#include <list>


class AActorSpawner;
class UNiagaraSystem;


using ActorPtr         = TWeakObjectPtr< AActor >;
using ActorSpawnerPtr  = TWeakObjectPtr< AActorSpawner >;

using ActorMap         = TMap< int, ActorPtr >;
using SpawnerMap       = TMap< int, ActorSpawnerPtr >;
using ActorSpawnerList = TArray< ActorSpawnerPtr >;


class ObjectManager final : public SingletonBase< ObjectManager>
{
private:
	UPROPERTY()
	ActorMap         Objects;      // ��� ������Ʈ ����Ʈ

	UPROPERTY()
	ActorSpawnerList SpawnerList;  // ��� ������ ����Ʈ

	UPROPERTY()
	SpawnerMap       SpawnerMap;   // key:��ȯ�Ⱦ��� ���̵�, value:������
	
	int ObjectId;                  // ������Ʈ ���� ���̵�

public:
	ObjectManager();
	~ObjectManager();

	// ƽ �Լ�
	void Tick( float InDeltaTime );

	///////////////////////////////////////////////////////////////
	/// ������Ʈ
	///////////////////////////////////////////////////////////////
	
	// ���� ����
	AActor* SpawnActor( UClass* InClass, const FVector& InLocation, const FRotator& InRotator, ETeamType InTeamType = ETeamType::MAX, AActorSpawner* InSpawner = nullptr );

	// ���� ������Ʈ ����
	AActor* SpawnStaticObject( const FString& InName, const FVector& InLocation, const FRotator& InRotator );

	// ��ƼŬ ����
	void SpawnParticle( const FString& InEffectName, const AActor* InUseActor, const FVector& InLocation, const FRotator& InRotator );

	// ��ƼŬ ����
	void SpawnParticle( UNiagaraSystem* InEffect, const AActor* InUseActor, const FVector& InLocation, const FRotator& InRotator );

	// ���� ����
	void DestroyActor( AActor* InActor );

	// ����Ʈ�� �����Ѵ�.
	void Clear();

	// �����Ϳ��� ��ȯ�� ���͸� ����Ѵ�.
	void RegisterActorInEditor( AActor* InActor );

	///////////////////////////////////////////////////////////////
	/// ������
	///////////////////////////////////////////////////////////////
	
	// ������ ��� �Լ�
	void RegisterSpawner( AActorSpawner* InSpawner);

	// �����ʿ��� �����Ѵ�.
	void SpawnActorInSpawner( float InDeltaTime );

};
inline ObjectManager& GetObjectManager() { return ObjectManager::GetInstance(); };
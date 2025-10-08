
#include "GgObjectManager.h"
#include "Engine/World.h"
#include "Actor/GgCharacterPC.h"
#include "Actor/GgCharacterNPC.h"
#include "Actor/GgStaticObject.h"
#include "System/GgGameInstance.h"
#include "Component/GgObjectComp.h"
#include "Components/SceneComponent.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "UObject/ConstructorHelpers.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Actor.h"


FGgObjectManager::FGgObjectManager()
{
	ObjectId = 1;
}

FGgObjectManager::~FGgObjectManager()
{
	Clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 틱 함수
/////////////////////////////////////////////////////////////////////////////////////////////////////
void FGgObjectManager::Tick( float InDeltaTime )
{
	SpawnActorInSpawner( InDeltaTime );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 액터 생성
/////////////////////////////////////////////////////////////////////////////////////////////////////
AActor* FGgObjectManager::SpawnActor( UClass* InClass, const FVector& InLocation, const FRotator& InRotator, ETeamType InTeamType, FActorSpawnerPtr InSpawner )
{
	const auto& actor = Objects.FindRef( ObjectId );
	if( actor.IsValid() )
	{
		actor->SetActorLocationAndRotation( InLocation, InRotator );
		return actor.Get();
	}
	else
	{
		//FString path = TEXT( "/Game/CharacterNPC/" );
		//UClass* pClass = LoadClass< AActor >( path );
		//if( !pClass )
		//	return nullptr;

		UWorld* World = GetGgGameInstance().GetWorld();
		if( !World )
			return nullptr;

		FTransform spawnTransform( InRotator, InLocation );
		AActor* newActor = World->SpawnActorDeferred< AActor >( InClass, spawnTransform );
		if( !newActor )
			return nullptr;

		auto objectComp = newActor ? newActor->FindComponentByClass<UGgObjectComp>() : nullptr;
		if( objectComp )
		{
			objectComp->SetObjId( ObjectId );
			objectComp->SetTeamType( InTeamType );
			objectComp->SetIsSpawnedInEditor( false );
		}

		if( InSpawner.IsValid() )
			SpawnerMap.Add( ObjectId, FActorSpawnerPtr( InSpawner ) );

		Objects.Add( ObjectId, newActor );

		ObjectId++;

		newActor->FinishSpawning( spawnTransform );

		return newActor;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 파티클 생성
/////////////////////////////////////////////////////////////////////////////////////////////////////
void FGgObjectManager::SpawnParticle( TSoftObjectPtr<UNiagaraSystem> InEffectPath, const FActorPtr InUseActor, const FVector& InLocation, const FRotator& InRotator, const FVector InParam /*= FVector() */)
{
	if( InEffectPath.IsNull() ) return;

	UNiagaraSystem* effect = InEffectPath.IsValid() ? InEffectPath.Get() : InEffectPath.LoadSynchronous();
	if( !effect ) return;

	UNiagaraComponent* niagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
		effect,
		InUseActor->GetRootComponent(),
		NAME_None,
		InLocation,
		InRotator,
		EAttachLocation::KeepWorldPosition,
		true,
		true,
		ENCPoolMethod::None
	);

	if( niagaraComp )
	{
		niagaraComp->SetVariableVec3( FName( "ParamVec" ), InParam );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 파티클 생성
/////////////////////////////////////////////////////////////////////////////////////////////////////
void FGgObjectManager::SpawnParticle( TSoftObjectPtr<UNiagaraSystem> InEffectPath, USceneComponent* InSceneComp, const FTransform InTrasnform, const FVector InParam /*= FVector() */ )
{
	if( InEffectPath.IsNull() ) return;

	UNiagaraSystem* effect = InEffectPath.IsValid() ? InEffectPath.Get() : InEffectPath.LoadSynchronous();
	if( !effect ) return;

	UNiagaraComponent* niagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
		effect,
		InSceneComp,
		NAME_None,
		InTrasnform.GetLocation(),
		InTrasnform.GetRotation().Rotator(),
		EAttachLocation::KeepWorldPosition,
		true,
		true,
		ENCPoolMethod::None
	);

	if( niagaraComp )
	{
		niagaraComp->SetVariableVec3( FName( "ParamVec" ), InParam );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 액터 제거 함수
/////////////////////////////////////////////////////////////////////////////////////////////////////
void FGgObjectManager::DestroyActor( FActorPtr InActor )
{
	auto objectComp = InActor.IsValid() ? InActor->FindComponentByClass<UGgObjectComp>() : nullptr;
	if( !objectComp )
		return;

	if( Objects.Find( objectComp->GetObjId() ) )
	{
		InActor->Destroy();
		Objects.Remove( objectComp->GetObjId() );

		const auto& spawner = SpawnerMap.FindRef( objectComp->GetObjId() );
		if( spawner.IsValid() )
		{
			spawner.Get()->SubSpawnCountInWorld();
			spawner.Get()->ResetSpawnIntervalCount();
		}
	}
	else
	{
		if( InActor.IsValid() )
			InActor->Destroy();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 리스트를 정리한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void FGgObjectManager::Clear()
{
	Objects.Empty();
	SpawnerList.Empty();
	SpawnerMap.Empty();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 에디터에서 소환된 액터를 등록한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void FGgObjectManager::RegisterActorInEditor( FActorPtr InActor )
{
	auto objectComp = InActor.IsValid() ? InActor->FindComponentByClass<UGgObjectComp>() : nullptr;
	if( !objectComp )
		return;

	if( Objects.Find( objectComp->GetObjId() ) )
		return;

	objectComp->SetObjId( ObjectId );
	objectComp->SetTeamType( ETeamType::MAX );

	Objects.Add( ObjectId, InActor );

	ObjectId++;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 스포너 등록 함수
/////////////////////////////////////////////////////////////////////////////////////////////////////
void FGgObjectManager::RegisterSpawner( FActorSpawnerPtr InSpawner )
{
	if( !InSpawner.IsValid() )
		return;

	SpawnerList.Add( InSpawner );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 스포너에서 스폰한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void FGgObjectManager::SpawnActorInSpawner( float InDeltaTime )
{
	for( const auto& spawner : SpawnerList )
	{
		if( !spawner.IsValid() )
			continue;

		spawner->AddSpawnIntervalCount( InDeltaTime );

		if( spawner->CanSpawn() )
		{
			SpawnActor( spawner->GetActorClass(), spawner->GetActorLocation(), spawner->GetActorRotation(), ETeamType::MAX, spawner.Get() );

			spawner->AddSpawnCountTotal();
			spawner->AddSpawnCountInWorld();
			spawner->ResetSpawnIntervalCount();
		}
	}
}

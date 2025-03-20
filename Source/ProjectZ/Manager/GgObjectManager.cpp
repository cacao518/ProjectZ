
#include "GgObjectManager.h"
#include "Engine/World.h"
#include "Actor/GgCharacterPC.h"
#include "Actor/GgCharacterNPC.h"
#include "Actor/GgStaticObject.h"
#include "System/GgGameInstance.h"
#include "Component/GgObjectComp.h"
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
AActor* FGgObjectManager::SpawnActor( UClass* InClass, const FVector& InLocation, const FRotator& InRotator, ETeamType InTeamType, AGgActorSpawner* InSpawner )
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
			objectComp->SetId( ObjectId );
			objectComp->SetTeamType( InTeamType );
			objectComp->SetIsSpawnedInEditor( false );
		}

		if( InSpawner )
			SpawnerMap.Add( ObjectId, ActorSpawnerPtr( InSpawner ) );

		Objects.Add( ObjectId, newActor );

		ObjectId++;

		newActor->FinishSpawning( spawnTransform );

		return newActor;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 정적 오브젝트 생성
/////////////////////////////////////////////////////////////////////////////////////////////////////
AActor* FGgObjectManager::SpawnStaticObject( const FString& InName, const FVector& InLocation, const FRotator& InRotator )
{
	FString path = FString( TEXT( "/Game/Blueprints/StaticObject/" ) ) + InName;
	UClass* staticObject = ConstructorHelpersInternal::FindOrLoadClass( path, AGgStaticObject::StaticClass() );
	if( !staticObject )
		return nullptr;

	return SpawnActor( staticObject, InLocation, InRotator, ETeamType::MAX );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 파티클 생성
/////////////////////////////////////////////////////////////////////////////////////////////////////
void FGgObjectManager::SpawnParticle( const FString& InEffectName, const AActor* InUseActor, const FVector& InLocation, const FRotator& InRotator )
{
	FString path = FString( TEXT( "/Game/Particle/" ) ) + InEffectName;
	UNiagaraSystem* effect = LoadObject<UNiagaraSystem>( NULL, *path, NULL, LOAD_None, NULL);
	if( !effect )
		return;

	SpawnParticle( effect, InUseActor, InLocation, InRotator );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 파티클 생성
/////////////////////////////////////////////////////////////////////////////////////////////////////
void FGgObjectManager::SpawnParticle( UNiagaraSystem* InEffect, const AActor* InUseActor, const FVector& InLocation, const FRotator& InRotator )
{
	UNiagaraFunctionLibrary::SpawnSystemAttached( InEffect, InUseActor->GetRootComponent(), NAME_None, InLocation, InRotator, EAttachLocation::KeepWorldPosition, true, true, ENCPoolMethod::None );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 액터 제거 함수
/////////////////////////////////////////////////////////////////////////////////////////////////////
void FGgObjectManager::DestroyActor( AActor* InActor )
{
	auto objectComp = InActor ? InActor->FindComponentByClass<UGgObjectComp>() : nullptr;
	if( !objectComp )
		return;

	if( Objects.Find( objectComp->GetId() ) )
	{
		InActor->Destroy();
		Objects.Remove( objectComp->GetId() );

		const auto& spawner = SpawnerMap.FindRef( objectComp->GetId() );
		if( spawner.IsValid() )
		{
			spawner.Get()->SubSpawnCountInWorld();
			spawner.Get()->ResetSpawnIntervalCount();
		}
	}
	else
	{
		if( InActor )
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
void FGgObjectManager::RegisterActorInEditor( AActor* InActor )
{
	auto objectComp = InActor ? InActor->FindComponentByClass<UGgObjectComp>() : nullptr;
	if( !objectComp )
		return;

	if( Objects.Find( objectComp->GetId() ) )
		return;

	objectComp->SetId( ObjectId );
	objectComp->SetTeamType( ETeamType::MAX );

	Objects.Add( ObjectId, InActor );

	ObjectId++;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 스포너 등록 함수
/////////////////////////////////////////////////////////////////////////////////////////////////////
void FGgObjectManager::RegisterSpawner( AGgActorSpawner* InSpawner )
{
	if( !InSpawner )
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

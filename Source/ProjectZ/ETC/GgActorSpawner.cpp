
#include "GgActorSpawner.h"
#include "Manager/GgObjectManager.h"

AGgActorSpawner::AGgActorSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	SpawnCountTotal = 0;
	SpawnCountInWorld = 0;
}

void AGgActorSpawner::BeginPlay()
{
	Super::BeginPlay();

	GetGgObjectManager().RegisterSpawner( this );
}

void AGgActorSpawner::BeginDestroy()
{
	Super::BeginDestroy();
}

void AGgActorSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 스폰 가능한지 여부를 확인한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
bool AGgActorSpawner::CanSpawn()
{
	if( SpawnInterval <= SpawnIntervalCount )
	{
		if( SpawnCountTotal < SpawnCountMaxTotal && SpawnCountInWorld < SpawnCountMaxInWorld )
			return true;
	}

	return false;
}

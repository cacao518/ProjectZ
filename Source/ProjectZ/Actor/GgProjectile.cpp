// Fill out your copyright notice in the Description page of Project Settings.


#include "GgProjectile.h"
#include "Component/GgProjectileComp.h"
#include "Components/BoxComponent.h"

// Sets default values
AGgProjectile::AGgProjectile()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>( TEXT( "RootComp" ) );

	// ObjectComp
	ProjectileComp = CreateDefaultSubobject<UGgProjectileComp>( TEXT( "ProjectileComp" ) );

	// Box Component
	HitColl = CreateDefaultSubobject<UBoxComponent>( TEXT( "HitColl" ) );
	HitColl->SetupAttachment( RootComponent );
	HitColl->SetCollisionProfileName( TEXT( "HitColl" ) );

	// AttackBox Component
	AttackColl = CreateDefaultSubobject<UBoxComponent>( TEXT( "AttackColl" ) );
	AttackColl->SetupAttachment( RootComponent );
	AttackColl->SetCollisionProfileName( TEXT( "AttackColl" ) );
}

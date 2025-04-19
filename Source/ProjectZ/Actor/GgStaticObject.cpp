// Fill out your copyright notice in the Description page of Project Settings.


#include "GgStaticObject.h"
#include "Component/GgObjectComp.h"
#include "Components/BoxComponent.h"
#include "ProceduralMeshComponent.h"


// Sets default values
AGgStaticObject::AGgStaticObject()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>( TEXT( "RootComp" ) );

	// ObjectComp
	ObjectComp = CreateDefaultSubobject<UGgObjectComp>( TEXT( "ObjectComp" ) );

	// Box Component
	HitColl = CreateDefaultSubobject<UBoxComponent>( TEXT( "HitColl" ) );
	HitColl->SetupAttachment( RootComponent );
	HitColl->SetCollisionProfileName( TEXT( "HitColl" ) );

	// AttackBox Component
	AttackColl = CreateDefaultSubobject<UProceduralMeshComponent>( TEXT( "AttackColl" ) );
	AttackColl->SetupAttachment( RootComponent );
	AttackColl->SetCollisionProfileName( TEXT( "AttackColl" ) );
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "GgAirborneDecalComp.h"
#include "System/GgAnimInstance.h"
#include "System/GgGameInstance.h"
#include "GameFramework/Character.h"


UGgAirborneDecalComp::UGgAirborneDecalComp()
{
	PrimaryComponentTick.bCanEverTick = true;
}

UGgAirborneDecalComp::~UGgAirborneDecalComp()
{

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief Begin
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgAirborneDecalComp::BeginPlay()
{
	Super::BeginPlay();

	OwningCharacter = Cast<ACharacter>( GetOwner() );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief Tick
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgAirborneDecalComp::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if( !OwningCharacter.IsValid() ) return;

	UGgAnimInstance* animInstance = Cast<UGgAnimInstance>( OwningCharacter->GetMesh()->GetAnimInstance() );
	if( !animInstance )
		return;

	if( animInstance->IsAirborne )
	{
		SetHiddenInGame( false );

		FVector start = OwningCharacter->GetActorLocation();
		FVector end = start - FVector( 0, 0, 5000 );

		FHitResult hitResult;
		FCollisionQueryParams traceParameters( FName( TEXT( "" ) ), false, OwningCharacter.Get() ); //Tag, Bool Trace Complex, Ignore 액터 (자신 제외)
		
		bool bHit = GetWorld()->LineTraceSingleByChannel(
			hitResult,
			start,
			end,
			ECC_Visibility,
			traceParameters
		);

		if( bHit )
		{
			SetWorldLocation( hitResult.ImpactPoint );
		}

		FRotator currentRotation = GetComponentRotation();
		currentRotation.Yaw += 100.f * DeltaTime;
		SetWorldRotation( currentRotation );
	}
	else
	{
		if( !bHiddenInGame )
		{
			SetHiddenInGame( true );
		}
	}
}
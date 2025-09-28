// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_Launch.h"
#include "ETC/GgConst.h"
#include "Component/GgCharacterComp.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

FString UAnimNotify_Launch::GetNotifyName_Implementation() const
{
	return Super::GetNotifyName_Implementation();
}

void UAnimNotify_Launch::Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation )
{
	if( !MeshComp )
		return;

	ACharacter* owner = Cast<ACharacter>( MeshComp->GetOwner() );
	if( !owner )
		return;

	UGgCharacterComp* obj = MeshComp->GetOwner()->FindComponentByClass<UGgCharacterComp>();
	if ( !obj )
		return;

	const FVector vec = ( CONST::MAX_MASS - obj->GetStat().Weight ) * Vec;

	if( bUseOwnerBaseDir )
	{
		FVector forward = owner->GetActorForwardVector();
		FVector right = owner->GetActorRightVector();
		FVector up = owner->GetActorUpVector();

		FVector direction = ( forward * vec.X ) + ( right * vec.Y ) + ( up * vec.Z );
		owner->LaunchCharacter( direction, true, true );
	}
	else
	{
		owner->LaunchCharacter( vec, true, true );
	}
}
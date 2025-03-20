// Fill out your copyright notice in the Description page of Project Settings.


#include "GameUI.h"
#include "Component/GgObjectComp.h"
#include "Component/GgWeaponComp.h"
#include "Component/GgCharacterComp.h"
#include "Actor/GgCharacterPC.h"
#include "System/GgAnimInstance.h"
#include "System/GgGameInstance.h"
#include "Components/ProgressBar.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief NativeConstruct
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGameUI::NativeConstruct()
{
	Super::NativeConstruct();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief NativeTick
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGameUI::NativeTick( const FGeometry& MyGeometry, float InDeltaTime )
{
	Super::NativeTick( MyGeometry, InDeltaTime );

	_UpdateProgressBar();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief BeginDestroy
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGameUI::BeginDestroy()
{
	Super::BeginDestroy();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief UI 생성 완료됨을 알린다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGameUI::OnCreated()
{
	ProgressBarHP      = Cast<UProgressBar>( GetWidgetFromName( TEXT( "ProgressBarHP" ) ) );
	ProgressBarMP      = Cast<UProgressBar>( GetWidgetFromName( TEXT( "ProgressBarMP" ) ) );
	ProgressBarWeapon1 = Cast<UProgressBar>( GetWidgetFromName( TEXT( "ProgressBarWeapon1" ) ) );
	ProgressBarWeapon2 = Cast<UProgressBar>( GetWidgetFromName( TEXT( "ProgressBarWeapon2" ) ) );
	ProgressBarWeapon3 = Cast<UProgressBar>( GetWidgetFromName( TEXT( "ProgressBarWeapon3" ) ) );

	AGgCharacterPC* myPlayer = GetGgGameInstance().GetMyPlayer();
	if( !myPlayer )
		return;

	MyPlayerCharComp   = myPlayer->FindComponentByClass<UGgCharacterComp>();
	MyPlayerWeaponComp = myPlayer->FindComponentByClass<UGgWeaponComp>();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 프로그레스바를 업데이트한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGameUI::_UpdateProgressBar()
{
	if( !MyPlayerCharComp || !MyPlayerWeaponComp )
		return;

	if( ProgressBarHP )
		ProgressBarHP->SetPercent( MyPlayerCharComp->Stat.Hp / MyPlayerCharComp->Stat.Hpm );

	if( ProgressBarMP )
		ProgressBarMP->SetPercent( MyPlayerCharComp->Stat.Mp / MyPlayerCharComp->Stat.Mpm );

	//if( ProgressBarWeapon1 )
	//	ProgressBarWeapon1->SetPercent( MyPlayerGgWeaponComp->GetWeaponCoolTimePercent( EWeaponState::SWORD ) );

	//if( ProgressBarWeapon2 )
	//	ProgressBarWeapon2->SetPercent( MyPlayerGgWeaponComp->GetWeaponCoolTimePercent( EWeaponState::AXE ) );

	//if( ProgressBarWeapon3 )
	//	ProgressBarWeapon3->SetPercent( MyPlayerGgWeaponComp->GetWeaponCoolTimePercent( EWeaponState::SPEAR ) );
}

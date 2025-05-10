// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ETC/GgConst.h"
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include <functional>
#include <map>
#include "GgPlayerController.generated.h"


class AGgCharacterPC;
class UGgCharacterComp;
class UGgWeaponComp;


UCLASS()
class PROJECTZ_API AGgPlayerController : public APlayerController
{
	GENERATED_BODY()
	
private:
	using InputMap = TMap< EInputKeyType, function<void()> >;

private:
	AGgCharacterPC*          MyPlayer;
	UGgCharacterComp*        CharacterComp;
	UGgWeaponComp*           WeaponComp;

	InputMap                 InputTypeAndFuncMap;  // Key:입력키종류, Value:입력키 함수
	float                    ReadySkillResetTime;  // 발동 대기중인 스킬 초기화 시간
	function<void()>         ReadySkillFunc;       // 발동 대기중인 마지막으로 입력한 스킬 함수
	EInputKeyType            ReadySkillInputKey;   // 발동 대기중인 마지막으로 입력한 키 종류
						     
	float				     BaseTurnRate;
	float				     BaseLookUpRate;
	bool				     IsDownBothMoustButton;

public:
	AGgPlayerController();

public:
	virtual void BeginPlay() override;
	virtual void Tick( float InDeltaTime ) override;

	void MoveForward( float Value );
	void MoveRight( float Value );
	void TurnAtRate( float Rate );
	void LookUpAtRate( float Rate );
	void JumpStart();
	void JumpStop();
	void DashStart();
	void DashStop();

	void ProcessLeftMouse();
	void ProcessRightMouse();
	void ProcessBothMouseDown();
	void ProcessBothMouseUp();
	void ProcessWheelClick();
	void ProcessWheelUp();
	void ProcessWheelDown();
	void ProcessSpace();
	void ProcessTab();
	void ProcessF();
	void ProcessR();
	void Process1();
	void Process2();
	void Process3();

private:
	// 스킬을 재생한다.
	bool _SkillPlay( int InSkillNum );

	// 발동 대기중 스킬 초기화
	void _ResetReadySkill();

	// 발동 대기중 스킬 설정
	void _SetReadySkill( EInputKeyType InReadyInputKey );

	// 발동 대기중인 스킬 수행
	void _ProcessReadySkill( float InDeltaTime );

	// 회전 비율 값을 셋팅한다.
	void _ProcessRotationRate();

	// 다중 키 입력을 받는다.
	void _ProcessInputBothKey();
};

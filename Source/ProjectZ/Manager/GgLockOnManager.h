// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ETC/GgConst.h"
#include "GgSingletonBase.h"
#include "CoreMinimal.h"
#include <unordered_map>
#include <list>

enum class ELockOnMode : uint8
{
	Default,
	Prev,
	Next,
	Max
};

class FGgLockOnManager final : public FGgSingletonBase< FGgLockOnManager >
{

private:
	FCharacterPtr LockOnTarget;         // 락온 된 대상 

public:
	FGgLockOnManager();
	~FGgLockOnManager();

	// 틱 함수
	virtual void Tick( float InDeltaTime ) override;

	// 록온 시작
	void LockOnStart( ELockOnMode InMode = ELockOnMode::Default );

	// 록온 해제
	void LockOnRelease();

	// 락온 된 대상을 반환한다.
	FCharacterPtr GetLockOnTarget() { return LockOnTarget; };

private:
	// 락온 기능 수행
	void _ProcessLockOn( float InDeltaTime );

};
inline FGgLockOnManager& GetGgLockOnManager() { return FGgLockOnManager::GetInstance(); };
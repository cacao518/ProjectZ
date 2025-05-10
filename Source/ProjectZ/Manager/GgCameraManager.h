// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ETC/GgConst.h"
#include "GgSingletonBase.h"
#include "CoreMinimal.h"
#include <unordered_map>
#include <list>


class FGgCameraManager final : public FGgSingletonBase< FGgCameraManager>
{

public:
	FGgCameraManager();
	~FGgCameraManager();

	// 틱 함수
	virtual void Tick( float InDeltaTime ) override;

	// 카메라 쉐이크를 실행한다.
	void CameraShake( AActor* InCaster, float InScale = 1.f, bool InShakeByWeight = false, bool InShakeByIntensity = false );

private:
	// 카메라 암 로직을 수행한다.
	void _ProcessCameraArm( float InDeltaTime );

};
inline FGgCameraManager& GetGgCameraManager() { return FGgCameraManager::GetInstance(); };
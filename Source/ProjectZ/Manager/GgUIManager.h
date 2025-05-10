// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ETC/GgConst.h"
#include "GgSingletonBase.h"
#include "CoreMinimal.h"
#include <unordered_map>
#include <list>


class UUIBase;


class FGgUIManager final : public FGgSingletonBase< FGgUIManager>
{

public:
	FGgUIManager();
	~FGgUIManager();

	// 틱 함수
	void Tick( float InDeltaTime );

	// UI를 생성하고 띄운다.
	UUIBase* ShowUI( const FString& InPath );

};
inline FGgUIManager& GetGgUIManager() { return FGgUIManager::GetInstance(); };
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/GameEngine.h"
#include "GgGameInstance.generated.h"


class AGgCharacterPC;


UCLASS()
class PROJECTZ_API UGgGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UGgGameInstance();
	~UGgGameInstance();

	virtual void Init() override;
	virtual void Shutdown() override;

	// 자신의 플레이어를 반환한다.
	AGgCharacterPC* GetMyPlayer();

	// 자신의 컨트롤러를 반환한다.
	APlayerController* GetPlayerController();

	// 인스턴스를 반환한다.
	static UGgGameInstance* GetInstance();
};

inline UGgGameInstance& GetGgGameInstance() 
{ 
	return *( UGgGameInstance::GetInstance() );
};
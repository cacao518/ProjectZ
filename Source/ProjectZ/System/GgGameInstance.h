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

	// �ڽ��� �÷��̾ ��ȯ�Ѵ�.
	AGgCharacterPC* GetMyPlayer();

	// �ڽ��� ��Ʈ�ѷ��� ��ȯ�Ѵ�.
	APlayerController* GetPlayerController();

	// �ν��Ͻ��� ��ȯ�Ѵ�.
	static UGgGameInstance* GetInstance();
};

inline UGgGameInstance& GetGgGameInstance() 
{ 
	return *( UGgGameInstance::GetInstance() );
};
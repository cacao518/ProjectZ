// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ETC/SDB.h"
#include "CoreMinimal.h"

template< typename Type >
class FGgSingletonBase
{
private:
	static Type* Instance;

public:
	FGgSingletonBase(){ Instance = static_cast< Type* >( this ); };
	virtual ~FGgSingletonBase(){ Instance = nullptr; };
	virtual void Tick( float InDeltaTime ) {};
	static Type& GetInstance(){ return *Instance; }
	static Type* CreateInstance() { return !Instance ? Instance = new Type() : Instance; }
	static void DestroyInstance() { if( Instance ) delete Instance; Instance = nullptr; }
	static bool IsVaild(){ return Instance != nullptr; }
};

template< typename Type >
Type* FGgSingletonBase< Type >::Instance = nullptr;
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetComponent.generated.h"

namespace Net
{
	class CManager;
}
class CGameBuffer;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CARS_API UNetComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNetComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetID(unsigned int _uID) { m_uID = _uID; }

	void SerializeData();

	void DeserializeData(CGameBuffer* pData);

	void SetInput(const FVector2D& _vInput) { m_vMovementInput = _vInput; }

private:
	Net::CManager* m_pManager;
	unsigned int m_uID;
	FVector2D m_vMovementInput;
};

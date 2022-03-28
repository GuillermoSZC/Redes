// Fill out your copyright notice in the Description page of Project Settings.


#include "GameNet/NetComponent.h"
#include "Net/Manager.h"
#include "DrawDebugHelpers.h"
#include "GameNet/GameBuffer.h"
#include "Game/CarMovementComponent.h"
#include "Game/Car.h"

// Sets default values for this component's properties
UNetComponent::UNetComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	m_pManager = Net::CManager::getSingletonPtr();
	// ...
}


// Called when the game starts
void UNetComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UNetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (m_pManager->getID() == Net::ID::SERVER)
	{
		DrawDebugString(GetWorld(), FVector::ZeroVector, *FString("Real"),
			GetOwner(), FColor::Blue, 0.02f);
		SerializeData();
	}
	else if(m_pManager->getID() == m_uID)
	{
		DrawDebugString(GetWorld(), FVector::ZeroVector, *FString("Owner"),
			GetOwner(), FColor::Green, 0.02f);
		SerializeData();
	}
	else
	{
		DrawDebugString(GetWorld(), FVector::ZeroVector, *FString("Other"),
			GetOwner(), FColor::Red, 0.02f);
	}
}

void UNetComponent::SerializeData()
{
	CGameBuffer oData;
	Net::NetMessageType eMType = Net::NetMessageType::ENTITY_MSG;
	oData.write(eMType);
	oData.write(m_uID);
	if (m_pManager->getID() == Net::ID::SERVER)
	{
		FTransform vTrans = GetOwner()->GetActorTransform();
		oData.write(vTrans);
		m_pManager->send(&oData, false);
	}
	else
	{
		oData.write(m_vMovementInput);
		m_pManager->send(&oData, false);
	}
}

void UNetComponent::DeserializeData(CGameBuffer* pData)
{
	ACar* pCar = GetOwner<ACar>();
	if (m_pManager->getID() == Net::ID::SERVER)
	{
		FVector2D vInput;
		pData->read(vInput);
		pCar->GetCarMovementComponent()->SetInput(vInput);
	}
	else
	{
		FTransform vTrans;
		pData->read(vTrans);
		pCar->SetActorTransform(vTrans);
	}
}

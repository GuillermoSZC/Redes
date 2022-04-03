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

    isTrapSpawn = false;
    trapPosition = FVector::ZeroVector;
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
    else if (m_pManager->getID() == m_uID)
    {
        DrawDebugString(GetWorld(), FVector::ZeroVector, *FString("Owner"),
            GetOwner(), FColor::Green, 0.02f);
        SerializeData();
        if (isTrapSpawn == true)
        {
            DataSpawn();
            isTrapSpawn = false;
            trapPosition = FVector::ZeroVector;
        }
    }
    else
    {
        DrawDebugString(GetWorld(), FVector::ZeroVector, *FString("Other"),
            GetOwner(), FColor::Red, 0.02f);
    }
}

void UNetComponent::SerializeData()
{
    ACar* car = GetOwner<ACar>();
    if (car)
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
}

void UNetComponent::DeserializeData(CGameBuffer* pData)
{
    ACar* car = GetOwner<ACar>();
    if (car)
    {
        if (m_pManager->getID() == Net::ID::SERVER)
        {
            FVector2D vInput;
            pData->read(vInput);
            car->GetCarMovementComponent()->SetInput(vInput);
        }
        else
        {
            FTransform vTrans;
            pData->read(vTrans);
            car->SetActorTransform(vTrans);
        }
    }
}
#pragma region GETTERS_AND_SETTERS
unsigned int UNetComponent::GetID()
{
    return m_uID;
}

bool UNetComponent::GetTrapSpawned()
{
    return isTrapSpawn;
}

FVector UNetComponent::GetTrapPosition()
{
    return trapPosition;
}

void UNetComponent::SetTrapSpawn(bool _spawned)
{
    isTrapSpawn = _spawned;
}

void UNetComponent::SetTrapPosition(FVector _position)
{
    trapPosition = _position;
}
#pragma endregion

void UNetComponent::DataSpawn()
{
    CGameBuffer buffer;
    Net::NetMessageType messageType = Net::NetMessageType::TRAP_MSG;
    buffer.write(messageType);
    buffer.write(m_uID);

    if (m_pManager->getID() == Net::ID::SERVER)
    {
        FTransform trapTransform = GetOwner()->GetActorTransform();
        buffer.write(trapTransform);
        m_pManager->send(&buffer, false);
    }
    else
    {
        buffer.write(isTrapSpawn);
        buffer.write(trapPosition);
        m_pManager->send(&buffer, true);
    }
}

void UNetComponent::DestroyTrap(unsigned char _car)
{
    CGameBuffer buffer;
    Net::NetMessageType typeMessage = Net::NetMessageType::DESTROY_TRAP;
    buffer.write(typeMessage);
    buffer.write(m_uID);
    m_pManager->send(&buffer, true);
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Trap.generated.h"

class UStaticMeshComponent;
class UNetComponent;

UCLASS()
class CARS_API ATrap : public AActor
{
    GENERATED_BODY()

public:
    ATrap();
    ~ATrap() { UE_LOG(LogTemp, Log, TEXT("Trap deleted")); }

public:


protected:
    virtual void BeginPlay() override;
    // @TODO: private
    UPROPERTY(EditAnywhere)
        UStaticMeshComponent* meshComponent;

    UPROPERTY(EditAnywhere)
        UNetComponent* netComponent;

private:
    unsigned char trapID;
    FVector temp; // @WARN!
     // @WARN!

public:
    virtual void Tick(float DeltaTime) override;

#pragma region GETTERS_&_SETTERS
    UNetComponent* GetNetComponent();
    UStaticMeshComponent* GetMesh();
    unsigned char GetTrapID();

    void SetNetComponent(UNetComponent* _netComponent);
    void SetMesh(UStaticMeshComponent* _meshComponent);
    void SetTrapID(unsigned char _trapID);
#pragma endregion

};

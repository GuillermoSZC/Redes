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

public:

private:
    unsigned char trapID;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere)
        UStaticMeshComponent* meshComponent;

    UPROPERTY(EditAnywhere)
        UNetComponent* netComponent;

public:
    virtual void Tick(float DeltaTime) override;

    UNetComponent* GetNetComponent() { return netComponent; }

    unsigned char GetTrapID();
    void SetTrapID(unsigned char _id);

};

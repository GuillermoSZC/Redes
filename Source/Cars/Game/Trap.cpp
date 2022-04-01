// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Trap.h"
#include <Components/StaticMeshComponent.h>
#include <ConstructorHelpers.h>
#include "GameNet/NetComponent.h"
#include <Kismet/GameplayStatics.h>
#include "Car.h"
#include "GameNet/GameNetMgr.h"

ATrap::ATrap()
{
    PrimaryActorTick.bCanEverTick = true;

    meshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrapMesh"));
    RootComponent = meshComponent;
    // Establezco una mesh esferica por defecto
    static ConstructorHelpers::FObjectFinder<UStaticMesh> meshBody(TEXT("/Engine/BasicShapes/Sphere"));

    if (meshBody.Succeeded())
    {
        meshComponent->SetStaticMesh(meshBody.Object);
        // Le pongo el material M_Trap que he creado a la trampa
        static ConstructorHelpers::FObjectFinder<UMaterial> meshMat(TEXT("Material'/Game/Textures/M_Trap'"));
        meshComponent->SetMaterial(0, meshMat.Object);
    }
    netComponent = CreateDefaultSubobject<UNetComponent>(TEXT("NetComponent"));
}

void ATrap::BeginPlay()
{
    Super::BeginPlay();
    temp.X = 0.3f; // @WARN!
    temp.Y = 0.3f; // @WARN!
    temp.Z = 0.3f; // @WARN!

    SetActorScale3D(temp);
    SetActorEnableCollision(false);
}

void ATrap::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    Net::CManager* manager = Net::CManager::getSingletonPtr();

    if (manager && manager->getID() == Net::ID::SERVER)
    {
        TArray<AActor*> carsArray;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACar::StaticClass(), carsArray);

        for (auto carIterator : carsArray)
        {
            ACar* car = Cast<ACar>(carIterator);
            if (car)
            {
                float dist = GetDistanceTo(car);
                unsigned char otherCar = car->GetNetComponent()->GetID();
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::SanitizeFloat(dist));
                if (dist < 22.f && otherCar != trapID)
                {
                    unsigned int carID = car->GetNetComponent()->GetID();
                    netComponent->DestroyCar(otherCar);
                    // car->GetCarMovementComponent()->
                    Destroy();
                }
            }
        }
    }
}

UNetComponent* ATrap::GetNetComponent()
{
    return netComponent;
}

UStaticMeshComponent* ATrap::GetMesh()
{
    return meshComponent;
}

unsigned char ATrap::GetTrapID()
{
    return trapID;
}

void ATrap::SetNetComponent(UNetComponent* _netComponent)
{
    netComponent = _netComponent;
}

void ATrap::SetMesh(UStaticMeshComponent* _meshComponent)
{
    meshComponent = _meshComponent;
}

void ATrap::SetTrapID(unsigned char _trapID)
{
    trapID = _trapID;
}


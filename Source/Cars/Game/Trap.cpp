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
    // Tamaño de la trampa
    FVector trapSize;
    trapSize.X = 1.f;
    trapSize.Y = 1.f;
    trapSize.Z = 1.f;

    SetActorScale3D(trapSize);
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
                // Si la distancia entre la trampa y el otro coche es menor que 60..
                if (dist < 60.f && otherCar != trapID)
                {
                    unsigned int carID = car->GetNetComponent()->GetID();
                    netComponent->DestroyTrap(otherCar);
                    car->GetCarMovementComponent()->HitTrap();
                    Destroy();
                }
            }
        }
    }
}

unsigned char ATrap::GetTrapID()
{
    return trapID;
}

void ATrap::SetTrapID(unsigned char _id)
{
    trapID = _id;
}

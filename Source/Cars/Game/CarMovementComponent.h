// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CarMovementComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CARS_API UCarMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCarMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	FVector CalculateAcceleration() const;
	FVector CalculateAverageVelocity(const FVector& _vAccel, float _fDeltaTime);
	void ClampVelocity(FVector& _vVelocity_) const;
	void MoveActor(const FVector& _vAverageVelocity, float _fDeltaTime);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SetInput(const FVector2D& _vInput) { m_vMovementInput = _vInput; }
	float GetVelocityMagnitude() const;

protected:
	UPROPERTY(EditAnywhere)
		float m_fAccel = 120.f;
	UPROPERTY(EditAnywhere)
		float m_fDrag = 50.f;
	UPROPERTY(EditAnywhere)
		float m_fBrakeAccel = 300.f;
	UPROPERTY(EditAnywhere)
		float m_fRotationFactor = 2.f;
	UPROPERTY(EditAnywhere)
		float m_fMaxVelocity = 250.f;

	FVector2D m_vMovementInput = FVector2D::ZeroVector;
	FVector m_vVelocity = FVector::ZeroVector;
		
};

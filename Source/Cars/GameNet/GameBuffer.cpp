// Fill out your copyright notice in the Description page of Project Settings.


#include "GameNet/GameBuffer.h"

CGameBuffer::CGameBuffer(size_t initsize, size_t delta)
	: Net::CBuffer(initsize, delta)
{
}

CGameBuffer::~CGameBuffer()
{
}

void CGameBuffer::write(const FVector& _data)
{
	write(_data.X);
	write(_data.Y);
	write(_data.Z);
}
void CGameBuffer::read(FVector& data_)
{
	read(data_.X);
	read(data_.Y);
	read(data_.Z);
}
void CGameBuffer::write(const FVector2D& _data)
{
	write(_data.X);
	write(_data.Y);
}
void CGameBuffer::read(FVector2D& data_)
{
	read(data_.X);
	read(data_.Y);
}

void CGameBuffer::write(const FTransform& _data)
{
	write(_data.GetLocation());
	write(_data.GetRotation().GetAxisX());
	write(_data.GetScale3D());
}

void CGameBuffer::read(FTransform& data_)
{
	FVector vPos;
	read(vPos);
	FVector vDir;
	read(vDir);
	FVector vScale;
	read(vScale);
	FMatrix tMatrix = FRotationMatrix::MakeFromX(vDir);
	data_.SetFromMatrix(tMatrix);
	data_.SetTranslation(vPos);
	data_.SetScale3D(vScale);
}

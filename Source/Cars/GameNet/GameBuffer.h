// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Net/buffer.h"
#include "Math/Vector.h"
#include "Math/Transform.h"

/**
 * 
 */
class CGameBuffer : public Net::CBuffer
{
public:
	CGameBuffer(size_t initsize = 500, size_t delta = 100);
	~CGameBuffer();

	using Net::CBuffer::write;
	using Net::CBuffer::read;
	void write(const FVector& _data);
	void read(FVector& _data);
	void write(const FVector2D& _data);
	void read(FVector2D& _data);
	void write(const FTransform& _data);
	void read(FTransform& data_);
};

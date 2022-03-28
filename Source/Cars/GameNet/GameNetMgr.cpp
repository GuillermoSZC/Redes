// Fill out your copyright notice in the Description page of Project Settings.


#include "GameNet/GameNetMgr.h"
#include "GameNet/GameBuffer.h"
#include "Net/packet.h"
#include "Kismet/GameplayStatics.h"
#include "CarsGameInstance.h"
#include "Game/Car.h"
#include "GameNet/NetComponent.h"

CGameNetMgr::CGameNetMgr()
{
	Net::CManager::Init();
	m_pManager = Net::CManager::getSingletonPtr();
	m_pManager->addObserver(this);
}

CGameNetMgr::CGameNetMgr(UCarsGameInstance* _pOwner)
	: m_pCarsGameInstance(_pOwner)
{
	Net::CManager::Init();
	m_pManager = Net::CManager::getSingletonPtr();
	m_pManager->addObserver(this);
}

CGameNetMgr::~CGameNetMgr()
{
	m_pManager->removeObserver(this);
	Net::CManager::Release();
	m_pManager = nullptr;
}

void CGameNetMgr::dataPacketReceived(Net::CPacket* packet)
{
	CGameBuffer oData;
	oData.write(packet->getData(), packet->getDataLength());
	oData.reset();
	Net::NetMessageType iID;
	oData.read(iID);
	switch (iID)
	{
		case Net::LOAD_MAP:
	{
		char sLevel[32];
		oData.read(sLevel);
		UGameplayStatics::OpenLevel
		(m_pCarsGameInstance->GetWorld(), sLevel);
		Net::NetMessageType iResponseID = Net::NetMessageType::MAP_LOADED;
		CGameBuffer oData2;
		oData2.write(iResponseID);
		m_pManager->send(&oData2, true);
	}
	break;
	case Net::MAP_LOADED:
	{
		++m_uMapLoadedNotifications;
		if (m_uMapLoadedNotifications >=
			m_pManager->getConnections().size())
		{
			for (auto& rClient : m_pManager->getConnections())
			{
				CGameBuffer oData2;
				Net::NetMessageType iType = 
					Net::NetMessageType::LOAD_PLAYER;
				oData2.write(iType);
				oData2.write(rClient.first);
				FVector vPos(220.f, -310.f + rClient.first * 40.f, 0.f);
				oData2.write(vPos);
				m_pManager->send(&oData2, true);
				CreateCar(rClient.first, vPos);
			}
		}
	}
	break;
	case Net::LOAD_PLAYER:
	{
		unsigned int uClient;
		oData.read(uClient);
		FVector vPos;
		oData.read(vPos);
		CreateCar(uClient, vPos);
	}
	break;
	case Net::ENTITY_MSG:
	{
		Net::NetID uID;
		oData.read(uID);
		ACar* pCar = m_tPlayers[uID];
		pCar->GetNetComponent()->DeserializeData(&oData);
	}
	break;
	default:
		break;
	}
}

void CGameNetMgr::connectionPacketReceived(Net::CPacket* packet)
{
	if (m_pManager->getID() == Net::ID::SERVER)
	{
		GEngine->AddOnScreenDebugMessage
		(-1, 5.f, FColor::Green, "Client connected!");
	}
}

void CGameNetMgr::disconnectionPacketReceived(Net::CPacket* packet)
{

}

void CGameNetMgr::CreateCar(unsigned int _uClient, FVector _vPos)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Name = FName("Car", _uClient);
	SpawnInfo.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ACar* pCar = m_pCarsGameInstance->GetWorld()->SpawnActor<ACar>
		(_vPos, FRotator::ZeroRotator, SpawnInfo);
	if (pCar)
	{
		pCar->GetNetComponent()->SetID(_uClient);
		m_tPlayers[_uClient] = pCar;
		if (_uClient == m_pManager->getID())
		{
			APlayerController* pPC =
				GEngine->GetFirstLocalPlayerController
				(m_pCarsGameInstance->GetWorld());
			if (pPC)
			{
				pPC->Possess(pCar);
			}
		}
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "CarsGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameNet/GameBuffer.h"


UCarsGameInstance::UCarsGameInstance
(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , m_oGameNetMgr(this)
{
    m_pManager = Net::CManager::getSingletonPtr();
}

void UCarsGameInstance::OnStart()
{
  Super::OnStart();
  ChangeMenuWidget(StartingWidgetClass);
}

void UCarsGameInstance::ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
  if (CurrentWidget != nullptr)
  {
    CurrentWidget->RemoveFromViewport();
    CurrentWidget = nullptr;
  }
  if (NewWidgetClass != nullptr)
  {
    CurrentWidget = CreateWidget<UUserWidget>(this, NewWidgetClass);
    if (CurrentWidget != nullptr)
    {
      CurrentWidget->AddToViewport();
    }
  }
}

void UCarsGameInstance::OnServerButtonClick(FString sPort)
{
    m_pManager->activateAsServer(FCString::Atoi(*sPort));
}

void UCarsGameInstance::OnClientButtonClick(FString sIP, FString sPort)
{
    m_pManager->activateAsClient();
    m_pManager->connectTo(TCHAR_TO_ANSI(*sIP), FCString::Atoi(*sPort));
}

void UCarsGameInstance::OnServerStartButtonClick()
{
    const char* sLevel = "circuit1";
    UGameplayStatics::OpenLevel(GetWorld(), sLevel);
    Net::NetMessageType iID = Net::LOAD_MAP;
    CGameBuffer oData;
    oData.write(iID);
    oData.write(sLevel);
    m_pManager->send(&oData, true);
}

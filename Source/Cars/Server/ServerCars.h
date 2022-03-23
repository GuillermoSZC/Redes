#pragma once
#include "Net/Manager.h"

class CServerCars : public Net::CManager::IObserver
{
public:
  CServerCars();
  virtual ~CServerCars();

  void Step();

  // Net::CManager::IObserver
  virtual void dataPacketReceived(Net::CPacket* packet);
  virtual void connectionPacketReceived(Net::CPacket* packet);
  virtual void disconnectionPacketReceived(Net::CPacket* packet);
private:
  Net::CManager* m_pManager = nullptr;
};


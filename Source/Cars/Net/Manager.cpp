/**
@file Manager.cpp

Contiene la implementación de la clase CManager, Singleton que se encarga de
la gestión de la red del juego.

@see Net::CManager

@author David Llansó
@date Diciembre, 2010
*/

#include "Manager.h"
#include "buffer.h"
#include "factoryenet.h"
#include "factory.h"
#include <cassert>

namespace Net {

	CManager* CManager::_instance = 0;

	//--------------------------------------------------------

	CManager::CManager(): _factory(0), _server(0), _client(0),
		_id(Net::ID::UNASSIGNED),_nextId(Net::ID::FIRSTCLIENT)
	{
		_instance = this;

	} // CManager

	//--------------------------------------------------------

	CManager::~CManager()
	{
		_instance = 0;

	} // ~CManager
	
	//--------------------------------------------------------

	bool CManager::Init()
	{
		assert(!_instance && "Segunda inicialización de Net::CManager no permitida!");

		new CManager();

		if (!_instance->open())
		{
			Release();
			return false;
		}

		return true;

	} // Init

	//--------------------------------------------------------

	void CManager::Release()
	{
		assert(_instance && "Net::CManager no está inicializado!");

		if(_instance)
		{
			_instance->close();
			delete _instance;
			_instance = 0;
		}

	} // Release

	//--------------------------------------------------------

	bool CManager::open()
	{
		 // Inicializamos la factoria de objetos de red
		_factory = new Net::CFactoryEnet();

		return true;

	} // open

	//--------------------------------------------------------

	void CManager::close() 
	{
		deactivateNetwork();
		if(_factory)
			delete _factory;

	} // close

	//---------------------------------------------------------

	void CManager::send(CBuffer* data, bool reliable)
	{
		if (!_connections.empty())
		{
			if (_server)
				_server->sendAll(data->getData(), data->getSize(), 0, reliable);
			if (_client)
				_client->sendData(getConnection(Net::ID::SERVER), data->getData(), data->getSize(), 0, reliable);
		}
	} // send

	//---------------------------------------------------------

	void CManager::tick() 
	{
		_packets.clear();
		Net::CManager::getSingletonPtr()->getPackets();

		for(std::vector<Net::CPacket*>::iterator iterp = _packets.begin();iterp != _packets.end();++iterp)
		{
			Net::CPacket* packet = *iterp;
			// El mensaje debe ser de tipo CONEXION
			switch (packet->getType())
			{
				case Net::CONNECTION:
					connect(packet->getConnection());
					for(std::vector<IObserver*>::iterator iter = _observers.begin();iter != _observers.end();++iter)
						(*iter)->connectionPacketReceived(packet);
					break;
				case Net::DATA:
					if(!internalData(packet))
						for(std::vector<IObserver*>::iterator iter = _observers.begin();iter != _observers.end();++iter)
							(*iter)->dataPacketReceived(packet);
					break;
				case Net::DISCONNECTION:
					for(std::vector<IObserver*>::iterator iter = _observers.begin();iter != _observers.end();++iter)
						(*iter)->disconnectionPacketReceived(packet);
					disconnect(packet->getConnection());
					break;
			}
			delete packet;
		}

	} // tick

	//---------------------------------------------------------


	void CManager::activateAsServer(int port, int clients, unsigned int maxinbw, unsigned int maxoutbw)
	{
		//Creamos el servidor de red
		_server = _factory->buildServer();

		_server->init(port,clients, maxinbw, maxoutbw);

		_id = Net::ID::SERVER;

	} // activateAsServer

	//---------------------------------------------------------

	void CManager::activateAsClient(unsigned int maxConnections, unsigned int maxinbw, unsigned int maxoutbw)
	{
		//Creamos el servidor de red
		_client = _factory->buildClient();

		_client->init(maxConnections, maxinbw, maxoutbw);

	} // activateAsClient

	//---------------------------------------------------------

	void CManager::connectTo(char* address, int port, int channels, unsigned int timeout)
	{
		assert(_client && "Cliente Red es null");
		assert(_connections.empty() && "Ya hay una conexion");
		CConnection* connection = _client->connect(address, port, channels,timeout);
		// Almacenamos esa conexión y le otorgamos un ID de red
    if (connection)
    {
      connection->setId(Net::ID::SERVER);
      addConnection(Net::ID::SERVER, connection);
    }
	} // connectTo

	//---------------------------------------------------------

	bool CManager::internalData(Net::CPacket* packet)
	{
		Net::CBuffer data;
		data.write(packet->getData(),packet->getDataLength());
		data.reset();
		// Ignoramos el tipo de mensaje de red. Ya lo hemos procesado
		Net::NetMessageType msg;
		data.read(&msg,sizeof(msg));
		switch (msg)
		{
		case Net::ASSIGNED_ID: 
			// Escribimos el id de red que corresponde a este cliente
			data.read(&_id,sizeof(_id));
			return true;
		default:
			return false;
		}

	} // internalData

	//---------------------------------------------------------

	void CManager::connect(CConnection* connection)
	{
		// Una nueva conexión de un cliente al sevidor

		// Almacenamos esa conexión y le otorgamos un ID de red
		connection->setId(_nextId);
		addConnection(_nextId,connection);

		// Avisamos al cliente de cual es su nuevo ID
		CBuffer buf;
		// Escribimos el tipo de mensaje de red a enviar
		NetMessageType type = Net::ASSIGNED_ID;
		buf.write(&type,sizeof(type));
		// Escribimos el id del cliente
		buf.write(&_nextId,sizeof(_nextId));
		_server->sendData(connection, buf.getData(),buf.getSize(),0,1);

		// Preparamos para la siguiente conexión
		_nextId = ID::NextID(_nextId);

	} // connect

	//---------------------------------------------------------

	void CManager::disconnect(CConnection* connection)
	{
		if(_server)
		{
			_server->disconnect(connection);
			removeConnection(connection->getId());
		}
		else if(_client)
		{
			_client->disconnect(getConnection(Net::ID::SERVER));
			removeConnection(Net::ID::SERVER);
		}

	} // disconnect
		
	//---------------------------------------------------------

	bool CManager::addConnection(NetID id, CConnection* connection) 
	{
		if(_connections.count(id))
			return false;
		TConnectionPair elem(id,connection);
		_connections.insert(elem);
		return true;

	} // addConection
		
	//---------------------------------------------------------

	bool CManager::removeConnection(NetID id) 
	{
		if(_connections.count(id))
		{
			CConnection* connection = getConnection(id);
			_connections.erase(id);
			delete connection;
			return true;
		}
		return false;

	} // removeConection

	//---------------------------------------------------------

	void CManager::deactivateNetwork()
	{
		if(_server)
		{
			_server->release();
			delete _server;
			_server = 0;
		}
		if(_client)
		{
			_client->release();
			delete _client;
			_client = 0;
		}
		if(!_connections.empty())
		{
			for(TConnectionTable::const_iterator it = _connections.begin(); it != _connections.end(); it++)
				delete (*it).second;
			_connections.clear();
		}
	} // deactivateNetwork

	//---------------------------------------------------------

	void CManager::getPackets()
	{
		if(_server)
			_server->service(_packets);
		if(_client)
			_client->service(_packets);

	} // getPackets

	//---------------------------------------------------------

	void CManager::addObserver(IObserver* listener)
	{
		_observers.push_back(listener);

	} // addObserver

	//---------------------------------------------------------

	void CManager::removeObserver(IObserver* listener)
	{
		for(std::vector<IObserver*>::iterator iter = _observers.begin();iter != _observers.end();++iter)
			if((*iter)==listener)
			{
				_observers.erase(iter);
				break;
			}
	} // removeObserver

	//---------------------------------------------------------
	
	namespace ID
	{
		/**
			Devuelve el siguiente ID dado el anterior.

			@param id Último ID asignado.
			@return Nuevo ID.
		*/
		NetID NextID(const NetID &id)
		{
			NetID ret = id + 1;
			assert( id != UNASSIGNED && id != SERVER && 
					ret != UNASSIGNED && ret != SERVER);
			return ret;
		
		} // NextClientID

	} // namespace ID

} // namespace Net

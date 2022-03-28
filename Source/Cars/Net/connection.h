//---------------------------------------------------------------------------
// connection.h
//---------------------------------------------------------------------------

/**
 * @file connection.h
 *
 * Representa una conexión con un peer.
 *
 *
 * @author Juan A. Recio-García, David Llansó
 * @date Octubre, 2006
 */

#ifndef __CONNECTION_H
#define __CONNECTION_H

#define DEBUG_CONNECTION 1

#include <string>

namespace Net {

	/** 
	ID de identificación en la red.
	*/
	typedef unsigned int NetID;

class CConnection {

public:

	virtual ~CConnection() {}
	virtual int getAddress()=0;
	virtual short getPort()=0;
	virtual void setId(NetID id)=0;
	virtual NetID getId()=0;
};


} // namespace Net

#endif // __CONNECTION_H
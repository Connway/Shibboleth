//
// CScriptSocket
//
// This class represents a socket. It can be used to set up listeners for 
// connecting incoming clients, and also to connect to a server. The socket 
// class will create threads in the background to handle the communication 
// and update a send and a receive buffer so the script will not have to deal 
// with that.
//

#ifndef SCRIPTSOCKET_H
#define SCRIPTSOCKET_H

#include <string>
#include <list>
#ifdef _WIN32
#include <winsock2.h>
#endif

#ifndef ANGELSCRIPT_H 
// Avoid having to inform include path if header is already include before
#include <angelscript.h>
#endif

BEGIN_AS_NAMESPACE

class CScriptSocket
{
public:
	CScriptSocket();

	void AddRef() const;
	void Release() const;

	int            Listen(asWORD port);
	int            Close();
	CScriptSocket* Accept();
	int            Connect(asUINT ipv4Address, asWORD port);
	int            Send(const std::string& data);
	std::string    Receive();

protected:
	~CScriptSocket();
	mutable int m_refCount;

	int m_socket;
	bool m_isListening;
};

int RegisterScriptSocket(asIScriptEngine* engine);

END_AS_NAMESPACE

#endif
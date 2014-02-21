/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
************************************************************************************/

#include "Gaff_Connection.h"
#include "Gaff_IncludeAssert.h"
#include "Gaff_Host.h"
#include <enet/enet.h>

NS_GAFF

Connection::Connection(const Connection& rhs):
	_peer(rhs._peer), _cleanup(true)
{
	((Connection&)rhs)._peer = nullptr; // take over control of their _peer pointer
}

Connection::Connection(_ENetPeer* peer):
	_peer(peer), _cleanup(false)
{
}

Connection::Connection(void):
	_peer(nullptr), _cleanup(true)
{
}

Connection::~Connection(void)
{
	if (_cleanup) {
		destroy();
	}
}

void Connection::destroy(void)
{
	assert(_cleanup);

	if (_peer) {
		enet_peer_disconnect_now(_peer, 0);
		enet_peer_reset(_peer);
		_peer = nullptr;
	}
}

unsigned int Connection::getID(void) const
{
	// Lazy way for giving a unique ID.
	// This is mainly used for the checkForEvent()/waitForEvent() callback
	// to identify which Connection the event came from.
	return (Host::PeerIDType)_peer;
}

unsigned int Connection::getHost(void) const
{
	assert(_peer);
	return _peer->address.host;
}

unsigned short Connection::getPort(void) const
{
	assert(_peer);
	return _peer->address.port;
}

void* Connection::getUserData(void) const
{
	assert(_peer);
	return _peer->data;
}

void Connection::setUserData(void* data)
{
	assert(_peer);
	_peer->data = data;
}

void Connection::setThrottle(unsigned int interval, unsigned int acceleration, unsigned int deceleration)
{
	assert(_peer);
	enet_peer_throttle_configure(_peer, interval, acceleration, deceleration);
}

void Connection::setTimeout(unsigned int limit, unsigned int min, unsigned int max)
{
	assert(_peer);
	enet_peer_timeout(_peer, limit, min, max);
}

void Connection::setPingInterval(unsigned int interval)
{
	assert(_peer);
	enet_peer_ping_interval(_peer, interval);
}

void Connection::disconnect(unsigned int data)
{
	assert(_peer);
	enet_peer_disconnect(_peer, data);
}

void Connection::disconnectNow(unsigned int data)
{
	assert(_peer);
	enet_peer_disconnect_now(_peer, data);
}

void Connection::disconnectLater(unsigned int data)
{
	assert(_peer);
	enet_peer_disconnect_later(_peer, data);
}

void Connection::ping(void)
{
	assert(_peer);
	enet_peer_ping(_peer);
}

bool Connection::send(unsigned char channel, void* data, size_t data_size, unsigned int packet_flags)
{
	assert(_peer);
	ENetPacket* packet = enet_packet_create(data, data_size, packet_flags);
	return !enet_peer_send(_peer, channel, packet);
}

bool Connection::valid(void) const
{
	return _peer != nullptr;
}

bool Connection::operator==(const Connection& rhs) const
{
	return _peer == rhs._peer;
}

bool Connection::operator!=(const Connection& rhs) const
{
	return _peer != rhs._peer;
}

const Connection& Connection::operator=(const Connection& rhs)
{
	_cleanup = rhs._cleanup;
	_peer = rhs._peer;

	if (_cleanup) {
		((Connection&)rhs)._peer = nullptr; // take over control of their _peer pointer
	}

	return *this;
}

Connection::operator bool(void) const
{
	return valid();
}

void Connection::connect(Host& host, const char* address, unsigned short port, size_t channels, unsigned int data)
{
	ENetAddress addr = { ENET_HOST_ANY, port };

	if (enet_address_set_host(&addr, address)) {
		return;
	}

	_peer = enet_host_connect(host._host, &addr, channels, data);
}

NS_END

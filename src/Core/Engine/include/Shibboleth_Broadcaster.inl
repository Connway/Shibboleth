/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

template <class Message>
Broadcaster::ID Broadcaster::listen(const eastl::function<void (const Message&)>& callback)
{
	const Listener func = Gaff::Func<void (const void*)>([callback](const void* message) -> void
	{
		const Message* const msg = reinterpret_cast<const Message*>(message);
		callback(*msg);
	});

	return listenInternal(Hash::ClassHashable<Message>::GetHash(), std::move(func));
}

template <class Message>
Broadcaster::ID Broadcaster::listen(eastl::function<void (const Message&)>&& callback)
{
	const Listener func = Gaff::Func<void(const void*)>([callback](const void* message) -> void
	{
		const Message* const msg = reinterpret_cast<const Message*>(message);
		callback(*msg);
	});

	return listenInternal(Hash::ClassHashable<Message>::GetHash(), std::move(func));
}

template <class Message>
bool Broadcaster::remove(const eastl::function<void(const Message&)>& callback)
{
	const ID id = { Hash::ClassHashable<Message>::GetHash(), Gaff::FNV1aHash64T(callback) };
	return remove(id);
}

template <class Message>
void Broadcaster::broadcastAsync(const Message& message)
{
	void* const data = SHIB_ALLOC(sizeof(BroadcastData) + sizeof(Message), s_allocator);

	BroadcastData* const broadcast_data = static_cast<BroadcastData*>(data);
	broadcast_data->msg_hash = Hash::ClassHashable<Message>::GetHash();
	broadcast_data->msg_size = sizeof(Message);
	broadcast_data->broadcaster = this;

	//if constexpr (std::is_standard_layout<Message>::value && std::is_trivial<Message>::value) {
		broadcast_data->deconstructor = [](void* data) -> void
		{
			Gaff::Deconstruct(static_cast<Message*>(data));
		};
	//}

	Message* const msg = reinterpret_cast<Message*>(static_cast<uint8_t*>(data) + sizeof(BroadcastData));
	*msg = message;

	broadcastAsyncInternal(*broadcast_data);
}

template <class Message>
void Broadcaster::broadcastSync(const Message& message)
{
	broadcastSync(Hash::ClassHashable<Message>::GetHash(), &message);
}

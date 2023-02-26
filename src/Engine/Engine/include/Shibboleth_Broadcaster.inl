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
	const EA::Thread::AutoMutex lock(_listener_lock);

	const Broadcaster::ID id = { Refl::Reflection<Message>::GetHash(), Gaff::FNV1aHash64T(callback) };
	auto& listener_data = _listeners[id.msg_hash];

	const ListenerData::Listener func = Gaff::Func<void (const void*)>([callback](const void* message) -> void
	{
		const Message* const msg = reinterpret_cast<const Message*>(message);
		callback(*msg);
	});

	// Should we make this a vector? Not a common scenario to register the same function for a message multiple times,
	// but still a possible scenario.
	listener_data.listeners[id.cb_hash] = func;

	return id;
}

template <class Message>
Broadcaster::ID Broadcaster::listen(eastl::function<void(const Message&)>&& callback)
{
	const EA::Thread::AutoMutex lock(_listener_lock);

	const Broadcaster::ID id = { Refl::Reflection<Message>::GetHash(), Gaff::FNV1aHash64T(callback) };
	auto& listener_data = _listeners[id.msg_hash];

	const ListenerData::Listener func = Gaff::Func<void(const void*)>([callback](const void* message) -> void
	{
		const Message* const msg = reinterpret_cast<const Message*>(message);
	callback(*msg);
	});

	// Should we make this a vector? Not a common scenario to register the same function for a message multiple times,
	// but still a possible scenario.
	listener_data.listeners[id.cb_hash] = std::move(func);

	return id;
}

template <class Message>
bool Broadcaster::remove(const eastl::function<void(const Message&)>& callback)
{
	const ID id = { Refl::Reflection<Message>::GetHash(), Gaff::FNV1aHash64T(callback) };
	return remove(id);
}

template <class Message>
void Broadcaster::broadcastAsync(const Message& message)
{
	GAFF_ASSERT_MSG(false, "Broadcaster::broadcastAsync() not implemented yet.");
	GAFF_REF(message);

	// $TODO: Find a way to make this work. Likely will involve a cache of some sort.
	//const auto func = [this, message](void*) -> void
	//{
	//	const EA::Thread::AutoMutex lock(_listener_lock);

	//	const auto it = _listeners.find(Refl::Reflection<Message>::GetHash());

	//	if (it == _listeners.end()) {
	//		return;
	//	}

	//	for (const auto cb : it->second) {
	//		if (cb) {
	//			cb(message);
	//		}
	//	}
	//};

	//Gaff::JobData data = { func, nullptr };
	//_job_pool->addJobs(&data, 1);
}

template <class Message>
void Broadcaster::broadcastSync(const Message& message)
{
	const EA::Thread::AutoMutex lock(_listener_lock);
	const auto it = _listeners.find(Refl::Reflection<Message>::GetHash());

	if (it == _listeners.end()) {
		return;
	}

	for (const auto func : it->second) {
		if (func) {
			func(&message);
		}
	}
}

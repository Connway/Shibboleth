/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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
BroadcastID Broadcaster::listen(const eastl::function<void (const Message&)>& callback)
{
	std::lock_guard lock(_listener_lock);

	auto& listener_data = _listeners[Reflection<Message>::GetHash()];
	BroadcastID id(Reflection<Message>::GetHash(), 0);

	const ListenerData::Listener func = Gaff::Func<void (const void*)>([callback](const void* message) -> void {
		const Message* const msg = reinterpret_cast<const Message*>(message);
		callback(*msg);
	});

	if (listener_data.unused_ids.empty()) {
		id.second = listener_data.listeners.size();
		listener_data.listeners.emplace_back(func);
	} else {
		id.second = listener_data.unused_ids.back();
		listener_data.unused_ids.pop_back();
		listener_data.listeners[id.second] = func;
	}

	return id;
}

template <class Message>
void Broadcaster::broadcastSync(const Message& message)
{
	const auto it = _listeners.find(Reflection<Message>::GetHash());

	if (it == _listeners.end()) {
		return;
	}

	for (const auto func : it->second.listeners) {
		if (func) {
			func(&message);
		}
	}
}

template <class Message>
void Broadcaster::broadcast(const Message& message)
{
	const auto func = [this, message](void*) -> void {
		std::lock_guard lock(_listener_lock);

		const auto it = _listeners.find(Reflection<Message>::GetHash());

		if (it == _listeners.end()) {
			return;
		}

		for (const auto cb : it->second.listeners) {
			if (cb) {
				cb(message);
			}
		}
	};

	Gaff::JobData data{ func, nullptr };
	_job_pool->addJobs(&data, 1, &_counter);
}

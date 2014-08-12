#include "Gaff_RefCounted.h"
#include "Gaff_ScopedLock.h"
#include "Gaff_SpinLock.h"
#include "Gaff_Function.h"
#include "Gaff_RefPtr.h"
#include "Gaff_Map.h"

NS_GAFF

typedef RefPtr<IRefCounted> WatchReceipt;

template <class T, class Allocator = DefaultAllocator>
class Watcher
{
public:
	typedef Gaff::FunctionBinder<void, const T&> Callback;

	Watcher(T&& data, const Allocator& allocator = Allocator());
	Watcher(const T& data, const Allocator& allocator = Allocator());
	Watcher(const Allocator& allocator = Allocator());
	~Watcher(void);

	bool init(void);

	const Watcher& operator=(T&& rhs);
	const Watcher& operator=(const T& rhs);

	WatchReceipt addCallback(const Callback& callback);

	const T* getPtr(void) const;
	const T& get(void) const;

private:
	class Remover : public RefCounted<Allocator>
	{
	public:
		Remover(Watcher* watcher, const Allocator& allocator);
		~Remover(void);

		void removeCallback(unsigned int id);

	private:
		Watcher* _watcher;
		SpinLock _lock;

		void watcherDeleted(void);

		friend class Watcher;
	};

	class Receipt : public IRefCounted
	{
	public:
		Receipt(unsigned int id, const RefPtr<Remover>& remover, const Allocator& allocator);
		~Receipt(void);

		void addRef(void) const;
		void release(void) const;

		unsigned int getRefCount(void) const;

	private:
		mutable Allocator _allocator;
		mutable RefPtr<Remover> _remover;
		mutable volatile unsigned int _ref_count;
		unsigned int _id;
	};

	Map<unsigned int, Callback, Allocator> _callbacks;
	T _data;

	Allocator _allocator;

	RefPtr<Remover> _remover;
	SpinLock _lock;

	unsigned int _next_id;

	void remove(unsigned int id);

	GAFF_NO_COPY(Watcher);
	GAFF_NO_MOVE(Watcher);

	friend class Remover;
};

#include "Gaff_Watcher.inl"

NS_END

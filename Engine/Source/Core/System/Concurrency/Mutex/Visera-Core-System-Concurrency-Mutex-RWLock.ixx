module;
#include <Visera.h>
export module Visera.Core.System.Concurrency.Mutex:RWLock;

export namespace VE
{
	class FRWLock
	{
	public:
		Bool TryToRead()    const { return Handle.try_lock_shared(); }
		void StartReading() const { Handle.lock_shared(); }
		void StopReading()  const { Handle.unlock_shared(); }

		Bool TryToWrite()   { return Handle.try_lock(); }
		void StartWriting() { Handle.lock(); }
		void StopWriting()  { Handle.unlock(); }

	private:
		mutable std::shared_mutex Handle;
	};
} 
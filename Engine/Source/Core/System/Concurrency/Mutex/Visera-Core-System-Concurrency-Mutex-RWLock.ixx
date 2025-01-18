module;
#include <Visera.h>
export module Visera.Core.System.Concurrency.Mutex:RWLock;

export namespace VE
{
	class FRWLock
	{
	public:
		Bool TryToRead()	{ return Handle.try_lock_shared(); }
		void StartReading() { Handle.lock_shared(); }
		void StopReading()  { Handle.unlock_shared(); }

		Bool TryToWrite()	{ return Handle.try_lock(); }
		void StartWriting() { Handle.lock(); }
		void StopWriting()  { Handle.unlock(); }

		Bool IsLocked()     { return !TryToWrite(); }

	private:
		std::shared_mutex Handle;
	};
} 
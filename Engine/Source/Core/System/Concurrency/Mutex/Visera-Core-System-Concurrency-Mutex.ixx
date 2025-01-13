module;
#include <Visera.h>
export module Visera.Core.System.Concurrency.Mutex;
export import :RWLock;

export namespace VE
{
	class FMutex
	{
	public:
		Bool TryToLock() { return Handle.try_lock(); }
		void Lock()		 { Handle.lock(); }
		void Unlock()	 { Handle.unlock(); }

	private:
		std::mutex Handle;
	};
} 
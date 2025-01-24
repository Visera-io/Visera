module;
#include <Visera.h>
export module Visera.Runtime.World.Object;

import Visera.Core.Type;
import Visera.Core.Math.Hash;
import Visera.Core.System.Concurrency;

export namespace VE { namespace Runtime
{

	class ViseraRuntime;

	class VObject
	{
		friend class ViseraRuntime;
	public:
		virtual void Update()   { /* Please Use RWLock in the Multi-Thread Program. */ };
		virtual void Create()	= 0;
		virtual void Destroy()	= 0;

		auto GetObjectID()				const -> UInt64		{ return ID; }
		auto GetObjectName()			const -> StringView { return Name.GetName(); }
		auto GetObjectNameWithNumber()	const -> String		{ return Name.GetNameWithNumber(); }

	public:
		VObject(FName _Name) noexcept;
		VObject() noexcept = delete;
		virtual ~VObject() noexcept {};

	protected:
		FRWLock RWLock;

	private:
		static inline UInt64 ObjectCount = 0;
		UInt64	ID = 0;
		FName	Name;
	};

	VObject::
	VObject(FName _Name) noexcept
		:Name{ _Name },
		 ID { ObjectCount++ }
	{
	
	};

} } // namespace VE::Runtime
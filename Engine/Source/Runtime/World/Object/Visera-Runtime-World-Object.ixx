module;
#include <Visera.h>

export module Visera.Runtime.World.Object;
import Visera.Core.Hash;

export namespace VE { namespace Runtime
{

class ViseraRuntime;

class VObject
{
	friend class ViseraRuntime;
public:
	auto GetID() const -> UInt64 { return ID; }

	virtual void Create()	= 0;
	virtual void Destroy()	= 0;

public:
	VObject(const String& _Name) noexcept;
	VObject() noexcept = delete;
	virtual ~VObject() noexcept {};

private:
	UInt64 ID;
};

VObject::
VObject(const String& _Name) noexcept
	:ID{ Hash::CityHash64(_Name) }
{
	
};
} } // namespace VE::Runtime
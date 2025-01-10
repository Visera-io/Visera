module;
#include <Visera.h>

export module Visera.Runtime.Object;
import Visera.Core.Hash;

export namespace VE { namespace Runtime
{

class ViseraRuntime;

class VObject
{
	friend class ViseraRuntime;
public:
	auto GetID() const -> UInt64 { return ID; }

public:
	VObject(const String& _Name) noexcept;
	VObject() noexcept = delete;
	virtual ~VObject() noexcept {};

private:
	UInt64 ID;
};

VObject::
VObject(const String& _Name) noexcept
	:ID{ Hash::CityHash64("Name") }
{
	
};
} } // namespace VE::Runtime
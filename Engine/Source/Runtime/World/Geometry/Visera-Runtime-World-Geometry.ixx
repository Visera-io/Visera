module;
#include <Visera.h>
export module Visera.Runtime.World.Geometry;
export import Visera.Runtime.World.Geometry.Mesh;

export namespace VE { namespace Runtime
{
	
	class Geometry
	{
		VE_MODULE_MANAGER_CLASS(Geometry);
	public:
		using FMesh = FMesh;
		VE_API CreateMesh(const FMesh::FCreateInfo& _CreateInfo) -> SharedPtr<FMesh> { return CreateSharedPtr<FMesh>(_CreateInfo); }
	};

} } // namespace Visera::Runtime
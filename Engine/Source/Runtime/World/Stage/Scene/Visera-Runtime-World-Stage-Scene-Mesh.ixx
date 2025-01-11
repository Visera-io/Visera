module;
#include <Visera.h>
export module Visera.Runtime.World.Stage.Scene:Mesh;

import Visera.Core.Math;

export namespace VE { namespace Runtime
{

	class FMesh
	{
	public:
		

	public:
		struct FVertex
		{
			Vector3F Position;
			Vector3F Normal;
			Vector2F UVCoords;
		};
		struct FTexture
		{
			UInt32 ID;
			UInt64 Type; //Hash
		};

	private:
		
	};

} } // namespace VE::Runtime
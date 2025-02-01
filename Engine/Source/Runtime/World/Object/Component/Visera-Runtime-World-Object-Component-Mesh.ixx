module;
#include <Visera.h>
export module Visera.Runtime.World.Object.Component:Mesh;

import :BasicComponent;

export namespace VE { namespace Runtime
{

	class OCMesh : public OCBasicComponent
	{
	public:
		auto GetVertices() const -> const Array<Float>& { return Vertices; }
		void SetVertices(Array<Float> _NewVertices) { Vertices = std::move(Vertices); }

		auto GetIndices() const  -> const Array<Float>& { return Indices; }
		void SetIndices(Array<Float> _NewIndices)	{ Indices = std::move(_NewIndices); }

	private:
		Array<Float> Vertices;
		Array<Float> Indices;

	protected:
		virtual void Update();
		virtual void Create()  override;
		virtual void Destroy() override;

	public:
		OCMesh() = delete;
		OCMesh(FName _Name, SharedPtr<VObject> _Owner)
			:OCBasicComponent(_Name, _Owner)
		{
			
		}
	};

	void OCMesh::Update()
	{

	}

	void OCMesh::Create()
	{

	}

	void OCMesh::Destroy()
	{

	}

} } // namespace VE::Runtime
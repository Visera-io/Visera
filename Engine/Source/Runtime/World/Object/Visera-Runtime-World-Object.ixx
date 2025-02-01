module;
#include <Visera.h>
export module Visera.Runtime.World.Object;
import Visera.Runtime.World.Object.Component;

import Visera.Core.Type;
import Visera.Core.System.Concurrency;

export namespace VE { namespace Runtime
{
	class World;
	class VObject;
	template<typename T>
	concept VObjectType = std::is_class_v<VObject>;

	class VObject : public std::enable_shared_from_this<VObject>
	{
		friend class World; // VObject can only be created via World::CreateObject(...)
	public:
		virtual void Update()   { /* Use RWLock in the Multi-Thread Program. */ };

		Bool HasTransform()				const { return TransformComponent != nullptr; }
		auto GetTransform()				const -> SharedPtr<OCTransform> { return TransformComponent; }
		Bool HasMesh()					const { return MeshComponent != nullptr; }
		auto GetMesh()					const -> SharedPtr<OCMesh> { return MeshComponent; }

		auto GetObjectName()			const -> StringView { return Name.GetName(); }
		auto GetObjectNameWithNumber()	const -> String		{ return Name.GetNameWithNumber(); }

	protected:
		virtual void Create()	= 0; // You may attach components here
		virtual void Destroy()	= 0;
		
		void AttachTransformComponent() { TransformComponent	= CreateSharedPtr<OCTransform>(FName("Component::Transform"), shared_from_this()); }
		void AttachMeshComponent()		{ MeshComponent			= CreateSharedPtr<OCMesh>(FName("Component::Mesh"), shared_from_this()); }
		void AttachCustomizedComponent(SharedPtr<OCBasicComponent> _Component) { CustomizedComponents.push_back(_Component); };

	protected:
		FRWLock								RWLock;
		SharedPtr<OCTransform>				TransformComponent;
		SharedPtr<OCMesh>					MeshComponent;
		Array<SharedPtr<OCBasicComponent>>	CustomizedComponents;

	private:
		FName	Name;
		Bool	bRecollectable = False;

	public:
		VObject() noexcept = default;
		~VObject() noexcept = default; // Do NOT do anything in deconstructor!
	};

} } // namespace VE::Runtime
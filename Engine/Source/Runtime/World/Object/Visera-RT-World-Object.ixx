module;
#include <Visera.h>
export module Visera.Runtime.World.Object;
import Visera.Runtime.World.Object.Component;

import Visera.Core.Type;
import Visera.Core.OS.Concurrency;

export namespace VE
{

	class VObject : public std::enable_shared_from_this<VObject>
	{
	public:
		virtual void Update()   { /* Use RWLock in the Multi-Thread Program. */ };

		Bool HasTransform()				const { return TransformComponent != nullptr; }
		auto GetTransform()				const -> SharedPtr<OCTransform> { return TransformComponent; }
		Bool HasMesh()					const { return MeshComponent != nullptr; }
		auto GetMesh()					const -> SharedPtr<OCMesh> { return MeshComponent; }

		auto GetObjectName()			const -> StringView { return Name.GetName(); }
		auto GetObjectNameWithNumber()	const -> String		{ return Name.GetNameWithNumber(); }

		Bool IsRecollectable()			const { return bRecollectable; }

		virtual void Create()	= 0; // You may attach components here
		virtual void Destroy()	= 0;

	protected:
		void AttachTransformComponent() { VE_WIP; /*TransformComponent	= CreateSharedPtr<OCTransform>(FName("component::transform"), shared_from_this());*/ }
		void AttachMeshComponent()		{ VE_WIP; /*MeshComponent			= CreateSharedPtr<OCMesh>(FName("component::mesh"), shared_from_this()); */ }
		void AttachCustomizedComponent(SharedPtr<IObjectComponent> _Component) { CustomizedComponents.push_back(_Component); };

	protected:
		FRWLock								RWLock;
		FName	Name;

		SharedPtr<OCTransform>				TransformComponent;
		SharedPtr<OCMesh>					MeshComponent;
		Array<SharedPtr<IObjectComponent>>	CustomizedComponents;

		Bool	bRecollectable = False;

	public:
		VObject()  noexcept = default;
		~VObject() noexcept = default; // Do NOT do anything in deconstructor!
	};

	template<typename T>
	concept VObjectType = std::is_class_v<VObject>;

} // namespace VE
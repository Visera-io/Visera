module;
#include <Visera.h>
export module Visera.Runtime.World.Object.Component:Interface;
import Visera.Core.Type;
import Visera.Core.Signal;

export namespace VE
{

	class IObjectComponent
	{
	public:
		auto GetComponentName()				-> StringView { return Name.GetName(); }
		auto GetComponentNameWithNumber()   -> String { return Name.GetNameWithNumber(); }

	protected:
		virtual void Update() {};
		virtual void Create() = 0;
		virtual void Destroy() = 0;

	public:
		IObjectComponent() = delete;
		IObjectComponent(FName _Name):Name{ _Name } { VE_ASSERT(!Name.IsNone()); }
		virtual ~IObjectComponent() = default;
				
	private:
		FName Name;
		//WeakPtr<VObject> Owner;
	};

} // namespace VE
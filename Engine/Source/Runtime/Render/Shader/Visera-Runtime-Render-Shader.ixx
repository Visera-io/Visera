module;
#include <Visera.h>
#include <slang.h>
#include <slang-com-ptr.h>
export module Visera.Runtime.Render.Shader;

import Visera.Core.Type;
import Visera.Runtime.RHI;

export namespace VE { namespace Runtime
{
	template<class T>
	using COMPtr = Slang::ComPtr<T>;

	class FSlang;

	class FShader
	{
		friend class FSlang;
	public:
		enum class EType	{ VulkanSPIRV, Default = VulkanSPIRV };
		auto GetName()		 const -> StringView { return Name; }
		auto GetEntryPoint() const -> StringView { return EntryPoint; }

		Bool IsCompiled() const { return Handle != nullptr; }

		FShader(StringView _Name, StringView _EntryPoint, EType _Type = EType::Default);

	private:
		COMPtr<slang::IComponentType> Handle {nullptr}; // Created after FSlang::CompileShader(...)
		EType  Type;
		String Name;
		String EntryPoint;
		//SharedPtr<> DescriptorSetLayout;
	};

	FShader::
	FShader(StringView _Name, StringView _EntryPoint, EType _Type/* = EType::Default*/)
		: Type{_Type},
		  Name{_Name},
		  EntryPoint{_EntryPoint}
	{
		
	}

} } // namespace VE::Runtime
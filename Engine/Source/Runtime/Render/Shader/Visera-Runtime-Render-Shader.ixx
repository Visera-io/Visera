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
		enum class ECompileType	{ VulkanSPIRV, Default = VulkanSPIRV };
		auto GetFileName()		const -> StringView { return FileName; }
		auto GetEntryPoint()	const -> StringView { return EntryPoint; }
		auto GetShaderStage()	const -> RHI::EShaderStage { return Stage; }
		auto GetRHIShader()		const -> SharedPtr<const RHI::FShader> { return RHIShader; }
		auto GetCompatiblePipelineLayout() const -> SharedPtr<const RHI::FPipelineLayout> { return CompatiblePipelineLayout; }

		Bool IsCompiled()  const { return RHIShader != nullptr; }
		Bool IsReflected() const { return CompatiblePipelineLayout != nullptr; }

		FShader(StringView _ShaderFileName, StringView _EntryPoint);

	private:
		COMPtr<slang::IComponentType> Handle {nullptr}; // Created after FSlang::CompileShader(...)
		String				FileName;
		String				EntryPoint;
		RHI::EShaderStage	Stage;
		ECompileType		Type;
		SharedPtr<RHI::FShader>			RHIShader;
		SharedPtr<RHI::FPipelineLayout> CompatiblePipelineLayout;
	};

	FShader::
	FShader(StringView _ShaderFileName, StringView _EntryPoint)
		: FileName{_ShaderFileName},
		  EntryPoint{_EntryPoint}
	{
		
	}

} } // namespace VE::Runtime
module;
#include <Visera.h>
#include <slang.h>
#include <slang-com-ptr.h>
export module Visera.Runtime.Render.Shader;

import Visera.Core.Type;
import Visera.Runtime.RHI;

export namespace VE
{
	template<class T>
	using COMPtr = Slang::ComPtr<T>;

	class FSlang;

	class FShader
	{
		friend class FSlang;
	public:
		enum class ECompileType	{ VulkanSPIRV, Default = VulkanSPIRV };
		
		Bool IsCompiled()  const { return Handle != nullptr; }

		auto GetFileName()		const -> StringView { return FileName; }
		auto GetEntryPoint()	const -> StringView { return EntryPoint; }
		auto GetShaderStage()	const -> RHI::EShaderStage { VE_ASSERT(IsCompiled()); return Handle->GetStage(); }
		auto GetRHIShader()		const -> SharedPtr<const RHI::FShader> { return Handle; }
		auto GetCompatiblePipelineLayout() const -> SharedPtr<const RHI::FPipelineLayout> { return CompatiblePipelineLayout; }

		FShader(StringView _ShaderFileName, StringView _EntryPoint, ECompileType _CompileType = ECompileType::Default);
		~FShader() = default;
	private:
		SharedPtr<RHI::FShader>		Handle;
		String						FileName;
		String						EntryPoint;
		ECompileType				Type;
		SharedPtr<RHI::FPipelineLayout> CompatiblePipelineLayout;
	};

	FShader::
	FShader(StringView _ShaderFileName,
			StringView _EntryPoint,
			ECompileType _CompileType/* = ECompileType::Default*/)
		: FileName{_ShaderFileName},
		  EntryPoint{_EntryPoint},
		  Type{_CompileType}
	{
		
	}

} // namespace VE
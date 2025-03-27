module;
#include <Visera.h>
#include <slang.h>
#include <slang-com-ptr.h>
export module Visera.Runtime.Render.Slang:Shader;

import Visera.Core.Type;
import Visera.Runtime.Render.RHI;

template<class T>
using COMPtr = Slang::ComPtr<T>;

export namespace VE
{

	class FSlangShader
	{
	public:
		enum class ECompileType	{ VulkanSPIRV, Default = VulkanSPIRV };
		
		Bool IsCompiled()  const { return Handle != nullptr; }

		auto GetType()			const -> ECompileType { return Type; }
		auto GetFileName()		const -> StringView { return FileName; }
		auto GetEntryPoint()	const -> StringView { return EntryPoint; }
		auto GetShaderStage()	const -> RHI::EShaderStage { VE_ASSERT(IsCompiled()); return Handle->GetStage(); }
		auto GetRHIShader()		const -> SharedPtr<const RHI::FShader> { return Handle; }
		auto GetCompatiblePipelineLayout() const -> SharedPtr<const RHI::FPipelineLayout> { return CompatiblePipelineLayout; }

		void Create(SharedPtr<RHI::FShader> _Handle) { Handle = std::move(_Handle); }

		FSlangShader(StringView _ShaderFileName, StringView _EntryPoint, ECompileType _CompileType = ECompileType::Default);
		~FSlangShader() = default;

	private:
		SharedPtr<RHI::FShader>		Handle;
		String						FileName;
		String						EntryPoint;
		ECompileType				Type;
		SharedPtr<RHI::FPipelineLayout> CompatiblePipelineLayout;
	};

	FSlangShader::
	FSlangShader(StringView _ShaderFileName,
				 StringView _EntryPoint,
				 ECompileType _CompileType/* = ECompileType::Default*/)
		: FileName{_ShaderFileName},
		  EntryPoint{_EntryPoint},
		  Type{_CompileType}
	{
		
	}

} // namespace VE
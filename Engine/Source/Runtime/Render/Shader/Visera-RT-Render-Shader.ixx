module;
#include <Visera.h>
#include <slang.h>
#include <slang-com-ptr.h>
export module Visera.Runtime.Render.Shader;
import Visera.Runtime.Render.Shader.Compiler;

import Visera.Core.Type;
import Visera.Core.Signal;

import Visera.Runtime.Render.RHI;

export namespace VE
{

	class FShader
	{
	public:
		enum class ELanguage { Slang };

		static inline auto
		Create(StringView _ShaderFileName, StringView _EntryPoint) { return CreateSharedPtr<FShader>(_ShaderFileName, _EntryPoint); }
		
		void Compile();
		Bool IsCompiled()  const { return Handle != nullptr; }

		auto GetLanguage()		const -> ELanguage  { return Language; }
		auto GetFileName()		const -> StringView { return FileName; }
		auto GetEntryPoint()	const -> StringView { return EntryPoint; }
		auto GetShaderStage()	const -> RHI::EShaderStage { VE_ASSERT(IsCompiled()); return Handle->GetStage(); }
		auto GetRHIShader()		const -> SharedPtr<const RHI::FShader> { return Handle; }
		auto GetCompatiblePipelineLayout() const -> SharedPtr<const RHI::FPipelineLayout> { return CompatiblePipelineLayout; }

		FShader() = delete;
		FShader(StringView _ShaderFileName, StringView _EntryPoint);
		~FShader() = default;

	private:
		SharedPtr<RHI::FShader>		Handle;
		String						FileName;   // Only Filename, Do not use FPath.
		String						EntryPoint;
		ELanguage				    Language = ELanguage::Slang;

		SharedPtr<RHI::FPipelineLayout> CompatiblePipelineLayout;
	};

	FShader::
	FShader(StringView   _ShaderFileName,
		    StringView   _EntryPoint)
		: FileName{_ShaderFileName},
		  EntryPoint{_EntryPoint}
	{
		Compile();
	}

	void FShader::
	Compile()
	{
		switch (Language)
		{
		case ELanguage::Slang:
		{
			static FSlangCompiler SlangCompiler{};
			Handle = SlangCompiler.CompileShader(FileName, EntryPoint);
			break;
		}
		default:throw SRuntimeError("Failed to compile the FShader - Unkowon Language!");
		}
	}

} // namespace VE
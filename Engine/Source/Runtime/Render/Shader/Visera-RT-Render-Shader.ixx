module;
#include <Visera.h>
#include <slang.h>
#include <slang-com-ptr.h>
export module Visera.Runtime.Render.Shader;
#define VE_MODULE_NAME "Shader"
import Visera.Runtime.Render.Shader.Compiler;

import Visera.Core.Log;
import Visera.Core.Type;
import Visera.Core.Signal;

import Visera.Runtime.Render.RHI;

export namespace VE
{

	class FShader : public std::enable_shared_from_this<FShader>
	{
	public:
		enum class ELanguage { Slang };

		static inline auto
		Create(StringView _ShaderFileName, StringView _EntryPoint) { return CreateSharedPtr<FShader>(_ShaderFileName, _EntryPoint); }
		auto inline
		Compile() -> SharedPtr<FShader>;

		Bool IsCompiled()  const { return Handle != nullptr; }

		auto GetLanguage()		const -> ELanguage  { return Language; }
		auto GetFileName()		const -> StringView { return FileName; }
		auto GetEntryPoint()	const -> StringView { return EntryPoint; }
		auto GetShaderStage()	const -> RHI::EShaderStage { VE_ASSERT(IsCompiled()); return Handle->GetStage(); }
		auto GetCompiledShader()const -> SharedPtr<const RHI::FSPIRVShader> { VE_ASSERT(IsCompiled()); return Handle; }
		auto GetCompatiblePipelineLayout() const -> SharedPtr<const RHI::FPipelineLayout> { return CompatiblePipelineLayout; }

		FShader() = delete;
		FShader(StringView _ShaderFileName, StringView _EntryPoint);
		~FShader() { if (!IsCompiled()) { VE_LOG_WARN("You may forget to compile this shader: {}", FileName)} }

	private:
		SharedPtr<RHI::FSPIRVShader>Handle;
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

	}

	SharedPtr<FShader> FShader::
	Compile()
	{
		if (IsCompiled()) { return shared_from_this(); }
		VE_LOG_DEBUG("Starting compiling the {}(entry point:{}).", FileName, EntryPoint);

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

		return shared_from_this();
	}

} // namespace VE
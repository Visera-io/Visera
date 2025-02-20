module;
#include <Visera.h>
export module Visera.Runtime.Render;
import Visera.Runtime.Render.Slang;
import Visera.Runtime.Render.Shader;

import Visera.Core.Log;
import Visera.Core.Type;
import Visera.Core.Signal;
import Visera.Runtime.RHI;

export namespace VE
{
	class Render //[TODO]: Remove and add funcs to RHI?
	{
		VE_MODULE_MANAGER_CLASS(Render);
	public:
		enum class ESystemRT { Color, Depth };
		using FShader = FShader;

		VE_API CreateShader(StringView _ShaderFileName, StringView _EntryPoint, FShader::ECompileType _CompileType = FShader::ECompileType::Default) throw (SIOFailure, SRuntimeError) -> SharedPtr<FShader>;

	private:
		static void inline Bootstrap();
		static void inline Tick();
		static void inline Terminate();
	};
	
	SharedPtr<FShader> Render::
	CreateShader(StringView _ShaderFileName,
				 StringView _EntryPoint,
				 FShader::ECompileType _CompileType/*Default*/)
	throw (SIOFailure, SRuntimeError)
	{ 
		static FSlang Slang{}; // Lazy Load
		auto Shader = CreateSharedPtr<FShader>(_ShaderFileName, _EntryPoint);
		Slang.CompileShader(Shader);

		return Shader;
	}

	void Render::
	Bootstrap()
	{

	}

	void Render::
	Terminate()
	{
		
	}

} // namespace VE
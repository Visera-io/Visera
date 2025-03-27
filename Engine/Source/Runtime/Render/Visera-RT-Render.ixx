module;
#include <Visera.h>
export module Visera.Runtime.Render;
export import Visera.Runtime.Render.Slang;
export import Visera.Runtime.Render.RHI;
export import Visera.Runtime.Render.RTC;

import Visera.Core.Log;
import Visera.Core.Type;
import Visera.Core.Signal;

export namespace VE
{
	using FShader = FSlangShader;

	class Render //[TODO]: Remove and add funcs to RHI?
	{
		VE_MODULE_MANAGER_CLASS(Render);
	public:
		enum class ESystemRT { Color, Depth };

		VE_API CreateShader(StringView _ShaderFileName, StringView _EntryPoint, FShader::ECompileType _CompileType = FShader::ECompileType::Default) -> SharedPtr<FShader>;

		static void inline Bootstrap();
		static void inline Terminate();
	};
	
	SharedPtr<FShader> Render::
	CreateShader(StringView _ShaderFileName,
				 StringView _EntryPoint,
				 FShader::ECompileType _CompileType/*Default*/)
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
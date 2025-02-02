module;
#include <Visera.h>
export module Visera.Runtime.Render;
import Visera.Runtime.Render.Slang;
import Visera.Runtime.Render.Shader;

import Visera.Core.Log;
import Visera.Core.Type;
import Visera.Core.Signal;
import Visera.Runtime.RHI;

export namespace VE { namespace Runtime
{
	class ViseraRuntime;

	class Render
	{
		VE_MODULE_MANAGER_CLASS(Render);
		friend class ViseraRuntime;
	public:
		VE_API CreateShader(StringView _ShaderName, StringView _EntryPoint) throw (SIOFailure, SRuntimeError) -> SharedPtr<FShader>;

	private:
		static void Bootstrap() {  };
		static void Tick()	    {  };
		static void Terminate() {  };
	};
	
	SharedPtr<FShader> Render::
	CreateShader(StringView _ShaderName, StringView _EntryPoint)
	throw (SIOFailure, SRuntimeError)
	{ 
		static FSlang Slang{}; // Lazy load
		// Create Shader
		auto Shader = CreateSharedPtr<FShader>(_ShaderName, _EntryPoint);
		
		// Compile Shader
		Slang.CompileShader(Shader);
		//Slang.ReflectShader(Shader);

		return Shader;
	}

} } // namespace VE::Runtime
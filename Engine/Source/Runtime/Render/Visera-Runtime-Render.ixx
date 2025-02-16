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
		enum class ESystemRT { Color, Depth };
		using FShader = FShader;

		VE_API CreateShader(StringView _ShaderFileName, StringView _EntryPoint, FShader::ECompileType _CompileType = FShader::ECompileType::Default) throw (SIOFailure, SRuntimeError) -> SharedPtr<FShader>;

	private:
		static void Bootstrap();
		static void Tick();
		static void Terminate();
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
	Tick()
	{
		static UInt32 FrameCount = 0;
		Log::Info("Frame ({})", ++FrameCount);
		auto& Frame = RHI::WaitNextFrame();
		{
			auto DrawCmds = Frame.GetGraphicsCommandBuffer();
			
		}
		RHI::RenderAndPresentCurrentFrame();
	}

	void Render::
	Terminate()
	{
		
	}

} } // namespace VE::Runtime
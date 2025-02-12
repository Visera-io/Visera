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
		class FFrame
		{
			friend class Render;
		public:
			SharedPtr<RHI::FCommandBuffer> DrawcallBuffer;
		private:
			//Array<RHI::FSemaphore> WaitSemaphores;
			Array<RHI::FSemaphore> SignalSemaphores;
		};

		VE_API CreateShader(StringView _ShaderFileName, StringView _EntryPoint, FShader::ECompileType _CompileType = FShader::ECompileType::Default) throw (SIOFailure, SRuntimeError) -> SharedPtr<FShader>;
		VE_API GetCurrentFrame() -> FFrame& { return Frames[RHI::GetSwapchainCursor()]; }

	private:
		static inline Array<FFrame> Frames;

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
		Frames.resize(RHI::GetSwapchainFrameCount());

		for (auto& Frame : Frames)
		{
			Frame.DrawcallBuffer = RHI::CreateCommandBuffer();
			auto& SignalSemaphore = Frame.SignalSemaphores.emplace_back(RHI::CreateSemaphore());
			Frame.DrawcallBuffer->AddSignalSemaphore(SignalSemaphore);
		}
	}

	void Render::
	Tick()
	{
		auto& CurrentFrame = GetCurrentFrame();
		if (!CurrentFrame.DrawcallBuffer->IsIdle())
		{
			Log::Warn("You may forget to stop recording DrawcallBuffer?");
			CurrentFrame.DrawcallBuffer->StopRecording();
		}
		RHI::RenderAndPresent(CurrentFrame.DrawcallBuffer);
	}

	void Render::
	Terminate()
	{
		Frames.clear();
	}

} } // namespace VE::Runtime
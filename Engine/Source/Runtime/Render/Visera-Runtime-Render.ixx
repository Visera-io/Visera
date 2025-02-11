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
		using FShader = FShader;
		VE_API CreateShader(StringView _ShaderFileName, StringView _EntryPoint, FShader::ECompileType _CompileType = FShader::ECompileType::Default) throw (SIOFailure, SRuntimeError) -> SharedPtr<FShader>;

		enum class ESystemRT { Color, Depth };

	private:
		/*struct FFrame
		{
			SharedPtr<RHI::FRenderTarget> RenderTargets;
			SharedPtr<RHI::FCommandBuffer> Drawcalls;
		};
		static inline Array<FFrame> Frames;*/
		
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
		static FSlang Slang{}; // Lazy load

		auto Shader = CreateSharedPtr<FShader>(_ShaderFileName, _EntryPoint);
		Slang.CompileShader(Shader);
		Slang.ReflectShader(Shader);

		return Shader;
	}

	void Render::
	Bootstrap()
	{
		/*Frames.resize(RHI::GetAPI()->GetSwapchain().GetSize());
		for (auto& Frame : Frames)
		{
			Frame.RenderTargets = RHI::CreateRenderTargets(
				{
				RHI::CreateImage(
					RHI::EImageType::Image2D,
					RHI::FExtent3D{ 1920, 1080, 1 },
					RHI::EFormat::U32_sRGB_R8_G8_B8_A8,
					RHI::EImageAspect::Color,
					RHI::EImageUsage::ColorAttachment
					| RHI::EImageUsage::InputAttachment
					| RHI::EImageUsage::TransferSource)
				},
				RHI::CreateImage(
					RHI::EImageType::Image2D,
					RHI::FExtent3D{ 1920, 1080, 1 },
					RHI::EFormat::U32_sRGB_R8_G8_B8_A8,
					RHI::EImageAspect::Color,
					RHI::EImageUsage::ColorAttachment
					| RHI::EImageUsage::InputAttachment
					| RHI::EImageUsage::TransferSource)
			);
		}*/
	}

	void Render::
	Tick()
	{

	}

	void Render::
	Terminate()
	{
		//Frames.clear();
	}

} } // namespace VE::Runtime
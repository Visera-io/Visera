module;
#include <ViseraEngine>

export module Visera.Runtime;
export import Visera.Runtime.Platform;
export import Visera.Runtime.Render;
import Visera.Runtime.Context;

import Visera.Core.Log;
import Visera.Internal;

export namespace VE
{
	class Visera;

	class ViseraRuntime
	{
		friend class Visera;
	private:
		static inline Bool
		Tick()
		{
			if (!RuntimeContext::MainLoop.ShouldStop())
			{
				PlatformRuntime::Tick();
				RenderRuntime::Tick();
				return True;
			}
			return False;
		}

		static inline void
		Bootstrap()
		{
			RuntimeContext::Bootstrap();
			PlatformRuntime::Bootstrap();
			RenderRuntime::Bootstrap();
		}

		static inline void
		Terminate()
		{
			RenderRuntime::Terminate();
			PlatformRuntime::Terminate();
			RuntimeContext::Terminate();
		}

		ViseraRuntime() noexcept = default;
	};

} // namespace VE

export namespace VISERA_APP_NAMESPACE
{

	enum class DrawCallLevel
	{ 
		Primary		= VE::Render::VulkanCommandPool::CommandBuffer::Level::Primary,
		Secondary	= VE::Render::VulkanCommandPool::CommandBuffer::Level::Secondary,
	};
	auto CreateDrawCallBuffer(DrawCallLevel Level) { return VE::Render::RHI::GetDefaultCommandPool().Allocate(VE::Render::VulkanCommandPool::CommandBuffer::Level(Level)); }

} // namespace VISERA_APP_NAMESPACE
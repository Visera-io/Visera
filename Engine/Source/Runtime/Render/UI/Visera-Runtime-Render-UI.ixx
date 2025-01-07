module;
#include <Visera.h>

export module Visera.Runtime.Render.UI;

export namespace VE
{

	class Layer
	{
		friend class RenderModule;
	public:

	private:
		Layer() noexcept = default;
		static void
		Bootstrap()
		{
		}
		static void
		Terminate()
		{
			
		}
	};

VISERA_MODULE_END
module;
#include <ViseraEngine>

export module Visera.Runtime.Render.UI;

export namespace VE { namespace UI
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

} } // namespace VE::UI
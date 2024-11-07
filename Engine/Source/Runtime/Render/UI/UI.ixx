module;
#include <Visera>

export module Visera.Engine.Runtime.Render.UI;

export namespace VE { namespace Runtime
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

} } // namespace VE::Runtime
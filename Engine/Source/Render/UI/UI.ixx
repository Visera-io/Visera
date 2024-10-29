module;
#include <ViseraEngine>

export module Visera.Render.UI;

export namespace VE
{
	class ViseraRender;

	class UI
	{
		friend class ViseraRender;
	public:

	private:
		UI() noexcept = default;
		static void
		Bootstrap()
		{
			
		}
		static void
		Terminate()
		{
			
		}
	};

} // namespace VE
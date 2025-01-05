module;
#include <Visera.h>
export module ViseraCanvas;
import Visera;

export namespace VISERA_APP_NAMESPACE
{
	class App final : public VE::ViseraApp
	{
	public:
		virtual void Tick() override
		{
			Log::Info("Hello World");


			Exit(); // Onetime
		}

		virtual void RenderTick() override
		{
			
		}

		virtual void Bootstrap() override
		{

		}

		virtual void Terminate() override
		{

		}
	};
} // namespace VISERA_APP_NAMESPACE
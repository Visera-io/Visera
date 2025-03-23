module;
#include <Visera.h>
#include <string>
#if (VE_IS_WINDOWS_SYSTEM)
#include <Windows.h>
#endif
export module Visera;
export import Visera.Core;
export import Visera.Runtime;
export import Visera.Editor;

export namespace VE
{

	class ViseraApp
	{
	public:
		virtual void Bootstrap() = 0;
		virtual void Terminate() = 0;
		virtual void Tick() = 0;

		void inline
		Exit(const SAppStop& Message = SAppStop("ViseraEngine App Exited Successfully.")) const { throw Message; }

		ViseraApp()	 noexcept { RuntimeCounter = System::GetRunningTime().milliseconds(); }
		virtual ~ViseraApp() noexcept { Log::Info("App Running Time {}ms", System::GetRunningTime().milliseconds() - RuntimeCounter); }

	private:
		UInt64 RuntimeCounter = 0;
	};

	class ViseraEngine
	{
		VE_MODULE_MANAGER_CLASS(ViseraEngine);
	public:
		VE_API Run(ViseraApp* App) -> ErrorCode
		{
			Int32 StateCode = EXIT_SUCCESS;

			try
			{
				if (!App) { Log::Fatal("ViseraEngine App is not created!"); }
				Bootstrap(App);
				{
					while (!Window::ShouldClose())
					{
						Window::PollEvents();

						auto& Frame = RHI::WaitFrameReady();
						Editor::BeginFrame(Frame.GetEditorCommandBuffer());

						try
						{
							App->Tick();
						}
						catch (const SRuntimeError& Signal)
						{
							Log::Error("ViseraApp Runtime Error:\n{}{}", Signal.What(), Signal.Where());
							StateCode = Signal.StateCode;
							//App->Exit();
						}
						World::Update();

						Editor::EndFrame(Frame.GetEditorCommandBuffer());
						RHI::RenderAndPresentCurrentFrame();
					}
				}

			}
			catch (const SRuntimeError& Signal)
			{
				Log::Error("ViseraEngine Runtime Error:\n{}{}", Signal.What(), Signal.Where());
				StateCode = Signal.StateCode;
			}
			catch (const SAppStop& Signal)
			{
				Log::Debug(VISERA_APP_NAME "Exited:\n{}{}", Signal.What(), Signal.Where());
				StateCode = Signal.StateCode;
			}
			catch (const SEngineStop& Signal)
			{
				Log::Debug("ViseraEngine Stopped:\n{}{}", Signal.What(), Signal.Where());
				StateCode = Signal.StateCode;
			}

			Terminate(App);
			Log::Debug("ViseraEngine has been terminated successfully!");
			return StateCode;
		}

	private:
		static inline void
		Bootstrap(ViseraApp* _App)
		{
#if defined(VE_ON_WINDOWS_SYSTEM)
			SetConsoleOutputCP(65001); // Enable Terminal WideString Output
#endif
			Log::Debug("Bootstrapping ViseraEngine Runtime...");
			Window::Bootstrap();
			RHI::Bootstrap();
			World::Bootstrap();
			Render::Bootstrap();

			Log::Debug("Bootstrapping ViseraEngine Edtior...");
			Editor::Bootstrap();

			Log::Debug("Bootstrapping the " VISERA_APP_NAME "...");
			_App->Bootstrap();
		}

		static inline void
		Terminate(ViseraApp* _App)
		{	
			RHI::WaitDeviceIdle();
			Log::Debug("Terminating ViseraEngine App...");
			_App->Terminate();
			delete _App;

			Log::Debug("Terminating ViseraEngine Editor...");
			Editor::Terminate();

			Log::Debug("Terminating ViseraEngine Runtime...");
			Render::Terminate();
			World::Terminate();
			RHI::Terminate();
			Window::Terminate();
		}
	};

} // namespace VE

#if (VE_IS_APPLE_SYSTEM)
// Testing Code
class App final : public VE::ViseraApp
{
public:
	virtual void Tick() override
	{
		VE::Editor::CreateWindow();

	}

	virtual void Bootstrap() override
	{

	}

	virtual void Terminate() override
	{

	}
};

export int main(int argc, char* argv[])
{
	return VE::ViseraEngine::Run(new App());
}
#endif
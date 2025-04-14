module;
#include <Visera.h>
#include <string>
#if (VE_IS_WINDOWS_SYSTEM)
#include <Windows.h>
#endif
export module Visera;
#define VE_MODULE_NAME "Visera"
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

		ViseraApp()	 noexcept { RuntimeCounter = OS::GetRunningTime().milliseconds(); }
		virtual ~ViseraApp() noexcept { Log::Info("App Running Time {}ms", OS::GetRunningTime().milliseconds() - RuntimeCounter); }

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
				if (!App) { VE_LOG_FATAL("App is not created!"); }
				Bootstrap(App);
				{
					while (!Window::ShouldClose())
					{
						Window::PollEvents();
						IO::ProcessEvents();

						auto& Frame = RHI::WaitFrameReady();
						Editor::BeginFrame(Frame.GetEditorCommandBuffer());

						try
						{
							App->Tick();
						}
						catch (const SRuntimeError& Signal)
						{
							VE_LOG_ERROR("ViseraApp Runtime Error:\n{}{}", Signal.What(), Signal.Where());
							StateCode = Signal.StateCode;
							//App->Exit();
						}
						World::Update();

						Editor::Display();
						Editor::EndFrame(Frame.GetEditorCommandBuffer());
						RHI::RenderAndPresentCurrentFrame();
					}
				}

			}
			catch (const SRuntimeError& Signal)
			{
				VE_LOG_ERROR("ViseraEngine Runtime Error:\n{}{}", Signal.What(), Signal.Where());
				StateCode = Signal.StateCode;
			}
			catch (const SAppStop& Signal)
			{
				VE_LOG_DEBUG(VISERA_APP_NAME "Exited:\n{}{}", Signal.What(), Signal.Where());
				StateCode = Signal.StateCode;
			}
			catch (const SEngineStop& Signal)
			{
				VE_LOG_DEBUG("ViseraEngine Stopped:\n{}{}", Signal.What(), Signal.Where());
				StateCode = Signal.StateCode;
			}

			Terminate(App);
			VE_LOG_DEBUG("Visera Engine has been terminated successfully!");
			return StateCode;
		}

	private:
		static inline void
		Bootstrap(ViseraApp* _App)
		{
#if defined(VE_ON_WINDOWS_SYSTEM)
			SetConsoleOutputCP(65001); // Enable Terminal WideString Output
#endif
			VE_LOG_DEBUG("Bootstrapping Runtime...");

			VE_LOG_TRACE("Bootstrapping Window...");
			Window::Bootstrap();
			VE_LOG_TRACE("Bootstrapping IO...");
			IO::Bootstrap();
			VE_LOG_TRACE("Bootstrapping RHI...");
			RHI::Bootstrap();
			VE_LOG_TRACE("Bootstrapping RTC...");
			RTC::Bootstrap();
			VE_LOG_TRACE("Bootstrapping World...");
			World::Bootstrap();
			VE_LOG_TRACE("Bootstrapping Render...");
			Render::Bootstrap();

			VE_LOG_DEBUG("Bootstrapping Edtior...");
			Editor::Bootstrap();

			VE_LOG_DEBUG("Bootstrapping the " VISERA_APP_NAME "...");
			_App->Bootstrap();
		}

		static inline void
		Terminate(ViseraApp* _App)
		{	
			RHI::WaitDeviceIdle();
			VE_LOG_DEBUG("Terminating App...");
			_App->Terminate();
			delete _App;

			VE_LOG_DEBUG("Terminating Editor...");
			Editor::Terminate();

			VE_LOG_DEBUG("Terminating Runtime...");

			VE_LOG_TRACE("Terminating Render...");
			Render::Terminate();
			VE_LOG_TRACE("Terminating World...");
			World::Terminate();
			VE_LOG_TRACE("Terminating RTC...");
			RTC::Terminate();
			VE_LOG_TRACE("Terminating Render...");
			RHI::Terminate();
			VE_LOG_TRACE("Terminating Render...");
			IO::Terminate();
			VE_LOG_TRACE("Terminating Render...");
			Window::Terminate();
		}
	};

} // namespace VE
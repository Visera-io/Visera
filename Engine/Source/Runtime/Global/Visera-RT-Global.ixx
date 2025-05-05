module;
#include <Visera.h>
export module Visera.Runtime.Global;
#define VE_MODULE_NAME "Global"
import Visera.Core.Log;
import Visera.Core.Type;
import Visera.Core.OS.FileSystem;

export namespace VE
{
	struct FPathCache
	{
		FPath Configs = FPath{VISERA_ENGINE_CONFIGS_DIR};
		FPath Assets  = FPath{VISERA_ENGINE_ASSETS_DIR};
	};

	class GEngine
	{
		VE_MODULE_MANAGER_CLASS(GEngine);
	public:
		static inline auto
		GetVersion() -> StringView { return Version; }
		static inline auto
		GetConfigs() -> const auto& { return Configs["Engine"]; }

		static const inline FPathCache Path;

	private:
		static inline RawString Version;
		static inline FJSON     Configs;

	public:
		static inline void
		Bootstrap()
		{
			Configs = FileSystem::CreateJSONFile(Path.Configs.Join(FPath{"Configs.json"}))->Parse();
			Version = GetConfigs()["Version"].GetString();
		}

		static inline void
		Terminate()
		{

		}
	};
	
} // namespace VE
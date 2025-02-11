module;
#include <Visera.h>
#include <slang.h>
#include <slang-com-ptr.h>
export module Visera.Runtime.Render.Slang;

import Visera.Runtime.Render.Shader;
import Visera.Runtime.RHI;
import Visera.Core.Signal;

export namespace VE { namespace Runtime
{
	class Render;
	
	template<class T>
	using COMPtr = Slang::ComPtr<T>;

	class FSlang
	{
		friend class Render;
	public:
		void CompileShader(SharedPtr<FShader> _Shader) const throw (SRuntimeError);

		FSlang();
		~FSlang() = default;

	private:
		COMPtr<slang::IGlobalSession> Context;		//[Note] Currently, the global session type is not thread-safe. Applications that wish to compile on multiple threads will need to ensure that each concurrent thread compiles with a distinct global session.
		struct FCompiler
		{
			COMPtr<slang::ISession>   Session;
			slang::TargetDesc		  Target;
			Array<RawString>		  ShaderPaths;
		};
		FCompiler					  VulkanSPIRVCompiler;
	};

	FSlang::
	FSlang()
	{
		// Create Slang Context
		if (slang::createGlobalSession(Context.writeRef()) != SLANG_OK)
		{ throw SRuntimeError("Failed to create the Slang Context (a.k.a, Global Session)!"); }

		// Create Vulkan Compiler Session
		VulkanSPIRVCompiler.Target =
		{
			.format  = SLANG_SPIRV,
			.profile = Context->findProfile("glsl_450"),
		};
		VulkanSPIRVCompiler.ShaderPaths = 
		{
			VISERA_ENGINE_SHADERS_DIR,
			VISERA_APP_SHADERS_DIR,
		};
		slang::SessionDesc VulkanShaderCompilerCreateInfo
		{
			.targets		 = &VulkanSPIRVCompiler.Target,
			.targetCount	 = 1,
			.searchPaths	 = VulkanSPIRVCompiler.ShaderPaths.data(),
			.searchPathCount = UInt32(VulkanSPIRVCompiler.ShaderPaths.size()),
		};
		if (Context->createSession(VulkanShaderCompilerCreateInfo, VulkanSPIRVCompiler.Session.writeRef()) != SLANG_OK)
		{ throw SRuntimeError("Failed to create the Slang Vulkan Compiler Session!"); }
	}

	void FSlang::
	CompileShader(SharedPtr<FShader> _Shader) const
	throw (SRuntimeError)
	{
		COMPtr<slang::IBlob>  Diagnostics;
		// Select Compiler
		const FSlang::FCompiler* Compiler = nullptr;
		switch (_Shader->Type)
		{
		case FShader::ECompileType::VulkanSPIRV:
			Compiler = &VulkanSPIRVCompiler;
			break;
		default:
			throw SRuntimeError("Unkonwn Shader TypeView!");
		}

		// Create Shader Module
		COMPtr<slang::IModule> ShaderModule{ Compiler->Session->loadModule(_Shader->GetFileName().data(), Diagnostics.writeRef()) };
		if (Diagnostics) { throw SRuntimeError(Text("Failed to create Slang Shader Module:\n{}", RawString(Diagnostics->getBufferPointer()))); }
		
		// Create Shader Program
		COMPtr<slang::IEntryPoint> ShaderEntryPoint;
		if (ShaderModule->findEntryPointByName(
			_Shader->GetEntryPoint().data(),
			ShaderEntryPoint.writeRef()) != SLANG_OK)
		{ throw SRuntimeError(Text("Failed to find the EntryPoint({}) from Shader({})!", _Shader->GetEntryPoint(), _Shader->GetFileName())); }

		const auto ShaderComponents = Segment<slang::IComponentType*, 2> 
		{ 
			reinterpret_cast<slang::IComponentType*>(ShaderModule.get()),
			reinterpret_cast<slang::IComponentType*>(ShaderEntryPoint.get())
		};
		if (Compiler->Session->createCompositeComponentType(
			ShaderComponents.data(),
			ShaderComponents.size(),
			_Shader->Handle.writeRef(),
			Diagnostics.writeRef()) != SLANG_OK)
		{ throw SRuntimeError(Text("Failed to create the Shader({}):\n{}", _Shader->GetFileName(), RawString(Diagnostics->getBufferPointer()))); }
	
		COMPtr<slang::IBlob> CompiledCode;
		if (_Shader->Handle->getEntryPointCode(
			0,
			0,
			CompiledCode.writeRef(),
			Diagnostics.writeRef()) != SLANG_OK)
		{ throw SRuntimeError(Text("Failed to obtain compiled shader code from {}! -- {}", _Shader->GetFileName(), RawString(Diagnostics->getBufferPointer()))); }
	
		// Reflect Shader
		slang::ProgramLayout* ShaderLayout = _Shader->Handle->getLayout(0, Diagnostics.writeRef());
		if (Diagnostics) { throw SRuntimeError(Text("Failed to get reflection info from Shader({})! -- {}", _Shader->GetFileName(), RawString(Diagnostics->getBufferPointer()))); }
		
		auto* EntryPointRef = ShaderLayout->findEntryPointByName(_Shader->GetEntryPoint().data());

		RHI::EShaderStage ShaderStage{};
		switch (EntryPointRef->getStage())
		{
		case SLANG_STAGE_VERTEX:	ShaderStage = RHI::EShaderStage::Vertex;   break;
		case SLANG_STAGE_FRAGMENT:	ShaderStage = RHI::EShaderStage::Fragment; break;
		default: throw SRuntimeError("Unsupported Shader Type!");
		}

		std::cerr << Text("Warn: Func(ReflectShader) is WIP but is still being used for testing!\n");
		
		_Shader->RHIShader = RHI::CreateShader(ShaderStage,
											   CompiledCode->getBufferPointer(),
											   CompiledCode->getBufferSize());
	}

} } // namespace VE::Runtime
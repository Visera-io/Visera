module;
#include <Visera.h>
#include <slang.h>
#include <slang-com-ptr.h>
export module Visera.Runtime.Render.Shader.Compiler:Slang;
#define VE_MODULE_NAME "Compiler:Slang"

import Visera.Core.Log;
import Visera.Core.Signal;
import Visera.Runtime.Render.RHI;

export namespace VE
{

	class FSlangCompiler
	{
	public:
		auto CompileShader(StringView _FileName, StringView _EntryPoint) const -> SharedPtr<RHI::FSPIRVShader>;

		FSlangCompiler();
		~FSlangCompiler() = default;

	private:
		Slang::ComPtr<slang::IGlobalSession> Context;		//[Note] Currently, the global session type is not thread-safe. Applications that wish to compile on multiple threads will need to ensure that each concurrent thread compiles with a distinct global session.
		struct FCompiler
		{
			Slang::ComPtr<slang::ISession>   Session;
			slang::TargetDesc		  Target;
			Array<RawString>		  ShaderPaths; //[TODO]: FPath
		};
		FCompiler					  VulkanSPIRVCompiler;
	};

	FSlangCompiler::
	FSlangCompiler()
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
		if (Context->createSession(VulkanShaderCompilerCreateInfo,
			                       VulkanSPIRVCompiler.Session.writeRef()) != SLANG_OK)
		{
			String Info = Text("Failed to create the Compiler Session! -- throw(SRuntimeError)");
			VE_LOG_ERROR("{}", Info);
			throw SRuntimeError(Info);
		}
	}

	 SharedPtr<RHI::FSPIRVShader> FSlangCompiler::
	 CompileShader(StringView _FileName, StringView _EntryPoint) const
	 {
	 	Slang::ComPtr<slang::IBlob>  Diagnostics;
	 	// Select Compiler
	 	const FSlangCompiler::FCompiler* Compiler = &VulkanSPIRVCompiler;

	 	// Create Shader Module
	 	Slang::ComPtr<slang::IModule> ShaderModule{ Compiler->Session->loadModule(_FileName.data(), Diagnostics.writeRef())};
	 	if (Diagnostics)
	 	{
	 		String Info = Text("Failed to create the Shader Module: {}! -- throw(SRuntimeError)",
	 			               RawString(Diagnostics->getBufferPointer()));
	 		VE_LOG_ERROR("{}", Info);
	 		throw SRuntimeError(Info);
	 	}

	 	// Create Shader Program
	 	Slang::ComPtr<slang::IEntryPoint> ShaderEntryPoint;
	 	if (ShaderModule->findEntryPointByName(
	 		_EntryPoint.data(),
	 		ShaderEntryPoint.writeRef()) != SLANG_OK)
	 	{
	 		String Info = Text("Failed to find the EntryPoint({}) from Shader({})! -- throw(SRuntimeError)",
	 			               _EntryPoint.data(), _FileName.data());
	 		VE_LOG_ERROR("{}", Info);
	 		throw SRuntimeError(Info);
	 	}

	 	const auto ShaderComponents = Segment<slang::IComponentType*, 2> 
	 	{ 
	 		reinterpret_cast<slang::IComponentType*>(ShaderModule.get()),
	 		reinterpret_cast<slang::IComponentType*>(ShaderEntryPoint.get())
	 	};

	 	Slang::ComPtr<slang::IComponentType> ShaderProgram {nullptr};
	 	if (Compiler->Session->createCompositeComponentType(
	 		ShaderComponents.data(),
	 		ShaderComponents.size(),
	 		ShaderProgram.writeRef(),
	 		Diagnostics.writeRef()) != SLANG_OK)
	 	{
	 		String Info = Text("Failed to create the Shader({}): {}! -- throw(SRuntimeError)",
	 		                   _FileName.data(), RawString(Diagnostics->getBufferPointer()));
	 		VE_LOG_ERROR("{}", Info);
	 		throw SRuntimeError(Info);
	 	}
	
	 	Slang::ComPtr<slang::IBlob> CompiledCode;
	 	if (ShaderProgram->getEntryPointCode(
	 		0,
	 		0,
	 		CompiledCode.writeRef(),
	 		Diagnostics.writeRef()) != SLANG_OK)
	 	{
	 		String Info = Text("Failed to obtain compiled shader code from {}: {}! -- throw(SRuntimeError)",
	 		                   _FileName.data(), RawString(Diagnostics->getBufferPointer()));
	 		VE_LOG_ERROR("{}", Info);
	 		throw SRuntimeError(Info);
	 	}

	 	// Reflect Shader
	 	slang::ProgramLayout* ShaderLayout = ShaderProgram->getLayout(0, Diagnostics.writeRef());
	 	if (Diagnostics)
		{
	 		String Info = Text("Failed to get reflection info from Shader({}): {} -- throw(SRuntimeError)",
							   _FileName.data(), RawString(Diagnostics->getBufferPointer()));
	 		VE_LOG_ERROR("{}", Info);
	 		throw SRuntimeError(Info);
		}
		
	 	auto* EntryPointRef = ShaderLayout->findEntryPointByName(_EntryPoint.data());

	 	RHI::EShaderStage ShaderStage{};
	 	switch (EntryPointRef->getStage())
	 	{
	 	case SLANG_STAGE_VERTEX:	ShaderStage = RHI::EShaderStage::Vertex;   break;
	 	case SLANG_STAGE_FRAGMENT:	ShaderStage = RHI::EShaderStage::Fragment; break;
	 	default: VE_LOG_FATAL("Unsupported Shader PoolType!");
	 	}

	 	auto RHIShader = RHI::CreateShader(ShaderStage,
	 					                   CompiledCode->getBufferPointer(),
	 					                   CompiledCode->getBufferSize());

		return RHIShader;
	 }

} // namespace VE
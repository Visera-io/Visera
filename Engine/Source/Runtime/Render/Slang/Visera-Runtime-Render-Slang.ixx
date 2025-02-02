module;
#include <Visera.h>
#include <slang.h>
#include <slang-com-ptr.h>
export module Visera.Runtime.Render.Slang;
import Visera.Runtime.Render.Shader;

import Visera.Core.Signal;

export namespace VE { namespace Runtime
{

	template<class T>
	using COMPtr = Slang::ComPtr<T>;

	class FSlang
	{
	public:
		void CompileShader(SharedPtr<FShader> _Shader) const throw (SRuntimeError);
		void ReflectShader(SharedPtr<FShader> _Shader) const throw (SRuntimeError);

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
		case FShader::EType::VulkanSPIRV:
			Compiler = &VulkanSPIRVCompiler;
			break;
		default:
			throw SRuntimeError("Unkonwn Shader Type!");
		}

		// Create Shader Module
		COMPtr<slang::IModule> ShaderModule{ Compiler->Session->loadModule(_Shader->GetName().data(), Diagnostics.writeRef()) };
		if (Diagnostics) { throw SRuntimeError(Text("Failed to create Slang Shader Module:\n{}", RawString(Diagnostics->getBufferPointer()))); }

		// Create Shader Program
		COMPtr<slang::IEntryPoint> ShaderEntryPoint;
		if (ShaderModule->findEntryPointByName(_Shader->GetEntryPoint().data(), ShaderEntryPoint.writeRef()) != SLANG_OK)
		{ throw SRuntimeError(Text("Failed to find the EntryPoint({}) from Shader({})!", _Shader->GetEntryPoint(), _Shader->GetName())); }

		slang::IComponentType* ShaderComponents[2] = 
		{ 
			reinterpret_cast<slang::IComponentType*>(ShaderModule.get()),
			reinterpret_cast<slang::IComponentType*>(ShaderEntryPoint.get())
		};
		Compiler->Session->createCompositeComponentType(ShaderComponents, 2, _Shader->Handle.writeRef(), Diagnostics.writeRef());
		if (Diagnostics) { throw SRuntimeError(Text("Failed to create the Shader({}):\n{}", _Shader->GetName(), RawString(Diagnostics->getBufferPointer()))); }
	}

	void FSlang::
	ReflectShader(SharedPtr<FShader> _Shader) const
	throw (SRuntimeError)
	{
		VE_ASSERT(_Shader->IsCompiled());
		VE_WIP;
		
			///*Note that because both IModule and IEntryPoint inherit from IComponentType,
			//  they can also be queried for their layouts individually.
			//  The layout for a module comprises just its global-scope parameters.
			//  The layout for an entry point comprises just its entry-point parameters
			//  (both uniform and varying).*/
			//slang::ProgramLayout* ProgramLayout = Program->getLayout(0, Diagnostics.writeRef());
			//if (Diagnostics) { Log::Fatal(RawString(Diagnostics->getBufferPointer())); }

			///*A key property of GPU shader programming is that
			//  the same type may be laid out differently, depending on how it is used.
			//  For example, a user-defined struct type Stuff will often be laid out differently
			//  if it is used in a ConstantBuffer<Stuff> than in a StructuredBuffer<Stuff>.
			//  Because the same thing can be laid out in multiple ways (even within the same program),
			//  the Slang reflection API represents types and variables as distinct things from the layouts applied to them. */
			////slang::ShaderReflection* Reflection = Module->ref
			//auto* R = ProgramLayout->findTypeByName("float");
			//Log::Warn("{}", R->getName());
			//auto* Rs = ProgramLayout->findFunctionByName("Bar");
			//Log::Warn("{}", Rs->getName());
			//slang::VariableLayoutReflection* GVarLayout = ProgramLayout->getGlobalParamsVarLayout();
			//UInt32 GParams = GVarLayout->getCategoryCount();
			//for (UInt32 Idx = 0; Idx < GParams; ++Idx)
			//{
			//	auto Cate = GVarLayout->getCategoryByIndex(Idx);
			//	/*auto* Var = Cate;
			//	auto* VarType = GVarLayout->getType();*/
			//	//Log::Info("(L) Name: {} | Type: {}", Var->getName(), VarType->getName());
			//}

			//UInt32 Params = ProgramLayout->getParameterCount();
			//for (UInt32 Idx = 0; Idx < Params; ++Idx)
			//{
			//	auto* VarLayout = ProgramLayout->getParameterByIndex(Idx);
			//	auto* Var = VarLayout->getVariable();
			//	auto* VarType = VarLayout->getType();
			//	Log::Info("(L) Name: {} | Type: {}", Var->getName(), VarType->getName());
			//}
			////slang::ProgramLayout* programLayout = program->getLayout(targetIndex);
			////auto Set2 = GlobalDescriptorPool.CreateDescriptorSet(SetLayout);
			////if (Set2 == nullptr) { Log::Fatal("Failed to allocate set2"); }

	}

} } // namespace VE::Runtime
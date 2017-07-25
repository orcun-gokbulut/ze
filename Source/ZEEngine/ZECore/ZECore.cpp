//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECore.cpp
 ------------------------------------------------------------------------------
 Copyright (C) 2008-2021 Yiğit Orçun GÖKBULUT. All rights reserved.

 This file is part of the Zinek Engine  Software. Zinek Engine Software and the
 accompanying  materials are  made available  under the  terms of Zinek  Engine
 Commercial License or the GNU General Public License Version 3.

                      ZINEK ENGINE COMMERCIAL LICENSE
 Licensees  holding  valid  Zinek Engine Commercial  License(s) may  use  Zinek
 Engine  Software in  accordance  with   the  commercial  license  agreement(s)
 (which can only be  issued  by  copyright  owner "Yiğit  Orçun  GÖKBULUT") and
 provided with the Software  or, alternatively,  in  accordance with the  terms
 contained  in  a  written  agreement  between  you  and  copyright  owner. For
 licensing  terms  and conditions  please  contact  with  copyright owner.

                    GNU GENERAL PUBLIC LICENSE VERSION 3
 This program is free software: you can  redistribute it and/or modify it under
 the terms of the GNU General Public  License as published by the Free Software
 Foundation, either  version 3 of  the License, or  (at your option)  any later
 version. This program is  distributed in the hope that  it will be useful, but
 WITHOUT ANY WARRANTY; without even the  implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. See  the GNU General Public License for more
 details. You  should have received  a copy of the  GNU General  Public License
 along with this program. If not, see <https://www.gnu.org/licenses/>.

 Copyright Owner Information:
  Name: Yiğit Orçun GÖKBULUT
  Contact: orcun.gokbulut@gmail.com
  Github: https://www.github.com/orcun-gokbulut/ZE
*******************************************************************************/
//ZE_SOURCE_PROCESSOR_END()

#include "ZECore.h"

#include "ZEMeta/ZEMTProvider.h"
#include "ZEMeta/ZEClass.h"
#include "ZEErrorManager.h"
#include "ZEModule.h"
#include "ZEConsole.h"
#include "ZEConsoleWindow.h"
#include "ZEOptionManager.h"
#include "ZECommandManager.h"
#include "ZESystemMessageManager.h"
#include "ZESystemMessageHandler.h"
#include "ZEProfiler.h"
#include "ZECrashHandler.h"
#include "ZESplashWindow.h"
#include "ZEResourceManager.h"
#include "ZEML/ZEMLReader.h"
#include "ZEML/ZEMLWriter.h"
#include "ZEPlugin.h"
#include "ZEThread/ZEThread.h"
#include "ZEFile/ZEDirectoryInfo.h"
#include "ZEFile/ZEFileInfo.h"
#include "ZEDS/ZEFormat.h"
#include "ZEFoundation.h"
#include "ZEEngine.h"
#include "ZEError.h"

#define ZE_CORE_MODULE_INCLUDES
#include "ZECoreModules.h"
#undef ZE_CORE_MODULE_INCLUDES

ZEOptionSection ZECore::CoreOptions; 
static HINSTANCE ApplicationInstance;

static ZEInt StartedCoreInstanceCount = 0;

void ZEError_Callback(ZEErrorType Type)
{
	if (Type < ZE_ET_CRITICAL_ERROR)
		return;

	ZECore* Core = ZECore::GetInstance();
	if (Core->GetCrashHandler() != NULL)
		Core->GetCrashHandler()->Crashed(ZE_CR_CRITICIAL_ERROR);

	TerminateProcess(GetCurrentProcess(), EXIT_FAILURE);
}

static void ZECore_AtExit()
{
	if (StartedCoreInstanceCount != 0)
		zeCriticalError("Unhandled exit function call detected. You cannot exit until core is shutdown. Terminating the core. This can cause huge problems.");

	TerminateProcess(GetCurrentProcess(), EXIT_FAILURE);
}

class ZECoreSystemMessageHandler : public ZESystemMessageHandler
{
	public:
		bool Callback(MSG* Message);
};

bool ZECoreSystemMessageHandler::Callback(MSG* Message)
{
	switch (Message->message)
	{
		case WM_QUIT:
			ZECore::GetInstance()->ShutDown();
			return true;

		default:
			return false;
	}
}

ZEPlugin* ZECore::LoadPlugin(const ZEString& Path)
{
	ZEFileInfo FileInfo(Path);
	FileInfo.SetPath(FileInfo.GetRealPath().Path);

	#ifdef ZE_DEBUG_ENABLE
		FileInfo.SetPath(ZEFormat::Format("{0}/{1}-Debug{2}", FileInfo.GetParentDirectory(), FileInfo.GetName(), FileInfo.GetExtension()));
	#endif

	zeLog("Loading plugin. Plugin Path: \"%s\".", FileInfo.Normalize().ToCString());

	SetDllDirectory(FileInfo.GetParentDirectory());
	SetDllDirectory(ZEFormat::Format("{0}\\Dependencies", FileInfo.GetParentDirectory()));
	SetDllDirectory(ZEFormat::Format("{0}\\Dependencies\\Common", FileInfo.GetParentDirectory()));
	SetDllDirectory(ZEFormat::Format("{0}\\Dependencies\\{1}", FileInfo.GetParentDirectory(), FileInfo.GetName()));
	HMODULE Module = LoadLibrary(FileInfo.GetRealPath().Path);
	SetDllDirectory(NULL);

	if (Module == NULL)
	{
		zeError("Cannot load plugin. Plugin file is not a plugin. Plugin Path: \"%s\".", FileInfo.Normalize().ToCString());
		return NULL;
	}

	ZECreatePluginInstance CreatePluginInstance = reinterpret_cast<ZECreatePluginInstance>(GetProcAddress(Module, "zeCreatePluginInstance"));
	if (CreatePluginInstance == NULL)
	{
		zeError("Cannot load plugin. Plugin does not have zeCreatePluginInstance() procedure. Plugin Path: \"%s\".", FileInfo.Normalize().ToCString());
		return NULL;
	}

	ZEPlugin* Plugin = CreatePluginInstance();
	if (Plugin == NULL)
	{
		zeError("Cannot load plugin. zeCreatePluginInstance() returned NULL. Plugin Path: \"%s\".", FileInfo.Normalize().ToCString());
		return NULL;
	}

	ZEVersion EngineVersion = ZEVersion::GetZinekVersion();
	ZEVersion PluginEngineVersion = Plugin->GetEngineVersion();
	if (PluginEngineVersion.Major != EngineVersion.Major ||
		PluginEngineVersion.Minor != EngineVersion.Minor)
	{
		zeError("Cannot load plugin. Plugin's engine version does not match with this engine version. "
			"Plugin Path: \"%s\", "
			"Plugin's Engine Version: %d.%d.", FileInfo.Normalize(), PluginEngineVersion.Major, PluginEngineVersion.Minor);

		Plugin->Destroy();
		return NULL;
	}

	#undef RegisterClass
	ZESize DeclarationCount = Plugin->GetDeclarationCount();
	ZEMTDeclaration* const* Declarations = Plugin->GetDeclarations();
	for (ZESize I = 0; I < DeclarationCount; I++)
		ZEMTProvider::GetInstance()->RegisterDeclaration(Declarations[I]);

	zeLog("Plugin loaded. Plugin Name: \"%s\". Plugin Version: \"%s\".", Plugin->GetName(), Plugin->GetVersion().GetShortString().ToCString());

	return Plugin;
}

bool ZECore::UnloadPlugin(ZEPlugin* Plugin)
{
	zeCheckError(Plugin == NULL, false, "Cannot unload plugin. Plugin is NULL.");
	zeCheckError(Plugin->CoreLink.GetInUse(), false, "Cannot unload plugin. Plugin is not added to core. Plugin Name: \"%s\".", Plugin->GetName());
	
	if (State != ZE_CS_NONE)
		zeWarning("Unloading plugin from alive core. This is dangerous operation. Plugin Name: \"%s\".", Plugin->GetName());
	
	Plugins.LockWrite();
	{
		ZESize DeclarationCount = Plugin->GetDeclarationCount();
		ZEMTDeclaration* const* Declarations = Plugin->GetDeclarations();
		for (ZESize I = 0; I < DeclarationCount; I++)
			ZEMTProvider::GetInstance()->UnregisterDeclaration(Declarations[I]);

		Plugins.Remove(&Plugin->CoreLink);

		HMODULE Module = (HMODULE)Plugin->GetData();
		Plugin->Destroy();
		FreeLibrary(Module);
	}
	Plugins.UnlockWrite();
}

bool ZECore::LoadInternalPlugin(ZEPlugin* Plugin)
{
	zeCheckError(Plugin == NULL, false, "Cannot add plugin. Plugin is NULL.");
	zeCheckError(Plugin->CoreLink.GetInUse(), false, "Cannot add plugin. Plugin is already added to a core. Plugin Name: \"%s\".", Plugin->GetName());

	ZEVersion EngineVersion = ZEVersion::GetZinekVersion();
	ZEVersion PluginEngineVersion = Plugin->GetEngineVersion();
	if (PluginEngineVersion.Major != EngineVersion.Major ||
		PluginEngineVersion.Minor != EngineVersion.Minor)
	{
		zeError("Cannot add plugin. Plugin's engine version does not match with this engine version. "
			"Plugin Name: \"%s\", "
			"Plugin's Engine Version: %d.%d.", Plugin->GetName(), PluginEngineVersion.Major, PluginEngineVersion.Minor);

		Plugin->Destroy();
		return NULL;
	}

	Plugins.AddEnd(&Plugin->CoreLink);

	#undef RegisterClass
	ZESize DeclarationCount = Plugin->GetDeclarationCount();
	ZEMTDeclaration* const* Declarations = Plugin->GetDeclarations();
	for (ZESize I = 0; I < DeclarationCount; I++)
		ZEMTProvider::GetInstance()->RegisterDeclaration(Declarations[I]);

	zeLog("Plugin added. Plugin Name: \"%s\". Plugin Version: \"%s\".", Plugin->GetName(), Plugin->GetVersion().GetShortString().ToCString());
}

bool ZECore::InitializeModule(ZEModule* Module, bool AutoInitializeDependencies)
{
	zeCheckError(Module == NULL, false, "Cannot initialize module. Module is NULL.");

	if (Module->IsInitialized())
		return true;

	zeLog("Initializing Module. Module Name: \"%s\".", Module->GetClass()->GetName());
	
	zeLog("Setting up module dependencies. Module Name: \"%s\".", Module->GetClass()->GetName());
	
	if (!Module->SetupDependence())
	{
		zeError("Cannot initialize module. Cannot setup dependencies. Module Name: \"%s\".", Module->GetClass()->GetName());
		return false;
	}

	bool UnitializedDependency = false;
	const ZEArray<ZEModule*>& Dependencies = Module->GetDependencyModules();
	for (ZESize I = 0; I < Dependencies.GetCount(); I++)
	{
		ZEModule* Dependency = Dependencies[I];
		if (!Dependency->IsInitialized())
		{
			zeLog("Uninitialized dependency module found. Module Name: \"%s\", Dependency Module Name: \"%s\".", Module->GetClass()->GetName(), Dependency->GetClass()->GetName());
			
			if (AutoInitializeDependencies)
			{
				zeLog("Initializing dependency module. Module Name: \"%s\", Dependency Module Name: \"%s\".", Module->GetClass()->GetName(), Dependency->GetClass()->GetName());
				if (!InitializeModule(Dependency, true))
				{
					zeError("Module initialization failed. Failed initializing dependency module. Module Name: \"%s\", Dependency Module Name: \"%s\".", Module->GetClass()->GetName(), Dependency->GetClass()->GetName());
					return false;
				}
			}
			else
			{
				UnitializedDependency = true;
			}
		}

		if (UnitializedDependency)
			zeError("Cannot initialize module. Uninitialized dependency modules found. Module Name: \"%s\".", Module->GetClass()->GetName());
	}

	Module->Initialize();

	if (!Module->IsInitialized())
	{
		zeError("Cannot initialize module. Initialization has failed. Module Name: \"%s\".", Module->GetClass()->GetName());
		return false;
	}

	return true;
}

bool ZECore::DeinitializeModule(ZEModule* Module, bool AutoDeinitializeDependents)
{
	zeCheckError(Module == NULL, false, "Cannot initialize module. Module is NULL.");

	if (Module->IsInitialized())
		return true;

	zeLog("Deinitializing Module. Module Name: \"%s\".", Module->GetClass()->GetName());

	bool InitializedDependent = false;
	const ZEArray<ZEModule*>& Dependents = Module->GetDependentModules();
	for (ZESize I = 0; I < Dependents.GetCount(); I++)
	{
		ZEModule* Dependent = Dependents[I];
		if (Dependent->IsInitialized())
		{
			zeLog("Initialized dependent module found. Module Name: \"%s\", Dependent Module Name: \"%s\".", Module->GetClass()->GetName(), Dependent->GetClass()->GetName());

			if (AutoDeinitializeDependents)
			{
				zeLog("Deinitializing dependent module. Module Name: \"%s\", Dependent Module Name: \"%s\".", Module->GetClass()->GetName(), Dependent->GetClass()->GetName());
				if (!DeinitializeModule(Dependent, true))
				{
					zeError("Module deinitialization failed. Failed deinitializing dependent module. Module Name: \"%s\", Dependent Module Name: \"%s\".", Module->GetClass()->GetName(), Dependent->GetClass()->GetName());
					return false;
				}
			}
			else
			{
				InitializedDependent = true;
			}
		}

		if (InitializedDependent)
			zeError("Cannot deinitialize module. Initialized dependent found. Module Name: \"%s\".", Module->GetClass()->GetName());
	}

	Module->Deinitialize();

	if (Module->IsInitialized())
	{
		zeError("Cannot deinitialize module. Deinitialization has failed. Module Name: \"%s\".", Module->GetClass()->GetName());
		return false;
	}

	return true;
}

void ZECore::InitializeModules()
{
	for (ZESize I = 0; I < Modules.GetCount(); I++)
	{
		ZEModule* Module = Modules[I];
		if (Module->IsInitialized())
			continue;

		if (!InitializeModule(Module, true))
			zeCriticalError("Cannot initialize modules. Module initialization failed. Module Name: \"%s\".", Module->GetClass()->GetName());
	}

}

void ZECore::DeinitializeModules()
{
	for (ZESize I = 0; I < Modules.GetCount(); I++)
	{
		ZEModule* Module = Modules[I];
		if (!Module->IsInitialized())
			continue;

		if (!DeinitializeModule(Module, true))
			zeCriticalError("Cannot deinitialize modules. Module deinitialization failed. Module Name: \"%s\".", Module->GetClass()->GetName());
	}
}

void ZECore::ShutdownInternal()
{
	DeinitializeModules();

	if (GetApplicationModule() != NULL)
		GetApplicationModule()->PreShutdown();

	Console->Deinitialize();
	CrashHandler->Deinitialize();

	ZEEngine_UnregisterDeclarations();
	ZEFoundation_UnregisterDeclarations();

	StartedCoreInstanceCount--;

	ZEError::GetInstance()->SetCallback(OldErrorCallback);

	State = ZE_CS_NONE;
}

ZECore::ZECore() 
{
	ZEFoundation_RegisterDeclarations();
	ZEEngine_RegisterDeclarations();

	ConfigurationPath = "#E:/Configurations/ZECore.ZEConfig";

	SplashWindow			= ZESplashWindow::CreateInstance();
	CrashHandler			= new ZECrashHandler();
	Profiler				= new ZEProfiler();
	SystemMessageManager	= new ZESystemMessageManager();
	SystemMessageHandler	= new ZECoreSystemMessageHandler();
	Console					= new ZEConsole();
	CommandManager			= new ZECommandManager();
	OptionManager			= new ZEOptionManager();
	ErrorManager			= new ZEErrorManager();
	ResourceManager			= new ZEResourceManager();

	SystemMessageManager->RegisterMessageHandler(SystemMessageHandler);

	#define ZE_CORE_MODULE(Type, Variable) Variable = NULL;
	#include "ZECoreModules.h"
	#undef ZE_CORE_MODULE
	
	CoreThreadID = NULL;
	
	static ZEOptionSection CoreOptions;
	CoreOptions.SetName("ZECore");
	CoreOptions.AddOption(new ZEOption("ZEGRGraphicsModule", "ZED11Module", ZE_OA_NORMAL));
	CoreOptions.AddOption(new ZEOption("ZEInputModule", "ZEWindowsInputModule", ZE_OA_NORMAL));
	CoreOptions.AddOption(new ZEOption("ZESoundModule", "ZEDSModule", ZE_OA_NORMAL));
	CoreOptions.AddOption(new ZEOption("ZENetworkModule", "ZEWinNetwork", ZE_OA_NORMAL));
	CoreOptions.AddOption(new ZEOption("ZEPhysicsModule", "ZEPhysXModule", ZE_OA_NORMAL));
	CoreOptions.AddOption(new ZEOption("ZEGameModule", "ZETestGame", ZE_OA_NORMAL));
	ZEOptionManager::GetInstance()->RegisterSection(&CoreOptions);
}

ZECore::~ZECore()
{
	if (State != ZE_CS_NONE)
		zeCriticalError("Cannot deconstruct the core. Core is still starting, running or shutting down.");

	while (Modules.GetFirstItem() != NULL)
	{
		ZEModule* Module = Modules.GetFirstItem();
		RemoveModule(Module);
		Module->Destroy();
	}

	while (Plugins.GetFirstItem() != NULL)
	{
		ZEPlugin* Plugin = Plugins.GetFirstItem();
		UnloadPlugin(Plugin);
	}

	delete ResourceManager;
	delete ErrorManager;
	delete OptionManager;
	delete CommandManager;
	delete Console;
	delete Profiler;
	SystemMessageManager->UnregisterMessageHandler(SystemMessageHandler);
	delete SystemMessageHandler;
	delete SystemMessageManager;
	delete CrashHandler;

	ZEEngine_UnregisterDeclarations();
	ZEFoundation_UnregisterDeclarations();
}

ZEErrorManager* ZECore::GetError()
{
	return ErrorManager;
}

ZEOptionManager* ZECore::GetOptions()
{
	return OptionManager;
}

ZEResourceManager* ZECore::GetResourceManager()
{
	return ResourceManager;
}

ZECommandManager* ZECore::GetCommands()
{
	return CommandManager;
}

ZEConsole* ZECore::GetConsole()
{
	return Console;
}

ZESystemMessageManager* ZECore::GetSystemMessageManager()
{
	return SystemMessageManager;
}

ZEProfiler* ZECore::GetProfiler()
{
	return Profiler;
}

ZECrashHandler* ZECore::GetCrashHandler()
{
	return CrashHandler;
}

void* ZECore::GetApplicationInstance()
{
	return ApplicationInstance;
}

ZEInt ZECore::GetCoreThreadId()
{
	return CoreThreadID;
}

void ZECore::SetConfigurationPath(const ZEString& Path)
{
	ConfigurationPath = Path;
}

const ZEString& ZECore::GetConfigurationPath() const
{
	return ConfigurationPath;
}

ZECoreState ZECore::GetState()
{
	return State;
}

bool ZECore::IsAlive()
{
	return (State == ZE_CS_RUNNING || State == ZE_CS_STARTING_UP || State == ZE_CS_SHUTTING_DOWN);
}

bool ZECore::IsRunning()
{
	return State == ZE_CS_RUNNING;
}

ZEModule* ZECore::GetModule(ZEClass* Class) const
{
	ZEModule* Output = NULL;
	Modules.LockRead();
	{
		ze_for_each(Module, Modules)
		{
			if (ZEClass::IsDerivedFrom(Class, Module->GetClass()))
			{
				Output = Module.GetPointer();
				break;
			}
		}
	}
	Modules.UnlockRead();

	return Output;
}

const ZEList2<ZEModule>& ZECore::GetModules() const
{
	return Modules;
}

bool ZECore::AddModule(ZEModule* Module)
{
	zeCheckError(Module == NULL, false, "Cannot add module. Module is NULL.");
	zeCheckError(Module->Core == this, false, "Cannot add module. Module is already registered. Module Name: \"%s\".", Module->GetClass()->GetName());

	Modules.LockWriteNested();
	{
		ze_for_each(CurrentModule, Modules)
		{
			// Check MultiInstance
			if (Module->GetClass()->CheckAttributeHasValue("ZEModule.MultiInstance", "false"))
			{
				if (ZEClass::IsDerivedFrom(CurrentModule->GetClass(), Module))
				{
					zeError(
						"Cannot add module. There is an already added module with same/derived class that does not allow MultiInstance."
						"Already Registred Module Name: \"%s\", Module Name: \"%s\".\n",
						CurrentModule->GetClass()->GetName(), Module->GetClass()->GetName());

					Modules.UnlockWrite();
					return false;
				}
			}
		}

		#define ZE_CORE_MODULE(Type, Variable) if (ZEClass::IsDerivedFrom(Type::Class(), Module)) Variable = static_cast<Type*>(Module);
		#include "ZECoreModules.h"
		#undef ZE_CORE_MODULE

		Module->Core = this;
		Modules.AddEnd(&Module->CoreLink);
		
		if (State == ZE_CS_STARTING_UP || State == ZE_CS_RUNNING)
		{
			if (Module->CheckUninitializedDependency())
				Module->Initialize();
		}
	}
	Modules.UnlockWrite();

	return true;
}

bool ZECore::RemoveModule(ZEModule* Module)
{
	zeCheckError(Module == NULL, false, "Module is NULL.");
	zeCheckError(Module->Core == this, false, "Module is already registered. Module Name: \"%s\".", Module->GetClass()->GetName());
	zeCheckError(Module->CheckInitializedDependent(), false, "Cannot remove module. There are modules that depends on this module.");
	
	if (IsAlive())
		zeWarning("Removing a module from core while is already alive. Not a good idea ! Module Name: \"%s\".", Module->GetClass()->GetName());

	Modules.LockWrite();
	{
		#define ZE_CORE_MODULE(Type, Variable) if (Variable == NULL) Variable = NULL;
		#include "ZECoreModules.h"
		#undef  ZE_CORE_MODULE

		Module->Deinitialize();
		Module->Core = NULL;
		Modules.Remove(&Module->CoreLink);
	}
	Modules.UnlockWrite();

	return true;
}

void ZECore::StartUp()
{
	if (State != ZE_CS_NONE)
		return;

	CoreThreadID = ZEThread::GetCurrentThreadId();

	State = ZE_CS_STARTING_UP;

	OldErrorCallback = ZEError::GetInstance()->GetCallback();
	ZEError::GetInstance()->SetCallback(ZEErrorCallback::Create<&ZEError_Callback>());

	atexit(ZECore_AtExit);
	
	StartedCoreInstanceCount++;

	ApplicationInstance = GetModuleHandle(NULL);

	ZESplashWindow* SplashWindow = ZESplashWindow::CreateInstance();
	SplashWindow->Show();

	zeLog("Loading ZECore configuration.");
	LoadConfiguration();

	if (GetApplicationModule() != NULL)
		GetApplicationModule()->PreStartup();

	Console->Initialize();
	Console->DisableInput();
	
	zeLog("Zinek Engine %s.", ZEVersion::GetZinekVersion().GetLongString().ToCString());
	zeLog("Initializing core...");
	
	if (ApplicationModule != NULL)
		ApplicationModule->StartUp();

	InitializeModules();

	Console->EnableInput();

	if (GetApplicationModule() != NULL)
		GetApplicationModule()->PostStartup();

	zeLog("Core initialized.");

	SplashWindow->Destroy();
	SplashWindow = NULL;

	State = ZE_CS_RUNNING;
}

void ZECore::ShutDown()
{
	zeLog("Shuting Down Core.");
	State = ZE_CS_SHUTTING_DOWN;
}

void ZECore::Terminate()
{
	ShutdownInternal();
	exit(0);
}

void ZECore::Process()
{
	ZETimeParameters Parameters;
	if (GetTimeManager() != NULL)
		Parameters = *GetTimeManager()->GetParameters();

	GetConsole()->Process();
	SystemMessageManager->ProcessMessages();

	/*Modules.LockRead();
	{
		if (State == ZE_CS_STARTING_UP || State == ZE_CS_RUNNING)
		{
			bool AllModulesInitialized = true;
			ze_for_each(Module, Modules)
			{
				if (!Module->IsInitializedOrInitializing() && Module->GetInitializationState() != ZE_IS_ERROR_INITIALIZATION)
				{
					AllModulesInitialized = false;
					if (Module->CheckUninitializedDependency())
						Module->Initialize();
				}
			}

			if (AllModulesInitialized)
			{
				if (State == ZE_CS_STARTING_UP)
					StartUpCompleted();

				State = ZE_CS_RUNNING;
			}
		}
		else if (State == ZE_CS_SHUTTING_DOWN)
		{
			bool AllModuleDeinitialized = true;
			ze_for_each(Module, Modules)
			{
				if (Module->IsInitializedOrInitializing())
				{
					AllModuleDeinitialized = false;
					if (Module->CheckInitializedDependent())
						Module->Deinitialize();
				}
			}
		}
	}
	Modules.UnlockRead();*/

	if (State == ZE_CS_SHUTTING_DOWN)
	{

	}
	ze_for_each(Module, Modules)
	{
		if (Module->IsInitialized())
			Module->PreProcess(&Parameters);
	}

	ze_for_each(Module, Modules)
	{
		if (Module->IsInitialized())
			Module->Process(&Parameters);
	}

	ze_for_each(Module, Modules)
	{
		if (Module->IsInitialized())
			Module->PostProcess(&Parameters);
	}
}

bool ZECore::Execute()
{
	StartUp();

	while (State != ZE_CS_NONE)
		Process();

	return true;
}

bool ZECore::LoadConfiguration()
{
	return LoadConfiguration(ConfigurationPath);
}

bool ZECore::LoadConfiguration(const ZEString& FileName)
{
	zeLog("Loading Core Configuration. Configration File Name: \"%s\"", FileName.ToCString());

	ConfigurationPath = FileName;

	ZEMLReader Reader;
	if (!Reader.Open(FileName))
	{
		zeError("Cannot load configuration. Cannot open configuration file. Configuration File Name: \"#E:/Configurations/ZECore.ZEConfig\".");
		return false;
	}

	ZEMLReaderNode RootNode = Reader.GetRootNode();
	if (!RootNode.IsValid() || RootNode.GetName() != "ZEConfig")
	{
		zeError("Cannot load configuration. Invalid configuration file root node. Configuration File Name: \"#E:/Configurations/ZECore.ZEConfig\".");
		return false;
	}

	ZEString Class = RootNode.ReadString("Class");
	if (Class != GetClass()->GetName())
	{
		zeError("Cannot load configuration. Configuration file class is different then ZECore class. Configuration File Name: \"#E:/Configurations/ZECore.ZEConfig\".");
		return false;
	}

	ZEMLReaderNode PluginsNode = RootNode.GetNode("Plugins");
	ZESize PluginCount = PluginsNode.GetNodeCount("Plugin");
	for (ZESize I = 0; I < PluginCount; I++)
	{
		ZEMLReaderNode PluginNode = PluginsNode.GetNode("Plugin", I);
		ZEString FileName = PluginNode.ReadString("FileName");
		ZEPlugin* Plugin = LoadPlugin(FileName);
		if (Plugin == NULL)
		{
			zeError("Cannot load configuration. Cannot load plugin. File Name: \"%s\".", FileName.ToCString());
			continue;
		}

		LoadInternalPlugin(Plugin);
	}

	ZEMLReaderNode ModulesNode = RootNode.GetNode("Modules");
	ZESize ModuleCount = ModulesNode.GetNodeCount("Module");
	for (ZESize I = 0; I < ModuleCount; I++)
	{
		ZEMLReaderNode ModuleNode = ModulesNode.GetNode("Module", I);
		ZEString ClassName = ModuleNode.ReadString("Class");
		ZEClass* ModuleClass = ZEMTProvider::GetInstance()->GetClass(ZEModule::Class(), ClassName);
		if (ModuleClass == NULL)
		{
			zeError("Cannot load configuration. Cannot find module class. Class Name: \"%s\".", ClassName.ToCString());
			continue;
		}

		AddModule(static_cast<ZEModule*>(ModuleClass->CreateInstance()));
	}

	return true;
}

bool ZECore::SaveConfiguration()
{
	return SaveConfiguration(ConfigurationPath);
}

bool ZECore::SaveConfiguration(const ZEString& FileName)
{
	zeLog("Saving Core Configuration. Configration File Name: \"%s\"", FileName.ToCString());

	ConfigurationPath = FileName;

	ZEMLWriter Writer;
	Writer.SetFormat(ZEMLFormat::GetDefaultTextFormat()->CreateInstance());

	if (!Writer.Open(FileName))
	{
		zeError("Cannot save configuration. Cannot open configuration file. Configuration File Name: \"#E:/Configurations/ZECore.ZEConfig\".");
		return false;
	}

	ZEMLWriterNode RootNode;
	Writer.OpenRootNode("ZEConfig", RootNode);
	RootNode.WriteString("Class", GetClass()->GetName());

	ZEMLWriterNode ModulesNode;
	RootNode.OpenNode("Modules", ModulesNode);
	for (ZESize I = 0; I < Modules.GetCount(); I++)
	{
		ZEMLWriterNode ModuleNode;
		ModulesNode.OpenNode("Module", ModuleNode);
		ModuleNode.WriteString("Class", Modules[I]->GetClass()->GetName());
		ModuleNode.CloseNode();
	}
	ModulesNode.CloseNode();
	RootNode.CloseNode();
	Writer.Close();

	return true;
}

ZECore* ZECore::GetInstance()
{	
	static ZECore Engine;
	return &Engine;
}

#define ZE_CORE_MODULE(Type, Variable) Type* ZECore::Get##Variable() const {return Variable;}
#include "ZECoreModules.h"
#undef ZE_CORE_MODULE

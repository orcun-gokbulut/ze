//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModule.cpp
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

#include "ZEModule.h"

#include "ZEDS/ZEFormat.h"
#include "ZEFile/ZEPathInfo.h"
#include "ZEML/ZEMLWriter.h"
#include "ZEML/ZEMLReader.h"
#include "ZECore.h"
#include "ZEApplicationModule.h"
#include "ZEMeta/ZEMTProvider.h"

void ZEModule::FindConfigurationFileName()
{
	const ZEUInt PathPatternCount = 20;
	const char* PathPatterns[PathPatternCount] = 
	{
		"#S:/Configurations/{1}/{0}/{0}.ZEConfig",
		"#S:/Configurations/{1}/{0}.ZEConfig",
		"#S:/Configurations/{0}/{0}.ZEConfig",
		"#S:/Configurations/{0}.ZEConfig",
		"#S:/Configurations/{1}/{2}/{2}.ZEConfig",
		"#S:/Configurations/{1}/{2}.ZEConfig",
		"#S:/Configurations/{2}/{2}.ZEConfig",
		"#S:/Configurations/{2}.ZEConfig",

		"#R:/Configurations/{1}/{0}/{0}.ZEConfig",
		"#R:/Configurations/{1}/{0}.ZEConfig",
		"#R:/Configurations/{0}/{0}.ZEConfig",
		"#R:/Configurations/{0}.ZEConfig",
		"#R:/Configurations/{1}/{2}/{2}.ZEConfig",
		"#R:/Configurations/{1}/{2}.ZEConfig",
		"#R:/Configurations/{2}/{2}.ZEConfig",
		"#R:/Configurations/{2}.ZEConfig",

		"#E:/Configurations/{0}/{0}.ZEConfig",
		"#E:/Configurations/{0}.ZEConfig",
		"#E:/Configurations/{2}/{2}.ZEConfig",
		"#E:/Configurations/{2}.ZEConfig"
	};

	const char* ApplicationName = "";
	if (GetCore()->GetApplicationModule() != NULL)
		ApplicationName = GetCore()->GetApplicationModule()->GetName();

	for (ZEUInt I = 0; I < PathPatternCount; I++)
	{
		ZEPathInfo PathInfo(ZEFormat::Format(PathPatterns[I], GetClass()->GetName(), ApplicationName, GetBaseModule()->GetName()));
		if (PathInfo.IsExists())
		{
			ConfigurationFileName = PathInfo.GetPath();
			break;
		}
	}
}

void ZEModule::SetConfigurationFileName(const ZEString& FileName)
{
	ConfigurationFileName = FileName;
}

ZEInitializationResult ZEModule::InitializeInternal()
{
	ZE_INITIALIZABLE_INITIALIZE_CHAIN(ZEInitializableAsync);

	zeCheckError(Core == NULL, ZE_IR_FAILED, "Cannot initialize module. Module is not added to a core. Module Name: \"%s\".", GetClass()->GetName());

	LoadConfiguration();

	return ZE_IR_DONE;
}

ZEInitializationResult ZEModule::DeinitializeInternal()
{
	ZE_INITIALIZABLE_DEINITIALIZE_CHAIN(ZEInitializableAsync);

	return ZE_IR_DONE;
}

bool ZEModule::SetupDependence()
{
	BrokenDependency = false;

	const ZEMTAttribute* Attribute = GetClass()->GetAttribute("ZEModule.Dependencies");
	if (Attribute != NULL && Attribute->ValueCount != 0)
	{
		const ZEList2<ZEModule>& CoreModules = GetCore()->GetModules();
		CoreModules.LockRead();
		{
			for (ZESize I = 0; I < Attribute->ValueCount; I++)
			{
				const char* ClassName = Attribute->Values[I];
				ZEClass* Class = ZEMTProvider::GetInstance()->GetClass(ClassName);
				if (Class == NULL)
				{
					zeError("Cannot setup module dependence. Cannot find dependency module class. Module Name: \"%s\". Dependency Module Class: \"%s\".", 
						GetClass()->GetName(),
						ClassName);

					BrokenDependency = true;
					continue;
				}

				ZEModule* Module = GetCore()->GetModule(Class);
				if (Module == NULL)
				{
					zeError("Cannot setup module dependence. Dependency module is not added to core. Module Name: \"%s\". Dependency Module Name: \"%s\".", 
						GetClass()->GetName(),
						ClassName);

					BrokenDependency = true;
					continue;
				}

				DependencyModules.LockWrite();
				{
					if (!DependencyModules.Exists(Module))
					{
						Module->DependentModules.LockWrite();
						{
							if (Module->DependentModules.Exists(this))
								Module->DependentModules.Add(this);
						}
						Module->DependentModules.UnlockWrite();

						DependencyModules.Add(Module);
					}
				}
				DependencyModules.UnlockWrite();
			}
		}
		CoreModules.UnlockRead();
	}

	if (BrokenDependency)
		DependencyModules.Clear();

	return !BrokenDependency;
}

bool ZEModule::CheckUninitializedDependency()
{
	bool Result = true;
	DependencyModules.LockRead();
	{
		for (ZESize I = 0; I < DependencyModules.GetCount(); I++)
		{
			if (!DependencyModules[I]->IsInitialized())
			{
				Result = false;
				break;
			}
		}
	}
	DependencyModules.UnlockRead();

	return Result;
}

bool ZEModule::CheckInitializedDependent()
{
	bool Result = true;

	DependentModules.LockRead();
	{
		for (ZESize I = 0; I < DependentModules.GetCount(); I++)
		{
			if (DependentModules[I]->IsInitializedOrInitializing())
			{
				Result = false;
				break;
			}
		}
	}
	DependentModules.UnlockRead();

	return Result;
}

ZEModule::ZEModule() : CoreLink(this)
{
	Core = NULL;
	Enabled = true;
}

ZEModule::~ZEModule()
{
	if (IsInitializedOrInitializing())
		zeCriticalError("Cannot destruct module. Module is initialized or initializing. Module: \"%s\"", GetClass()->GetName());
	
	if (Core != NULL)
		zeCriticalError("Cannot destruct module. Module is still added to the core. Module: \"%s\"", GetClass()->GetName());
}

ZECore* ZEModule::GetCore() const
{
	return Core;
}

ZEClass* ZEModule::GetBaseModule() const
{
	if (GetClass()->GetParentClass() != NULL)
		return GetClass()->GetParentClass();
	else
		return GetClass();
}

const ZEString& ZEModule::GetConfigurationFileName() const
{
	return ConfigurationFileName;
}


const ZEArray<ZEModule*>& ZEModule::GetDependencyModules() const
{
	return DependencyModules;
}

const ZEArray<ZEModule*>& ZEModule::GetDependentModules() const
{
	return DependentModules;
}

void ZEModule::SetEnabled(bool Enabled)
{
	this->Enabled = Enabled;
}

bool ZEModule::GetEnabled() const
{
	return Enabled;
}

void ZEModule::PreProcess(const ZETimeParameters* Parameters)
{

}

void ZEModule::Process(const ZETimeParameters* Parameters)
{

}

void ZEModule::PostProcess(const ZETimeParameters* Parameters)
{

}

bool ZEModule::LoadConfiguration()
{
	if (ConfigurationFileName.IsEmpty())
		FindConfigurationFileName();

	if (ConfigurationFileName.IsEmpty())
		return false;

	return LoadConfiguration(ConfigurationFileName);
}

bool ZEModule::LoadConfiguration(const ZEString& FileName)
{
	zeLog("Loading Module Configuration.  Module Name: \"%s\", Configration File Name: \"%s\".", GetClass()->GetName(), FileName.ToCString());

	ConfigurationFileName = FileName;

	ZEMLReader Reader;
	if (!Reader.Open(ConfigurationFileName))
	{
		zeError("Cannot load configuration. Cannot open configuration file. Module Name: %s, Configuration File Name: %s.", GetClass()->GetName(), ConfigurationFileName.ToCString());
		return false;
	}

	ZEMLReaderNode RootNode = Reader.GetRootNode();
	if (RootNode.GetName() != "ZEConfig")
	{
		zeError("Cannot load configuration. Unknown root node name. Module Name: %s, Configuration File Name: %s.", GetClass()->GetName(), ConfigurationFileName.ToCString());
		return false;
	}

	this->ConfigurationFileName = ConfigurationFileName;

	return LoadConfiguration(RootNode);
}

bool ZEModule::LoadConfiguration(const ZEMLReaderNode& ConfigNode)
{
	return GetClass()->Unserialize(this, ConfigNode);
}

bool ZEModule::SaveConfiguration()
{
	if (ConfigurationFileName.IsEmpty())
		return false;

	return LoadConfiguration(ConfigurationFileName);
}

bool ZEModule::SaveConfiguration(const ZEString& FileName)
{
	zeLog("Saving Module Configuration. Module Name: \"%s\", Configration File Name: \"%s\".", GetClass()->GetName(), FileName.ToCString());

	ConfigurationFileName = FileName;

	ZEMLWriter Writer;
	if (!Writer.Open(ConfigurationFileName))
	{
		zeError("Cannot save configuration. Cannot open configuration file. Module Name: %s, Configuration File Name: %s.", GetClass()->GetName(), ConfigurationFileName.ToCString());
		return false;
	}

	ZEMLWriterNode RootNode;
	if (!Writer.OpenRootNode("ZEConfig", RootNode))
	{
		zeError("Cannot save configuration. Cannot write root node to configuration file. Module Name: %s, Configuration File Name: %s.", GetClass()->GetName(), ConfigurationFileName.ToCString());
		return false;
	}

	this->ConfigurationFileName = ConfigurationFileName;

	return SaveConfiguration(RootNode);
}

bool ZEModule::SaveConfiguration(ZEMLWriterNode& ConfigNode)
{
	return GetClass()->Serialize(this, ConfigNode);
}

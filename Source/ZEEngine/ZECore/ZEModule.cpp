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

void ZEModule::RegisterClasses()
{

}

void ZEModule::UnregisterClasses()
{

}

bool ZEModule::InitializeInternal()
{
	if (!ZEInitializable::InitializeInternal())
		return false;

	RegisterClasses();
	LoadConfiguration();

	return true;
}

bool ZEModule::DeinitializeInternal()
{
	UnregisterClasses();

	return ZEInitializable::DeinitializeInternal();
}

ZEModule::ZEModule() : CoreLink(this)
{
	Core = NULL;
	Enabled = true;
}

ZEModule::~ZEModule()
{
	Deinitialize();
	if (Core != NULL)
		Core->RemoveModule(this);
}

ZECore* ZEModule::GetCore() const
{
	return Core;
}

ZEClass* ZEModule::GetBaseModule() const
{
	return GetClass();
}

const ZEString& ZEModule::GetConfigurationFileName() const
{
	return ConfigurationFileName;
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

bool ZEModule::LoadConfiguration(const ZEString& ConfigurationFileName)
{
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

bool ZEModule::SaveConfiguration(const ZEString& ConfigNode)
{
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

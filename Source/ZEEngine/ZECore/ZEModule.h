//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModule.h
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

#pragma once

#include "ZEMeta/ZEObject.h"
#include "ZEInitializableAsync.h"
#include "ZEDestroyable.h"

#include "ZEDS/ZELink.h"
#include "ZEExport.ZEEngine.h"
#include "ZEMeta/ZEEvent.h"

class ZECore;
class ZETimeParameters;
class ZEMLReaderNode;
class ZEMLWriterNode;

class ZE_EXPORT_ZEENGINE ZEModule : public ZEObject, public ZEInitializableAsync, public ZEDestroyable
{
	ZE_OBJECT
	friend class ZECore;
	private:
		ZECore*							Core;
		ZELink<ZEModule>				CoreLink;
		bool							Enabled;
		ZEString						ConfigurationFileName;
		ZEArray<ZEModule*>				DependentModules;
		ZEArray<ZEModule*>				DependencyModules;
		bool							BrokenDependency;

		void							FindConfigurationFileName();

	protected:
		void							SetConfigurationFileName(const ZEString& FileName);

		virtual ZEInitializationResult	InitializeInternal() override;
		virtual ZEInitializationResult	DeinitializeInternal() override;

		bool							SetupDependence();
		bool							CheckUninitializedDependency();
		bool							CheckInitializedDependent();

										ZEModule();
		virtual							~ZEModule();

	public:
		ZECore*							GetCore() const;
		virtual ZEClass*				GetBaseModule() const;
		const ZEString&					GetConfigurationFileName() const;
		const ZEArray<ZEModule*>&		GetDependencyModules() const;
		const ZEArray<ZEModule*>&		GetDependentModules() const;

		virtual bool					GetEnabled() const;
		virtual void					SetEnabled(bool Enabled);

		virtual void					PreProcess(const ZETimeParameters* Parameters);
		virtual void					Process(const ZETimeParameters* Parameters);
		virtual void					PostProcess(const ZETimeParameters* Parameters);

		bool							LoadConfiguration();
		bool							LoadConfiguration(const ZEString& FileName);
		virtual bool					LoadConfiguration(const ZEMLReaderNode& ConfigNode);

		bool							SaveConfiguration();
		bool							SaveConfiguration(const ZEString& FileName);
		virtual bool					SaveConfiguration(ZEMLWriterNode& ConfigNode);

		ZE_EVENT(						OnInitializationDone,(ZEModule* Module));
		ZE_EVENT(						OnInitializationFailed,(ZEModule* Module));
		ZE_EVENT(						OnDeinitializationDone,(ZEModule* Module));
		ZE_EVENT(						OnDeinitializationFailed,(ZEModule* Module));
}
ZEMT_ATTRIBUTE(ZEModule.Dependencies)
ZEMT_ATTRIBUTE(ZEModule.MultiInstance, false)
ZEMT_ATTRIBUTE(ZEModule.Dependencies)
ZEMT_ATTRIBUTE(ZEModule.ProcessOrderBefore)
ZEMT_ATTRIBUTE(ZEModule.ProcessOrderAfter);

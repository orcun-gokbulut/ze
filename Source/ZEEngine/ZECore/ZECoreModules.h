//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECoreModules.h
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

/*
ZE_CORE_MODULE(ZECrashHandler,					CrashHandler)
ZE_CORE_MODULE(ZEErrorManager,					ErrorManager)
ZE_CORE_MODULE(ZEOptionManager,					OptionManager)
ZE_CORE_MODULE(ZECommandManager,				CommandManager)
ZE_CORE_MODULE(ZEResourceManager,				ResourceManager)
ZE_CORE_MODULE(ZESystemMessageManager,			SystemMessageManager)
ZE_CORE_MODULE(ZESystemMessageHandler,			SystemMessageHandler)	
ZE_CORE_MODULE(ZEConsole,						Console)
ZE_CORE_MODULE(ZEProfiler,						Profiler)
*/

#ifdef ZE_CORE_MODULE_INCLUDES
#include "ZETimeManager.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZEInput/ZEInputModule.h"
#include "ZEPhysics/ZEPhysicsModule.h"
#include "ZESound/ZESoundModule.h"
#include "ZEApplicationModule.h"
#else
ZE_CORE_MODULE(ZETimeManager,					TimeManager)
ZE_CORE_MODULE(ZEGRGraphicsModule,				GraphicsModule)
ZE_CORE_MODULE(ZESoundModule,					SoundModule)
ZE_CORE_MODULE(ZEInputModule,					InputModule)
ZE_CORE_MODULE(ZEPhysicsModule,					PhysicsModule)
ZE_CORE_MODULE(ZEApplicationModule,				ApplicationModule)
#endif

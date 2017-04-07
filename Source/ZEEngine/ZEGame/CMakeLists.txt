#ZE_SOURCE_PROCESSOR_START(License, 1.0)
#[[*****************************************************************************
 Zinek Engine - CMakeLists.txt
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
*****************************************************************************]]
#ZE_SOURCE_PROCESSOR_END()

cmake_minimum_required (VERSION 2.8)

ze_add_source(ZECanvasBrush.cpp			Sources)
ze_add_source(ZECanvasBrush.h			Sources Headers ZEMC)
ze_add_source(ZEEntity.cpp				Sources)
ze_add_source(ZEEntity.h				Sources Headers ZEMC)
ze_add_source(ZEEntityMacros.cpp		Sources)
ze_add_source(ZEEntityMacros.h			Sources Headers)
ze_add_source(ZEPlayer.cpp				Sources)
ze_add_source(ZEPlayer.h				Sources Headers ZEMC)
ze_add_source(ZERayCast.cpp				Sources)
ze_add_source(ZERayCast.h				Sources Headers ZEMC)
ze_add_source(ZEVolumeCast.cpp			Sources)
ze_add_source(ZEVolumeCast.h			Sources Headers)
ze_add_source(ZEScene.cpp				Sources)
ze_add_source(ZEScene.h					Sources Headers ZEMC)
ze_add_source(ZESea.cpp					Sources)
ze_add_source(ZESea.h					Sources Headers ZEMC)
ze_add_source(ZEGeographicEntity.cpp	Sources)
ze_add_source(ZEGeographicEntity.h		Sources Headers ZEMC)
ze_add_source(ZESector.cpp				Sources)
ze_add_source(ZESector.h				Sources Headers ZEMC)
ze_add_source(ZESectorManager.cpp		Sources)
ze_add_source(ZESectorManager.h			Sources Headers ZEMC)
ze_add_source(ZESectorSelector.cpp		Sources)
ze_add_source(ZESectorSelector.h		Sources Headers ZEMC)
ze_add_source(ZELoadingScreen.cpp		Sources)
ze_add_source(ZELoadingScreen.h			Sources Headers ZEMC)
ze_add_source(ZESpawner.cpp				Sources)
ze_add_source(ZESpawner.h				Sources Headers ZEMC)

ze_add_library(TARGET ZEGame 
	ZEMC ${ZEMC}
	SOURCES ${Sources}
	HEADERS ${Headers})

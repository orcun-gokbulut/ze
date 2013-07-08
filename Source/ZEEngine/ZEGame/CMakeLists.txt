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

#ze_add_source(ZECanvasBrush.cpp		Sources)
#ze_add_source(ZECanvasBrush.h			Sources Headers ZEPP)
ze_add_source(ZECharacter.cpp			Sources)
ze_add_source(ZECharacter.h				Sources Header ZEPP)
#ze_add_source(ZECloud.cpp				Sources)
#ze_add_source(ZECloud.h				Sources Header)
#ze_add_source(ZEDecal.cpp				Sources)
#ze_add_source(ZEDecal.h				Sources Headers)
ze_add_source(ZEDrawStatistics.cpp		Sources)
ze_add_source(ZEDrawStatistics.h		Sources Headers)
ze_add_source(ZEDrawParameters.cpp		Sources)
ze_add_source(ZEDrawParameters.h		Sources Headers)
ze_add_source(ZEEntity.cpp				Sources)
ze_add_source(ZEEntity.h				Sources Headers ZEPP)
ze_add_source(ZEEntityProvider.cpp		Sources)
ze_add_source(ZEEntityProvider.h		Sources Headers)
ze_add_source(ZEGame.cpp				Sources)
ze_add_source(ZEGame.h					Sources Headers)
ze_add_source(ZEGameProvider.cpp		Sources)
ze_add_source(ZEGameProvider.h			Sources Headers)
ze_add_source(ZEGrid.cpp				Sources)
ze_add_source(ZEGrid.h					Sources	Headers ZEPP)
ze_add_source(ZEGizmo.cpp	    		Sources)
ze_add_source(ZEGizmo.h					Sources	Headers)
#ze_add_source(ZEMoon.cpp	    		Sources)
#ze_add_source(ZEMoon.h					Sources	Headers)
ze_add_source(ZEOrbit.cpp				Sources)
ze_add_source(ZEOrbit.h					Sources Headers)
ze_add_source(ZEPlayer.cpp				Sources)
ze_add_source(ZEPlayer.h				Sources Headers ZEPP)
#ze_add_source(ZEPresentationSlide.cpp	Sources)
#ze_add_source(ZEPresentationSlide.h		Sources Header ZEPP)
ze_add_source(ZERayCast.cpp				Sources)
ze_add_source(ZERayCast.h				Sources Headers)
ze_add_source(ZEScene.cpp				Sources)
ze_add_source(ZEScene.h					Sources Headers)
ze_add_source(ZESceneDebugDraw.cpp		Sources)
ze_add_source(ZESceneDebugDraw.h		Sources Headers)
ze_add_source(ZESceneDebugDraw.cpp		Sources)
ze_add_source(ZESceneDebugDraw.h		Sources Headers)
ze_add_source(ZESceneCuller.cpp			Sources)
ze_add_source(ZESceneCuller.h			Sources Headers)
#ze_add_source(ZESkyBrush.cpp			Sources)
#ze_add_source(ZESkyBrush.h				Sources Header ZEPP)
#ze_add_source(ZESkyDome.cpp			Sources)
#ze_add_source(ZESkyDome.h				Sources Header)
ze_add_source(ZETrigger.cpp				Sources)
ze_add_source(ZETrigger.h				Sources Headers ZEPP)
#ze_add_source(ZEWeather.cpp			Sources)
#ze_add_source(ZEWeather.h				Sources Headers ZEPP)
#ze_add_source(ZESea.cpp				Sources)
#ze_add_source(ZESea.h					Sources Headers ZEPP)
#ze_add_source(ZEFoliage.cpp			Sources)
#ze_add_source(ZEFoliage.h				Sources Headers ZEPP)


ze_add_library(ZEGame 
	SOURCES ${Sources}
	HEADERS ${Headers})

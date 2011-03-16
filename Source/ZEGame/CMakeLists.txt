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

add_source (ZECanvasBrush.cpp		Sources)
add_source (ZECanvasBrush.h			Sources ZESDK ZEPP)
add_source (ZECharacter.cpp			Sources)
add_source (ZECharacter.h			Sources ZESDK ZEPP)
add_source (ZEComponent.cpp			Sources)
add_source (ZEComponent.h			Sources ZESDK ZEPP)
add_source (ZECompoundEntity.cpp	Sources)
add_source (ZECompoundEntity.h		Sources ZESDK ZEPP)
add_source (ZEDrawParameters.cpp	Sources)
add_source (ZEDrawParameters.h		Sources ZESDK)
add_source (ZEEntity.cpp			Sources)
add_source (ZEEntity.h				Sources ZESDK ZEPP)
add_source (ZEEntityProvider.cpp	Sources)
add_source (ZEEntityProvider.h		Sources ZESDK)
add_source (ZEGame.cpp				Sources)
add_source (ZEGame.h				Sources ZESDK)
add_source (ZEGameProvider.cpp		Sources)
add_source (ZEGameProvider.h		Sources ZESDK)
add_source (ZEGrid.cpp				Sources)
add_source (ZEGrid.h				Sources	ZESDK ZEPP)
add_source (ZEPlayer.cpp			Sources)
add_source (ZEPlayer.h				Sources ZESDK ZEPP)
add_source (ZEPortal.cpp			Sources)
add_source (ZEPortal.h				Sources ZESDK)
add_source (ZEPresentationSlide.cpp	Sources)
add_source (ZEPresentationSlide.h	Sources ZESDK ZEPP)
add_source (ZERayCaster.cpp			Sources)
add_source (ZERayCaster.h			Sources ZESDK)
add_source (ZEScene.cpp				Sources)
add_source (ZEScene.h				Sources ZESDK)
add_source (ZESceneDebugDraw.cpp	Sources)
add_source (ZESceneDebugDraw.h		Sources ZESDK)
add_source (ZESceneCuller.cpp		Sources)
add_source (ZESceneCuller.h			Sources ZESDK)
add_source (ZESerialization.cpp		Sources)
add_source (ZESerialization.h		Sources ZESDK)
add_source (ZESkyBrush.cpp			Sources)
add_source (ZESkyBrush.h			Sources ZESDK ZEPP)
add_source (ZETrigger.cpp			Sources)
add_source (ZETrigger.h				Sources ZESDK ZEPP)

add_library (ZEGame ${Sources})

set_property(TARGET ZEGame PROPERTY FOLDER "Zinek Engine")

install (TARGETS ZEGame COMPONENT "Libraries" CONFIGURATIONS Release DESTINATION Lib)
install (FILES ${ZESDK}  COMPONENT "Includes" CONFIGURATIONS Release DESTINATION Include/ZEGame)

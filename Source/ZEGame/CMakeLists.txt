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

cmake_minimum_required(VERSION 2.8)


add_source(ZECanvasBrush.cpp ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZECanvasBrush.h ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZECharacter.cpp ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZECharacter.h ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEComponent.cpp ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEComponent.h ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZECompoundEntity.cpp ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZECompoundEntity.h ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEDrawParameters.cpp ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEDrawParameters.h ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEEntity.cpp ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEEntity.h ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEEntityProvider.cpp ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEEntityProvider.h ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEGame.cpp ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEGame.h ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEGameManager.h ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEGameProvider.cpp ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEGameProvider.h ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEGrid.cpp ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEGrid.h ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEPlayer.cpp ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEPlayer.h ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEPortal.cpp ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEPortal.h ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEPresentationSlide.cpp ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEPresentationSlide.h ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEScene.cpp ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEScene.h ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZESceneDebugDraw.cpp ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZESceneDebugDraw.h ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZESerialization.cpp ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZESerialization.h ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZESkyBrush.cpp ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZESkyBrush.h ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZESoundBrush2D.cpp ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZESoundBrush2D.h ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZESoundBrush3D.cpp ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZESoundBrush3D.h ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZETrigger.cpp ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZETrigger.h ZEGame_SOURCE_FILES INCLUDED_SDKS zesdk)

add_library(ZEGame ${ZEGame_SOURCE_FILES})

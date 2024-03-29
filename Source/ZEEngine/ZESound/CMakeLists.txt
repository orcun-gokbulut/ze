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


ze_add_source(ZEListener.cpp				Sources)
ze_add_source(ZEListener.h					Sources Headers ZEMC)
ze_add_source(ZESoundFX.h					Sources Headers)
ze_add_source(ZESoundModule.cpp				Sources)
ze_add_source(ZESoundModule.h				Sources Headers ZEMC)
ze_add_source(ZESoundResource.cpp			Sources)
ze_add_source(ZESoundResource.h				Sources Headers ZEMC)
ze_add_source(ZESoundResourceMP3.cpp		Sources)
ze_add_source(ZESoundResourceMP3.h			Sources)
ze_add_source(ZESoundResourceOGG.cpp		Sources)
ze_add_source(ZESoundResourceOGG.h			Sources)
ze_add_source(ZESoundResourceWAV.cpp		Sources)
ze_add_source(ZESoundResourceWAV.h			Sources)
ze_add_source(ZESoundSource.cpp				Sources)
ze_add_source(ZESoundSource.h				Sources Headers ZEMC)

ze_add_library(TARGET ZESound
	SOURCES ${Sources}
	HEADERS ${Headers}
	ZEMC ${ZEMC}
	LIBS ZEFoundation libmpg123 libOGG libVorbis)

//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNShaderSlots.h
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

#define ZERN_SHADER_GBUFFER_NORMAL_MODEL		0
#define ZERN_SHADER_GBUFFER_DIFFUSE_SUBSURFACE	1
#define ZERN_SHADER_GBUFFER_SPECULAR			2
#define ZERN_SHADER_GBUFFER_ACCUMULATOR			3

#define ZERN_SHADER_CONSTANT_RENDERER			0
#define ZERN_SHADER_CONSTANT_SCENE				1
#define ZERN_SHADER_CONSTANT_VIEW				2
#define ZERN_SHADER_CONSTANT_STAGE				3
#define ZERN_SHADER_CONSTANT_MATERIAL			4
#define ZERN_SHADER_CONSTANT_POST_PROCESSOR		4
#define ZERN_SHADER_CONSTANT_DRAW_MATERIAL		5
#define ZERN_SHADER_CONSTANT_DRAW_TRANSFORM		6
#define ZERN_SHADER_CONSTANT_DRAW_SKIN			7
#define ZERN_SHADER_CONSTANT_DRAW_OTHER			8

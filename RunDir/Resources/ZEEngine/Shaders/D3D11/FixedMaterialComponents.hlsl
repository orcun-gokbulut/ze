//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - FixedMaterialComponents.hlsl
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

// Pipelien Shader Components
//#define ZE_SHADER_SKIN_TRANSFORM		(ZE_PIPELINE_COMPONENT & (1 << 1))
//#define ZE_SHADER_MORPH_TARGET			(ZE_PIPELINE_COMPONENT & (1 << 2))

// Custom Shader Components
/*
#define ZE_SHADER_AMBIENT 				ZE_SHADER_COMPONENTS & (1 << 1)
#define ZE_SHADER_DIFFUSE 				ZE_SHADER_COMPONENTS & (1 << 2)
#define ZE_SHADER_BASE_MAP 				ZE_SHADER_COMPONENTS & (1 << 3)
#define ZE_SHADER_NORMAL_MAP			ZE_SHADER_COMPONENTS & (1 << 4)
#define ZE_SHADER_PARALLAX_MAP			ZE_SHADER_COMPONENTS & (1 << 5)
#define ZE_SHADER_SPECULAR				ZE_SHADER_COMPONENTS & (1 << 6)
#define ZE_SHADER_SPECULAR_MAP			ZE_SHADER_COMPONENTS & (1 << 7)
#define ZE_SHADER_EMMISIVE				ZE_SHADER_COMPONENTS & (1 << 8)
#define ZE_SHADER_EMMISIVE_MAP			ZE_SHADER_COMPONENTS & (1 << 9)
#define ZE_SHADER_OPACITY				ZE_SHADER_COMPONENTS & (1 << 10)
#define ZE_SHADER_OPACITY_BASE_ALPHA	ZE_SHADER_COMPONENTS & (1 << 11)
#define ZE_SHADER_OPACITY_CONSTANT		ZE_SHADER_COMPONENTS & (1 << 12)
#define ZE_SHADER_OPACITY_MAP			ZE_SHADER_COMPONENTS & (1 << 13)
#define ZE_SHADER_DETAIL_MAP			ZE_SHADER_COMPONENTS & (1 << 14)
#define ZE_SHADER_DETAIL_DIFFUSE_MAP	ZE_SHADER_COMPONENTS & (1 << 15)
#define ZE_SHADER_DETAIL_NORMAL_MAP		ZE_SHADER_COMPONENTS & (1 << 16)
#define ZE_SHADER_REFLECTION			ZE_SHADER_COMPONENTS & (1 << 17)
#define ZE_SHADER_REFRACTION			ZE_SHADER_COMPONENTS & (1 << 18)
#define ZE_SHADER_LIGHT_MAP				ZE_SHADER_COMPONENTS & (1 << 19)
#define ZE_SHADER_DISTORTION_MAP		ZE_SHADER_COMPONENTS & (1 << 20)
#define ZE_SHADER_SKIN_TRANSFORM		ZE_SHADER_COMPONENTS & (1 << 21)
*/

#ifdef ZE_SHADER_COMPONENT_1
#define ZE_SHADER_AMBIENT
#endif

#ifdef ZE_SHADER_COMPONENT_2
#define ZE_SHADER_DIFFUSE
#endif

#ifdef ZE_SHADER_COMPONENT_3
#define ZE_SHADER_BASE_MAP
#endif

#ifdef ZE_SHADER_COMPONENT_4
#define ZE_SHADER_NORMAL_MAP
#endif

#ifdef ZE_SHADER_COMPONENT_5
#define ZE_SHADER_PARALLAX_MAP
#endif

#ifdef ZE_SHADER_COMPONENT_6
#define ZE_SHADER_SPECULAR
#endif

#ifdef ZE_SHADER_COMPONENT_7
#define ZE_SHADER_SPECULAR_MAP
#endif

#ifdef ZE_SHADER_COMPONENT_8
#define ZE_SHADER_EMMISIVE
#endif

#ifdef ZE_SHADER_COMPONENT_9
#define ZE_SHADER_EMMISIVE_MAP
#endif

#ifdef ZE_SHADER_COMPONENT_10
#define ZE_SHADER_OPACITY
#endif

#ifdef ZE_SHADER_COMPONENT_11
#define ZE_SHADER_OPACITY_BASE_ALPHA
#endif

#ifdef ZE_SHADER_COMPONENT_12
#define ZE_SHADER_OPACITY_CONSTANT
#endif

#ifdef ZE_SHADER_COMPONENT_13
#define ZE_SHADER_OPACITY_MAP
#endif

#ifdef ZE_SHADER_COMPONENT_14
#define ZE_SHADER_DETAIL_MAP
#endif

#ifdef ZE_SHADER_COMPONENT_15
#define ZE_SHADER_DETAIL_DIFFUSE_MAP
#endif

#ifdef ZE_SHADER_COMPONENT_16
#define ZE_SHADER_DETAIL_NORMAL_MAP
#endif

#ifdef ZE_SHADER_COMPONENT_17
#define ZE_SHADER_REFLECTION
#endif

#ifdef ZE_SHADER_COMPONENT_18
#define ZE_SHADER_REFRACTION
#endif

#ifdef ZE_SHADER_COMPONENT_19
#define ZE_SHADER_LIGHT_MAP
#endif

#ifdef ZE_SHADER_COMPONENT_20
#define ZE_SHADER_DISTORTION_MAP
#endif

#ifdef ZE_SHADER_COMPONENT_21
#define ZE_SHADER_SKIN_TRANSFORM
#endif

#ifdef ZE_SHADER_COMPONENT_22
#define ZE_SHADER_ALPHA_CULL
#endif

#ifdef ZE_SHADER_COMPONENT_23
#define ZE_SHADER_VERTEX_COLOR
#endif

#ifdef ZE_SHADER_COMPONENT_24
#define ZE_SHADER_SSAO
#endif

// Derivated Custom Shader Components
#if defined(ZE_SHADER_NORMAL_MAP) || defined(ZE_SHADER_PARALLAX_MAP) || defined(ZE_SHADER_DETAIL_NORMAL_MAP)
#define ZE_SHADER_TANGENT_SPACE
#endif

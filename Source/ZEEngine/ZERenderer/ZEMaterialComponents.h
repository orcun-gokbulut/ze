//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMaterialComponents.h
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
#ifndef	__ZE_MATERIAL_COMPONENTS_H__
#define __ZE_MATERIAL_COMPONENTS_H__


// Vertex Transform Properties
#define ZE_SHADER_MORPHING
#define ZE_SHADER_SKINNING

// Material Properties
#define ZE_SHADER_AMBIENT					(1 << 1)
#define ZE_SHADER_PARAMETER_AMBIENT			

#define ZE_SHADER_DIFFUSE					(1 << 2)
#define ZE_SHADER_PARAMETER_DIFFUSE			

#define ZE_SHADER_BASE_MAP					(1 << 3)
#define	ZE_SHADER_PARAMETER_BASE_MAP		

#define ZE_SHADER_BUMP_MAPPING

#define ZE_SHADER_NORMAL_MAP				(1 << 4)
#define	ZE_SHADER_PARAMETER_NORMAL_MAP		

#define ZE_SHADER_PARALLAX_MAP				(1 << 5)
#define	ZE_SHADER_PARAMETER_PARALLAX_MAP	

#define ZE_SHADER_SPECULAR					(1 << 6)
#define	ZE_SHADER_PARAMETER_SPECULAR		

#define ZE_SHADER_SPECULAR_MAP				(1 << 7)
#define	ZE_SHADER_PARAMETER_SPECULAR_MAP	

#define ZE_SHADER_EMMISIVE					(1 << 8)
#define	ZE_SHADER_PARAMETER_EMMISIVE		

#define ZE_SHADER_EMMISIVE_MAP				(1 << 9)
#define	ZE_SHADER_PARAMETER_EMMISIVE_MAP	

#define ZE_SHADER_OPACITY					(1 << 10)
#define	ZE_SHADER_PARAMETER_OPACITY			

#define ZE_SHADER_OPACITY_BASE_ALPHA		(1 << 11)
#define	ZE_SHADER_PARAMETER_OPACITY_BASE_ALPHA 

#define ZE_SHADER_OPACITY_CONSTANT			(1 << 12)
#define	ZE_SHADER_PARAMETER_OPACITY_CONSTANT 

#define ZE_SHADER_OPACITY_MAP				(1 << 13)
#define	ZE_SHADER_PARAMETER_OPACITY_MAP		

#define ZE_SHADER_DETAIL_MAP				(1 << 14)
#define	ZE_SHADER_PARAMETER_DETAIL_MAP		

#define ZE_SHADER_DETAIL_BASE_MAP			(1 << 15)
#define	ZE_SHADER_PARAMETER_DETAIL_BASE_MAP	

#define ZE_SHADER_DETAIL_NORMAL_MAP			(1 << 16)
#define	ZE_SHADER_PARAMETER_DETAIL_NORMAL_MAP 

#define ZE_SHADER_REFLECTION				(1 << 17)
#define	ZE_SHADER_PARAMETER_REFLECTION		

#define ZE_SHADER_REFRACTION				(1 << 18)
#define	ZE_SHADER_PARAMETER_REFRACTION		

#define ZE_SHADER_LIGHT_MAP					(1 << 19)
#define	ZE_SHADER_PARAMETER_LIGHT_MAP		

#define ZE_SHADER_DISTORTION_MAP			(1 << 20)
#define	ZE_SHADER_PARAMETER_DISTORTION_MAP	

#define ZE_SHADER_ALPHA_CULL				(1 << 22)
#define	ZE_SHADER_PARAMETER_ALPHA_CULL		

#define ZE_SHADER_VERTEX_COLOR				(1 << 23)
#define	ZE_SHADER_PARAMETER_VERTEX_COLOR	



#endif





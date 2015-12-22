//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNShading.hlsl
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

#ifndef __ZERN_SHADING_H__
#define __ZERN_SHADING_H__

#include "ZERNTransformations.hlsl"

struct ZERNShading_Light
{
	float3						PositionView;
	float						Range;
	float3						Color;
	float						Intensity;
	float3  					Attenuation;
	float						Fov;
	float3						DirectionView;
	int							Type;
};

struct ZERNShading_Surface
{
	float3						PositionView;
	float						Reserved;
	float3						NormalView;
	float						Reserved1;
	float3						Diffuse;
	float						Reserved2;
	float3						Specular;
	float						SpecularPower;
};

float3 ZERNShading_Diffuse_Lambert(ZERNShading_Light Light, ZERNShading_Surface Surface)
{
	float NdotL = max(0.0f, dot(Surface.NormalView, Light.DirectionView));
	
	return NdotL * Surface.Diffuse * Light.Color;
}

float3 ZERNShading_Specular_BlinnPhong(ZERNShading_Light Light, ZERNShading_Surface Surface)
{	
	float3 ViewDirection = normalize(-Surface.PositionView);
	float3 HalfVector = normalize(ViewDirection + Light.DirectionView);
	float NdotH = max(0.0f, dot(Surface.NormalView, HalfVector));
	
	return pow(NdotH, Surface.SpecularPower) * Surface.Specular * Light.Color;
}

#endif

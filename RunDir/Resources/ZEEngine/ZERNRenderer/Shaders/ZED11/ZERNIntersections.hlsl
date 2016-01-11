//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNIntersections.hlsl
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

#ifndef __ZERN_INTERSECTIONS_H__
#define __ZERN_INTERSECTIONS_H__

void ZERNIntersections_RaySphere1(float3 RayOrigin, float3 RayDirection, float3 SphereCenter, float SphereRadius, out float2 StartEndDistance)
{
	float3 RayOrigin_SphereCenter = RayOrigin - SphereCenter;
	float2 A = dot(RayDirection, RayDirection);
	float2 B = 2.0f * dot(RayDirection, RayOrigin_SphereCenter);
	float2 C = dot(RayOrigin_SphereCenter, RayOrigin_SphereCenter) - SphereRadius * SphereRadius;
	
	float2 Discriminant = B * B - 4.0f * A * C;
	float2 SquareRootDiscriminant = sqrt(Discriminant);
	
	if(Discriminant.x >= 0.0f)
		StartEndDistance = (-B + float2(-1.0f, 1.0f) * SquareRootDiscriminant) / (2.0f * A);
}

void ZERNIntersections_RaySphere2(float3 RayOrigin, float3 RayDirection, float3 SphereCenter, float2 SphereRadius, out float4 StartEndDistance)
{
	float3 RayOrigin_SphereCenter = RayOrigin - SphereCenter;
	float2 A = dot(RayDirection, RayDirection);
	float2 B = 2.0f * dot(RayDirection, RayOrigin_SphereCenter);
	float2 C = dot(RayOrigin_SphereCenter, RayOrigin_SphereCenter) - SphereRadius * SphereRadius;
	
	float2 Discriminant = B * B - 4.0f * A * C;
	float2 SquareRootDiscriminant = sqrt(Discriminant);
	float2 RealRootMask = (Discriminant >= 0.0f);
	
	StartEndDistance = RealRootMask.xxyy * ((-B.xxyy + float4(-1.0f, 1.0f, -1.0f, 1.0f) * SquareRootDiscriminant.xxyy) / (2.0f * A.xxyy));
}

#endif

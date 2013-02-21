//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - PipelineConstants.hlsl
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

cbuffer	ZEPipelineConstants : register(b13)
{
	// Elapsed time

	float		ZETime0toN						: packoffset(c0.x);
	float		ZESinTime0toN					: packoffset(c0.y);
	float		ZECosTime0toN					: packoffset(c0.z);
	float		ZETanTime0toN					: packoffset(c0.w);
	
	float		ZETime0to1						: packoffset(c1.x);
	float		ZESinTime0to1					: packoffset(c1.y);
	float		ZECosTime0to1					: packoffset(c1.z);
	float		ZETanTime0to1					: packoffset(c1.w);
	
	float		ZEFov							: packoffset(c2.x);
	float2		ZENearFarZ						: packoffset(c2.y);
	float		ZEAspectRatio					: packoffset(c2.w);
	
	float3		ZECameraWorldPos				: packoffset(c3.x);
	float		ZEElapsedTime					: packoffset(c3.w);
	float3		ZECameraWorldUp					: packoffset(c4.x);
	float3		ZECameraWorldFront				: packoffset(c5.x);
	float3		ZECameraWorldRight				: packoffset(c6.x);
	
	float2		ZEViewportWidthHeight			: packoffset(c7.x);
	float2		ZEInvViewportWidthHeight		: packoffset(c7.z);
};

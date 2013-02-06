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

cbuffer	ZEPipelineConstants : register(b5)
{
	float	ZE_PC_Time_0_N						: packoffset(c0.x);
	float	ZE_PC_Time_0_1						: packoffset(c0.y);
	float	ZE_PC_Sin_Time_0_N					: packoffset(c0.z);
	float	ZE_PC_Cos_Time_0_N					: packoffset(c0.w);
	float	ZE_PC_Tan_Time_0_N					: packoffset(c1.x);
	float	ZE_PC_Sin_Time_0_1					: packoffset(c1.y);
	float	ZE_PC_Cos_Time_0_1					: packoffset(c1.z);
	float	ZE_PC_Tan_Time_0_1					: packoffset(c1.w);
	float	ZE_PSC_Fov							: packoffset(c2.x);
	float2	ZE_PSC_Near_Far_Z					: packoffset(c2.y);
	float	ZE_PSC_Aspect_Ratio					: packoffset(c2.w);
	float3	ZE_PSC_Camera_Pos					: packoffset(c3.x);
	float3	ZE_PSC_Camera_World_Up				: packoffset(c4.x);
	float3	ZE_PSC_Camera_World_Front			: packoffset(c5.x);
	float3	ZE_PSC_Camera_World_Right			: packoffset(c6.x);
	float2	ZE_PSC_Viewport_Width_Height		: packoffset(c7.x);
	float2	ZE_PSC_Inv_Viewport_Width_Height	: packoffset(c7.z);
	flaot4	ZE_PSC_View_Matrix;					: packoffset(c8.x);
	flaot4	ZE_PSC_Proj_Matrix;					: packoffset(c12.x);
	flaot4	ZE_PSC_Inv_View_Matrix				: packoffset(c16.x);
	flaot4	ZE_PSC_Inv_Proj_Matrix				: packoffset(c20.x);
	flaot4	ZE_PSC_View_Proj_Matrix				: packoffset(c24.x);
	flaot4	ZE_PSC_Inv_View_Proj_Matrix			: packoffset(c28.x);
};

//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Shader.h
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
#ifndef __ZE_SHADER_H__
#define __ZE_SHADER_H__

#define ZESHADER_COMPONENTCOUNT				12

#define ZESHADER_SKINTRANSFORM				1
#define ZESHADER_DIFFUSEMAP					2
#define ZESHADER_NORMALMAP					4
#define ZESHADER_SPECULARMAP				8
#define ZESHADER_EMMISIVEMAP				16
#define ZESHADER_OCAPASITYMAP				32
#define ZESHADER_DETAILDIFFUSEMAP			64
#define ZESHADER_DETAILNORMALMAP			128
#define ZESHADER_REFLECTION					256
#define ZESHADER_REFRACTION					512
#define ZESHADER_LIGHTMAP					1024
#define ZESHADER_DISTORTIONMAP				2048
#define ZESHADER_SKY						4096				




class ZEShader
{
	public:
		//ZEShader*					Create(unsigned int ShaderComponents);
		virtual void				Release() = 0;
};

#endif

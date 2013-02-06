//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETexture.h
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
#ifndef __ZE_TEXTURE_H__
#define __ZE_TEXTURE_H__


#include "ZETypes.h"

enum ZETextureType
{
	ZE_TT_NONE		= 0,
	ZE_TT_2D		= 1,
	ZE_TT_3D		= 2,
	ZE_TT_CUBE		= 3
};

enum ZETexturePixelFormat
{
	ZE_TPF_NOTSET,

	ZE_TPF_I8,
	ZE_TPF_I8_2,
	ZE_TPF_I8_4,

	ZE_TPF_I16,
	ZE_TPF_I16_2,
	ZE_TPF_I16_4,

	ZE_TPF_I32,

	ZE_TPF_F16,
	ZE_TPF_F16_2,
	ZE_TPF_F16_4,

	ZE_TPF_F32,
	ZE_TPF_F32_2,
	ZE_TPF_F32_4,

	ZE_TPF_DXT1,
	ZE_TPF_DXT3,
	ZE_TPF_DXT5
};

class ZETexture
{
	friend class ZEGraphicsDevice;
	friend class ZEGraphicsModule;

	// Should be public for only internal usage
	public:
		bool						Static;
		bool						RenderTarget;
		ZETextureType				TextureType;
		ZETexturePixelFormat		PixelFormat;

	protected:
									ZETexture();
		virtual						~ZETexture();

	public:
		bool						IsStatic() const;
		bool						IsRenderTarget() const;
		ZETextureType				GetTextureType() const;
		ZETexturePixelFormat		GetPixelFormat() const;

		virtual void				Destroy();

		virtual bool				IsEmpty() const = 0;
};

#endif





//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETexture2D.h
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
#ifndef __ZE_TEXTURE_2D_H__
#define __ZE_TEXTURE_2D_H__

#include "ZETypes.h"
#include "ZETexture.h"

class ZEViewPort;

class ZETexture2D : public ZETexture
{
	ZE_OBJECT

	protected:
		ZEUInt						Width;
		ZEUInt						Height;	
		ZEUInt						LevelCount;

		ZETexturePixelFormat		PixelFormat;
		bool						RenderTarget;

									ZETexture2D();
		virtual						~ZETexture2D();

	public:
		virtual ZETextureType		GetTextureType() const;

		ZEUInt						GetWidth() const;
		ZEUInt						GetHeight() const;
		ZEUInt						GetLevelCount() const;

		ZETexturePixelFormat		GetPixelFormat() const;
		bool						IsRenderTarget() const;

		virtual bool				IsEmpty() const = 0;

		virtual ZEViewPort*			GetViewPort() = 0;

		virtual bool				Create(ZEUInt Width, ZEUInt Height, ZEUInt LevelCount, ZETexturePixelFormat PixelFormat, bool RenderTarget = false) = 0;
		virtual void				Lock(void** Buffer, ZESize* Pitch, ZEUInt Level) = 0;
		virtual void				Lock(void** Buffer, ZESize* Pitch, ZEUInt Level, ZEUInt PositionX, ZEUInt PositionY, ZEUInt Width, ZEUInt Height) = 0;

		virtual void				Unlock(ZEUInt Level) = 0;

		static ZETexture2D*			CreateInstance();
};


#endif

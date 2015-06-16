//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDepthStencilBuffer.h
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

#ifndef __ZE_DEPTH_STENCIL_BUFFER_H__ 
#define __ZE_DEPTH_STENCIL_BUFFER_H__

#include "ZETypes.h"
#include "ZEDS/ZEString.h"

enum ZEDepthStencilPixelFormat
{
	ZE_DSPF_NOTSET				= 0,
	ZE_DSPF_DEPTH16				= 1,	// 16 bit unsigned normalized depth values
	ZE_DSPF_DEPTH24_STENCIL8	= 2,	// 24 bit unsigned normalized depth values + 8 bit unsigned int stencil values
	ZE_DSPF_DEPTHD32_FLOAT		= 3,	// 32 bit float depth values
};

class ZEDepthStencilBuffer
{
	friend class ZEGraphicsModule;
	friend class ZEGraphicsDevice;

	protected:
		static ZESize					TotalSize;
		static ZEUInt16					TotalCount;

#ifdef ZE_DEBUG_ENABLE
		ZEString						DebugName;
#endif
		ZEUInt							Width;
		ZEUInt							Height;
		ZEDepthStencilPixelFormat		PixelFormat;

										ZEDepthStencilBuffer();
		virtual							~ZEDepthStencilBuffer();

	public:
		ZEUInt							GetWidth() const;
		ZEUInt							GetHeight() const;
		ZEDepthStencilPixelFormat		GetPixelFormat() const;

		void							SetDebugName(const char* String);
		const char*						GetDebugName() const;

		virtual bool					IsEmpty() const = 0;
		
		virtual void					Destroy();
		virtual bool					Create(ZEUInt Width, ZEUInt Height, ZEDepthStencilPixelFormat PixelFormat);

		static ZEDepthStencilBuffer*	CreateInstance();
};

#endif

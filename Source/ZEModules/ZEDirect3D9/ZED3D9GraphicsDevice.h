//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9GraphicsDevice.h
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

#ifndef __ZE_D3D9_GRAPHICS_DEVICE_H__
#define __ZE_D3D9_GRAPHICS_DEVICE_H__

#include "ZEGraphics/ZEGraphicsDevice.h"
#include "ZED3D9ComponentBase.h"

class ZED3D9GraphicsDevice : public ZEGraphicsDevice, private ZED3D9ComponentBase
{
	private:
		ZEBlendState			DefaultBlendState;
		ZERasterizerState		DefaultRasterStates;
		ZEStencilZState			DefaultStencilZState;
		ZESamplerState			DefaultSamplerState;

	public:
		void					ApplyStates();
		void					ResetToDefaultStates();

		bool					Present();
		bool					EndScene();
		bool					BeginScene();
		bool					Clear(bool RenderTargets, bool ZBuffer, bool StencilBuffer, const ZEVector4& Color, float ZClearValue, ZEUInt32 StencilClearValue);

		virtual void			Draw(ZEPrimitiveType PrimitiveType, ZEUInt32 StartVertex, ZEUInt32 PrimitiveCount);
		virtual void			DrawIndexed(ZEPrimitiveType PrimitiveType, ZEInt BaseVertexIndex, ZEUInt32 MinIndex, ZEUInt32 VertexCount, ZEUInt32 StartIndex, ZEUInt32 PrimitiveCount);

								ZED3D9GraphicsDevice();
		virtual					~ZED3D9GraphicsDevice();
};
#endif

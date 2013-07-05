//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D10GraphicsDevice.h
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

#ifndef __ZE_D3D10_GRAPHICS_DEVICE_H__
#define __ZE_D3D10_GRAPHICS_DEVICE_H__

#include "ZED3D10StatePool.h"
#include "ZED3D10ComponentBase.h"
#include "ZEGraphics/ZEGraphicsDevice.h"

class ZERenderTarget;
class ZEDepthStencilBuffer;

class ZED3D10GraphicsDevice : public ZEGraphicsDevice, public ZED3D10ComponentBase
{
	friend class ZED3D10GraphicsModule;

	protected:
		void				ApplyInputStates();
		void				ApplyVertexShaderStates();
		void				ApplyGeometryShaderStates();
		void				ApplyRasterizerStates();
		void				ApplyPixelShaderStates();
		void				ApplyOutputStates();

		void				ApplyStates();
		
							ZED3D10GraphicsDevice();
		virtual				~ZED3D10GraphicsDevice();

	public:
		virtual bool		Present() const;

		virtual void		Draw(ZEPrimitiveType PrimitiveType, ZEUInt VertexCount, ZEUInt FirstVertex);
		virtual void		DrawIndexed(ZEPrimitiveType PrimitiveType, ZEUInt IndexCount, ZEUInt FirstIndex, ZEInt BaseVertex);
		virtual void		DrawInstanced(ZEPrimitiveType PrimitiveType, ZEUInt VertexCount, ZEUInt FirstVertex, ZEUInt InstanceCount, ZEUInt FirstInstance);
		virtual void		DrawIndexedInstanced(ZEPrimitiveType PrimitiveType, ZEUInt IndexCount, ZEUInt InstanceCount, ZEUInt FirstIndex, ZEInt BaseVertex, ZEUInt FirstInstance);

		virtual void		ClearRenderTarget(const ZERenderTarget* RenderTarget, const ZEVector4& ClearColor);
		virtual void		ClearDepthStencilBuffer(const ZEDepthStencilBuffer* DepthStencil, bool Depth, bool Stencil, float DepthValue, ZEUInt8 StencilValue);
};

#endif

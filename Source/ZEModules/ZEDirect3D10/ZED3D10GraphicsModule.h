//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D10GraphicsModule.h
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

#ifndef	__ZE_D3D10_GRAPHICS_MODULE_H__
#define __ZE_D3D10_GRAPHICS_MODULE_H__

#include "ZETypes.h"
#include "ZEDS/ZEString.h"
#include "ZEGraphics/ZEGraphicsModule.h"

#include <D3D10.h>

class ZETexture2D;
class ZETexture3D;
class ZETextureCube;
class ZEIndexBuffer;
class ZEVertexBuffer;
class ZEShaderCompiler;
class ZEGraphicsDevice;
class ZEDepthStencilBuffer;
class ZEGraphicsEventTracer;

#define ZED3D_RELEASE(x)	\
{							\
	if ((x) != NULL)		\
	{						\
		(x)->Release();		\
		(x) = NULL;			\
	}						\
}

class ZED3D10GraphicsModule : public ZEGraphicsModule
{
	ZE_MODULE(ZED3D10GraphicsModule)
	
	friend class ZED3D10GraphicsModuleDescription;

	protected:
		bool							Enabled;
		ZEString						VideoCardName;
		ZEArray<DXGI_MODE_DESC>			AvailableModes;

		ID3D10Device*					Device;
		IDXGISwapChain*					SwapChain;
		IDXGIOutput*					AdapterOutput;
		IDXGIAdapter*					Adapter;
		IDXGIFactory*					Factory;
		
		bool							SetFullScreenState(bool Value);
		bool							ResizeBackBuffers(ZEUInt Width, ZEUInt Height);
		bool							ResizeFrontBuffer(DXGI_MODE_DESC& ModeToSwitchTo);
		DXGI_MODE_DESC					FindMatchingDisplayMode(ZEUInt Width, ZEUInt Height);

		bool							CreateBuffers(ZEUInt Width, ZEUInt Height);
		void							DestroyBuffers();

										ZED3D10GraphicsModule();
		virtual							~ZED3D10GraphicsModule();

	public:
		bool							Initialize();
		void							Deinitialize();

		virtual bool					SetFullScreen(bool Enabled);
		virtual bool					SetSampleCount(ZEUInt Count);
		virtual void					SetVerticalSync(bool Enabled);
		virtual bool					SetScreenCount(ZEUInt Count);
		
		virtual void					SetAnisotropicFilter(ZEUInt Anisotropy);
		virtual bool					SetScreenSize(ZEUInt Width, ZEUInt Height);

		virtual ZEGraphicsDevice*		GetDevice() const;
		virtual ZEGraphicsEventTracer*	GetEventTracer() const;
		virtual ZEShaderCompiler*		GetShaderCompiler() const;

		virtual ZETexture2D*			CreateTexture2D() const;
		virtual ZETexture3D*			CreateTexture3D() const;
		virtual ZETextureCube*			CreateTextureCube() const;
		virtual ZEIndexBuffer*			CreateIndexBuffer() const;
		virtual ZEVertexBuffer*			CreateVertexBuffer() const;
		virtual ZEConstantBuffer*		CreateConstantBuffer() const;
		virtual ZEDepthStencilBuffer*	CreateDepthStencilBuffer() const;

		ID3D10Device*					GetD3D10Device() const;
		IDXGISwapChain*					GetDXGISwapChain() const;
};

#endif

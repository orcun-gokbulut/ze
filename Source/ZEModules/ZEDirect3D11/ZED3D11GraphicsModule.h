//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D11GraphicsModule.h
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

#ifndef	__ZE_D3D11_GRAPHICS_MODULE_H__
#define __ZE_D3D11_GRAPHICS_MODULE_H__

#include <dxgi1_2.h>
#include <d3d11.h>

#include "ZETypes.h"
#include "ZEDS/ZEString.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZEGraphics/ZEGRMonitor.h"

class ZEGRTexture2D;
class ZEGRTexture3D;
class ZEStatePool;
class ZEGRTextureCube;
class ZEGRIndexBuffer;
class ZEGRVertexBuffer;
class ZEGRShaderCompiler;
class ZEGRDevice;
class ZEDepthStencilBuffer;
class ZEGraphicsEventTracer;
class ZED3D11GraphicsDevice;

#define ZED3D_RELEASE(x)	\
{							\
	if ((x) != NULL)		\
	{						\
		(x)->Release();		\
		(x) = NULL;			\
	}						\
}

#define ZE_MAX_MULTI_SAMPLE_COUNT		D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT
#define ZE_MAX_ANISOTROPY_LEVEL			D3D11_MAX_MAXANISOTROPY

class ZED3D11GraphicsModule : public ZEGRGraphicsModule
{
	ZE_MODULE(ZED3D11GraphicsModule)

	protected:
		IDXGIFactory2*							DXGIFactory;

		ZEArray<IDXGIAdapter2*>					Adapters;
		ZEArray<ID3D11Device*>					D3DDevices;
		ZEArray<ID3D11DeviceContext*>			D3DContexes;

		DXGI_FORMAT								DisplayFormat;

		void									ReleaseWindows();

		void									ReleaseFactory();
		bool									CreateFactory();

		void									ReleaseAdapters();
		bool									EnumerateAdapters();

		void									ReleaseMonitors();
		bool									EnumerateMonitors();

		void									ReleaseDevices();
		bool									CreateDevices();

		bool									DisableAssociations();
		
		virtual bool							InitializeSelf();
		virtual bool							DeinitializeSelf();

												ZED3D11GraphicsModule();
		virtual									~ZED3D11GraphicsModule();

	public:
		virtual ZEStatePool*					GetStatePool() const;
		virtual ZEGraphicsEventTracer*			GetEventTracer() const;
		virtual ZEGRShaderCompiler*				GetShaderCompiler() const;

		virtual ZEGRTexture2D*					CreateTexture2D();
		virtual ZEGRTexture3D*					CreateTexture3D();
		virtual ZEGRTextureCube*					CreateTextureCube();
		virtual ZEGRIndexBuffer*					CreateIndexBuffer();
		virtual ZEGRVertexBuffer*					CreateVertexBuffer();
		virtual ZEGRWindow*				CreateGraphicsWindow();
		virtual ZEConstantBuffer*				CreateConstantBuffer();
		virtual ZEDepthStencilBuffer*			CreateDepthStencilBuffer();
		
		ID3D11Device*							GetD3D10Device(ZESize Index = 0) const;
		const ZEArray<ID3D11Device*>&			GetD3D10Devices() const;
		
		ID3D11DeviceContext*					GetD3D10Context(ZESize Index = 0) const;
		const ZEArray<ID3D11DeviceContext*>&	GetD3D10Contexes() const;

		IDXGIFactory2*							GetDXGIFactory() const;
		DXGI_FORMAT								GetDXGIDisplayFormat() const;
};




#endif

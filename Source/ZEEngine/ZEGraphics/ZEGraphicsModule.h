//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGraphicsModule.h
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

#ifndef	__ZE_GRAPHICS_MODULE_H__
#define __ZE_GRAPHICS_MODULE_H__

#include "ZETypes.h"
#include "ZECore/ZEModule.h"
#include "ZEGraphicsMonitor.h"
#include "ZEGraphicsDeviceState.h"
#include "ZECore/ZEOptionSection.h"
#include "ZETexture/ZETextureOptions.h"

#define ZE_DESTROY(x)		\
{							\
	if ((x) != NULL)		\
	{						\
		(x)->Destroy();		\
		(x) = NULL;			\
	}						\
}



class ZEGRRenderState;
class ZEGRTexture2D;
class ZEGRTexture3D;
class ZEGRTextureCube;
class ZEGRIndexBuffer;
class ZEGRVertexBuffer;
class ZEGRVertexLayout;
class ZEGraphicsDevice;
class ZEGRDevice;
class ZEGraphicsWindow;
class ZEGRShader;
class ZEGRShaderCompiler;
class ZEGRConstantBuffer;

class ZEGraphicsModule : public ZEModule
{
	ZE_MODULE(ZEGraphicsModule)

	friend class ZEGraphicsDevice;

	protected:
		ZEArray<ZEGraphicsWindow*>			Windows;
		ZEArray<ZEGRMonitor*>				Monitors;
		ZEArray<ZEGraphicsDevice*>			Devices;

		virtual bool						InitializeSelf();
		virtual bool						DeinitializeSelf();

											ZEGraphicsModule();
		virtual								~ZEGraphicsModule();

	public:
		// NOTE: These methods should be here ?
		// -------------------------------------------
		ZETextureQuality					TextureQuality;
		static ZEOptionSection				GraphicsOptions;
		static void							BaseInitialize();
		static void							BaseDeinitialize();
		virtual ZETextureOptions*			GetTextureOptions();
		// -------------------------------------------

		const ZEArray<ZEGraphicsWindow*>&	GetWindows() const;
		const ZEArray<ZEGRMonitor*>&		GetMonitors() const;
		const ZEArray<ZEGraphicsDevice*>&	GetDevices() const;

		virtual ZEGraphicsWindow*			CreateGraphicsWindow() = 0;
		virtual ZEGRDevice*					CreateContext();
		virtual ZEGRTexture2D*				CreateTexture2D() = 0;
		virtual ZEGRTexture3D*				CreateTexture3D() = 0;
		virtual ZEGRTextureCube*			CreateTextureCube() = 0;
		virtual ZEGRIndexBuffer*			CreateIndexBuffer() = 0;
		virtual ZEVertexBuffer*				CreateVertexBuffer() = 0;
		virtual ZEGRConstantBuffer*			CreateConstantBuffer() = 0;
		virtual ZEGRDepthStencilBuffer*		CreateDepthStencilBuffer() = 0;
		virtual ZEGRShaderCompiler*			CreateShaderCompiler() = 0;

		virtual void						GetStatistics(ZEGraphicsStatistics& Statistics) const;

		static ZEGraphicsModule*			GetInstance();
};

#endif

//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9SSAAProcessor.h
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
#ifndef __ZE_D3D9_SSAA_PROCESSOR_H__
#define __ZE_D3D9_SSAA_PROCESSOR_H__

#include "ZED3D9ComponentBase.h"

class ZED3D9PixelShader;
class ZED3D9VertexShader;
class ZED3D9Texture2D;
class ZETexture2D;
class ZED3D9ViewPort;
class ZEFrameRenderer;
class ZED3D9FrameRenderer;
class ZETexture2DResource;

class ZED3D9SSAAProcessor : public ZED3D9ComponentBase
{
	private:
		ZED3D9FrameRenderer*			Renderer;

		LPDIRECT3DVERTEXDECLARATION9	VertexDeclaration;
		ZED3D9VertexShader*				VertexShader;
		ZED3D9PixelShader*				PixelShader;

		ZED3D9Texture2D*				InputColorBuffer;
		ZED3D9Texture2D*				InputDepthBuffer;
		ZED3D9Texture2D*				InputNormalBuffer;
		ZED3D9ViewPort*					Output;


	public:
		void							SetRenderer(ZEFrameRenderer* Renderer);
		ZEFrameRenderer*				GetRenderer();

		void							SetInputColor(ZETexture2D* Texture);
		ZETexture2D*					GetInputColor();

		void							SetInputDepth(ZETexture2D* Texture);
		ZETexture2D*					GetInputDepth();

		void							SetInputNormal(ZETexture2D* Texture);
		ZETexture2D*					GetInputNormal();

		void							SetOutput(ZED3D9ViewPort* Texture);
		ZED3D9ViewPort*					GetOutput();

		void							Initialize();
		void							Deinitialize();

		void							OnDeviceLost();
		void							OnDeviceRestored();

		void							Process();

										ZED3D9SSAAProcessor();
										~ZED3D9SSAAProcessor();
};

#endif	/* __ZE_D3D9_SSAA_PROCESSOR_H__ */
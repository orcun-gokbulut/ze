//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED11Texture3D.h
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

#include "ZEGraphics/ZEGRTexture3D.h"
#include "ZED11ComponentBase.h"
#include "ZETypes.h"

class ZEGRRenderTarget;

class ZED11Texture3D : public ZEGRTexture3D, public ZED11ComponentBase
{
	friend class ZED11Module;
	friend class ZED11Context;

	private:
		ID3D11Texture3D*					Texture3D;
		ID3D11ShaderResourceView*			ResourceView;
		ID3D11UnorderedAccessView*			UnorderedAccessView;

		virtual bool						Initialize(ZEUInt Width, ZEUInt Height, ZEUInt Depth, ZEUInt LevelCount, ZEGRFormat Format, bool RenderTarget, bool UAV);
		virtual void						Deinitialize();

											ZED11Texture3D();

	public:
		ID3D11Texture3D*					GetTexture();
		ID3D11ShaderResourceView*			GetResourceView();
		ID3D11UnorderedAccessView*			GetUnorderedAccessView();

		virtual bool						Lock(void** Buffer, ZESize* Pitch, ZEUInt Depth, ZEUInt Level);
		virtual void						Unlock(ZEUInt Level);

		virtual ZEGRRenderTarget*			GetRenderTarget(ZEUInt Depth, ZEUInt MipLevel);
};

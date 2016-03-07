//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNSunMaterial.h
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

#include "ZERNMaterial.h"

#include "ZEPointer/ZEHolder.h"
#include "ZEDS/ZEFlags.h"
#include "ZEMath/ZEVector.h"

class ZEGRShader;
class ZEGRRenderStateData;
class ZEGRConstantBuffer;

class ZERNSunMaterial : public ZERNMaterial
{
private:
	mutable ZEFlags							DirtyFlags;

	mutable ZEHolder<ZEGRShader>			VertexShader;
	mutable ZEHolder<ZEGRShader>			PixelShader;
	mutable ZEHolder<ZEGRRenderStateData>	RenderStateData;

	ZEHolder<ZEGRConstantBuffer>			ConstantBuffer;

	struct 
	{
		ZEVector2							SunPositionScreen;
		ZEVector2							SunSizeScreen;
	} Constants;

	bool									UpdateShaders() const;
	bool									UpdateRenderState() const;
	bool									UpdateConstantBuffer() const;

	virtual bool							InitializeSelf();
	virtual void							DeinitializeSelf();

	ZERNSunMaterial();

public:
	virtual ZEUInt							GetStageMask() const;

	void									SetSunPositionScreen(const ZEVector2& SunPositionScreen);
	const ZEVector2&						GetSunPositionScreen() const;

	void									SetSunSizeScreen(const ZEVector2& SunSizeScreen);
	const ZEVector2&						GetSunSizeScreen() const;

	virtual bool							SetupMaterial(ZEGRContext* Context, ZERNStage* Stage) const;
	virtual void							CleanupMaterial(ZEGRContext* Context, ZERNStage* Stage) const;

	virtual bool							Update() const;

	static ZEHolder<ZERNSunMaterial>		CreateInstance();
};

//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEATMoon.h
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

#include "ZEGame/ZEEntity.h"

#include "ZEDS/ZEFlags.h"
#include "ZEMath/ZEVector.h"
#include "ZEPointer/ZEHolder.h"
#include "ZEATCommon.h"
#include "ZERenderer/ZERNCommand.h"
#include "ZERenderer/ZERNView.h"
#include "ZERenderer/ZERNMap.h"

class ZEGRShader;
class ZEGRRenderStateData;
class ZEGRConstantBuffer;
class ZEGRTexture2D;

class ZEATMoon : public ZEEntity
{
	private:
		ZEFlags							DirtyFlags;
		ZERNCommand						Command;

		ZEHolder<ZEGRShader>			VertexShader;
		ZEHolder<ZEGRShader>			PixelShader;
		ZEHolder<ZEGRRenderStateData>	RenderStateData;

		ZEHolder<ZEGRConstantBuffer>	ConstantBuffer;

		ZEHolder<const ZEGRTexture2D>	DensityBuffer;

		ZERNMap							PhaseTexture;

		ZEVector3						Direction;
		float							DiskRadius;

		struct 
		{
			ZEVector2					PositionScreen;
			ZEVector2					SizeScreen;

			float						Phase;
			float						CosZenith;
			float						Intensity;
			float						Reserved;
		} Constants;

		bool							UpdateShaders();
		bool							UpdateRenderStates();
		bool							UpdateConstantBuffers();
		bool							Update();

		bool							CalculateMoonPositionScreen(const ZERNView& View, ZEVector2& OutVector);

		virtual bool					InitializeSelf();
		virtual bool					DeinitializeSelf();

	public:
		virtual ZEDrawFlags				GetDrawFlags() const;

		void							SetTextureFile(const ZEString& FileName, ZEUInt HorizTileCount, ZEUInt VertTileCount);
		const ZEString&					GetTextureFile() const;

		void							SetDirection(const ZEVector3& Direction);
		const ZEVector3&				GetDirection() const;

		void							SetDiskRadius(float DiskRadius);
		float							GetDiskRadius() const;

		void							SetDensityBuffer(ZEGRTexture2D* DensityBuffer);
		const ZEGRTexture2D*			GetDensityBuffer() const;

		virtual bool					PreRender(const ZERNCullParameters* CullParameters);
		virtual void					Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command);

										ZEATMoon();
};

//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNSimpleMaterial.h
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

#include "ZETypes.h"
#include "ZERNMaterial.h"
#include "ZERNMap.h"
#include "ZERNStageID.h"
#include "ZEGraphics\ZEGRRenderState.h"
#include "ZEPacking.h"

class ZERNSimpleMaterial : public ZERNMaterial
{
	private:
		ZEFlags							DirtyFlags;

		bool							TwoSided;
		bool							Wireframe;
		bool							DepthTestDisabled;

		ZEGRHolder<ZEGRShader>			VertexShader;
		ZEGRHolder<ZEGRShader>			PixelShader;
		ZEGRHolder<ZEGRRenderStateData> RenderStateData;

		struct
		{
			ZEVector4					MaterialColor;
			bool 						TextureEnabled;
			float						Reserved0;
			bool 						VertexColorEnabled;
			float						Reserved1;
		} Constants;

		ZEGRHolder<ZEGRConstantBuffer>	ConstantBuffer;
		ZERNMap							TextureMap;

		void							UpdateShaders();
		void							UpdateRenderState();
		void							UpdateConstantBuffer();
	
		virtual bool					InitializeSelf();
		virtual void					DeinitializeSelf();

										ZERNSimpleMaterial();

	public:
		virtual ZERNStageMask			GetStageMask();

		void							SetTwoSided(bool Enable);
		bool							GetTwoSided() const;

		void							SetWireframe(bool Enable);
		bool							GetWireframe() const;

		void							SetDepthTestDisabled(bool Disabled);
		bool							GetDepthTestDisabled();

		void							SetMaterialColor(const ZEVector4& Color);
		const ZEVector4&				GetMaterialColor() const;

		void							SetTexture(const ZERNMap& Sampler);
		const ZERNMap&					GetTexture() const;
	
		void							SetVertexColorEnabled(bool Enable);
		bool							GetVertexColorEnabled();

		virtual bool					Update();

		virtual bool					SetupMaterial(ZEGRContext* Context, ZERNStage* Stage);
		virtual void					CleanupMaterial(ZEGRContext* Context, ZERNStage* Stage);

		static ZERNSimpleMaterial*		CreateInstance();
};

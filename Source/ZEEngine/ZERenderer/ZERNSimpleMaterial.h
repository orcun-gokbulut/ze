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

#include "ZERNMaterial.h"

#include "ZETypes.h"
#include "ZEDS/ZEFlags.h"
#include "ZEPointer/ZEHolder.h"
#include "ZEMath/ZEVector.h"
#include "ZERNMap.h"
#include "ZERNStageID.h"
#include "ZEGraphics/ZEGRRasterizerState.h"

class ZEGRShader;
class ZEGRRenderStateData;
class ZEGRConstantBuffer;

class ZERNSimpleMaterial : public ZERNMaterial
{
	ZE_OBJECT
	private:
		mutable ZEFlags							DirtyFlags;

		bool									TwoSided;
		bool									Wireframe;
		bool									DepthTestDisabled;
		bool									Transparent;
		ZERNStageMask							StageMask;
		ZEGRPrimitiveType						PrimitiveType;

		mutable ZEHolder<ZEGRShader>			VertexShader;
		mutable ZEHolder<ZEGRShader>			PixelShader;
		mutable ZEHolder<ZEGRRenderStateData>	RenderStateData;
		mutable ZEHolder<ZEGRConstantBuffer>	ConstantBuffer;

		ZERNMap									TextureMap;

		struct
		{
			ZEVector4							MaterialColor;
			ZEBool32 							TextureEnabled;
			ZEBool32 							VertexColorEnabled;
			float								Opacity;
			float								Reserved0;
		} Constants;

		bool									UpdateShaders() const;
		bool									UpdateRenderState() const;
		bool									UpdateConstantBuffer() const;

		virtual bool							InitializeInternal();
		virtual bool							DeinitializeInternal();

												ZERNSimpleMaterial();

	public:
		void									SetStageMask(ZERNStageMask Mask);
		virtual ZEUInt							GetStageMask() const;

		void									SetTwoSided(bool Enable);
		bool									GetTwoSided() const;

		void									SetWireframe(bool Enable);
		bool									GetWireframe() const;

		void									SetDepthTestDisabled(bool Disabled);
		bool									GetDepthTestDisabled() const;

		void									SetPrimitiveType(ZEGRPrimitiveType Type);
		ZEGRPrimitiveType						GetPrimitiveType() const;

		void									SetTransparent(bool Transparent);
		bool									GetTransparent() const;

		void									SetOpacity(float Opacity);
		float									GetOpacity() const;

		void									SetMaterialColor(const ZEVector4& Color);
		const ZEVector4&						GetMaterialColor() const;

		void									SetTexture(const ZERNMap& Map);
		const ZERNMap&							GetTexture() const;
	
		void									SetVertexColorEnabled(bool Enable);
		bool									GetVertexColorEnabled() const;

		virtual bool							Update() const;

		virtual bool							PreRender(ZERNCommand& Command);
		virtual bool							SetupMaterial(ZEGRContext* Context, const ZERNStage* Stage) const;
		virtual void							CleanupMaterial(ZEGRContext* Context, const ZERNStage* Stage) const;

		static ZEHolder<ZERNSimpleMaterial>		CreateInstance();
};

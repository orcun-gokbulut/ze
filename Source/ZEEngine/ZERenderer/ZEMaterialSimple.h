//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMaterialSimple.h
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
#ifndef __ZE_SIMPLE_MATERIAL_H__ 
#define __ZE_SIMPLE_MATERIAL_H__

#include "ZETypes.h"
#include "ZEMaterial.h"
#include "ZEMath/ZEVector.h"
#include "ZEGraphics/ZESamplerState.h"

class ZEShader;
class ZERenderer;
class ZETexture2D;
class ZERenderCommand;
class ZEConstantBuffer;

class ZEMaterialSimple : public ZEMaterial
{
	friend class ZED3D9GraphicsModule;
	protected:
		ZEShader*					VertexShader;
		ZEShader*					PixelShader;

		ZEConstantBuffer*			VertexShaderData;
		ZEConstantBuffer*			PixelShaderData;

		bool						TwoSided;
		bool						Wireframe;
		bool						VertexColorEnabled;

		ZEMaterialTransparancyMode	TransparancyMode;
		ZEUInt						TransparancyCullLimit;
		ZEVector4					MaterialColor;

		ZETexture2D*				Texture;
		ZETextureAddressMode		TextureAddressModeU;
		ZETextureAddressMode		TextureAddressModeV;

									ZEMaterialSimple();
		virtual						~ZEMaterialSimple();


		void						CreateShaders();
		void						ReleaseShaders();

	public:
		ZEUInt32					GetHash() const;
		ZEMaterialFlags				GetMaterialFlags() const;

		void						SetTwoSided(bool Enable);
		bool						GetTwoSided() const;

		void						SetWireframe(bool Enable);
		bool						GetWireframe() const;

		void						SetVertexColor(bool Enable);
		bool						GetVertexColor();

		void						SetMaterialColor(const ZEVector4& Color);
		const ZEVector4&			GetMaterialColor() const;

		void						SetTransparancyMode(ZEMaterialTransparancyMode Mode);
		ZEMaterialTransparancyMode	GetTransparancyMode() const;

		void						SetTransparancyCullLimit(ZEUInt Limit);
		ZEUInt						GetTransparancyCullLimit() const;

		void						SetTexture(ZETexture2D* Texture);
		ZETexture2D*				GetTexture() const;
		void						SetTextureAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode		GetTextureAddressModeU() const;
		void						SetTextureAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode		GetTextureAddressModeV() const;

		bool						SetupPass(ZEUInt PassId, const ZERenderStage* Stage, const ZERenderCommand* RenderCommand);

		void						UpdateMaterial();

		static ZEMaterialSimple*	CreateInstance();
};

#endif





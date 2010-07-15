//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9FixedMaterial.h
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
#ifndef __ZE_D3D9_FIXED_MATERIAL_H__
#define __ZE_D3D9_FIXED_MATERIAL_H__

#include <d3d9.h>
#include "ZED3D9ComponentBase.h"
#include "ZED3D9Material.h"
#include "ZEGraphics\ZEFixedMaterial.h"

class ZED3D9VertexShader;
class ZED3D9PixelShader;

class ZED3D9FixedMaterial : public ZEFixedMaterial, public ZED3D9Material, private ZED3D9ComponentBase
{
	friend class ZED3D9Module;
	private:
		ZERenderOrder*					RenderOrder;
		ZECamera*						Camera;

		void							SetTextureStage(unsigned int Id, ZETextureAddressMode AddressU, ZETextureAddressMode AddressV) const;
		void							SetTextureStage(unsigned int Id, ZETextureAddressMode AddressU, ZETextureAddressMode AddressV, ZETextureAddressMode AddressW) const;

		ZED3D9VertexShader*				PreZPassVertexShader;
		ZED3D9PixelShader*				PreZPassPixelShader;
		ZED3D9VertexShader*				GBufferPassVertexShader;
		ZED3D9PixelShader*				GBufferPassPixelShader;
		ZED3D9VertexShader*				ForwardPassVertexShader;
		ZED3D9PixelShader*				ForwardPassPixelShader;
		ZED3D9VertexShader*				ShadowPassVertexShader;
		ZED3D9PixelShader*				ShadowPassPixelShader;

		void							CreateShaders();
		void							ReleaseShaders();

	protected:
										ZED3D9FixedMaterial();
		virtual							~ZED3D9FixedMaterial();

	public:
		virtual const char*				GetMaterialUID() const;
		virtual unsigned int			GetMaterialFlags() const;
		virtual ZEMaterialType			GetMaterialType() const;

		const char*						ConvertToString(unsigned int MaterialComponent);

		virtual bool					SetupPreZPass() const;
		virtual bool					SetupGBufferPass() const;
		virtual bool					SetupForwardPass() const;
		virtual bool					SetupShadowPass() const;	

		virtual void					UpdateMaterial();

		virtual void					Release();
};
#endif

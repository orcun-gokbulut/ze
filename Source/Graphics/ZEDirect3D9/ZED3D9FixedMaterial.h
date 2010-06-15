//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - D3D9FixedMaterial.h
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
#include "D3D9ComponentBase.h"
#include "D3D9FixedMaterialShader.h"
#include "Graphics/FixedMaterial.h"

class ZED3D9FixedMaterial : public ZEFixedMaterial, private ZED3D9ComponentBase
{
	friend class ZED3D9Module;
	private:
		ZED3D9FixedMaterialShader*		Shader;
		ZERenderOrder*					RenderOrder;
		ZECamera*						Camera;
		void							SetShaderPass(ZED3D9FixedMateriaShaderPass* Pass, bool Skinned) const;
		void							SetTextureStage(unsigned int Id, ZETextureAddressMode AddressU, ZETextureAddressMode AddressV) const;
		void							SetTextureStage(unsigned int Id, ZETextureAddressMode AddressU, ZETextureAddressMode AddressV, ZETextureAddressMode AddressW) const;

	protected:
										ZED3D9FixedMaterial();
		virtual							~ZED3D9FixedMaterial();

	public:
		virtual const char*				GetMaterialUID() const;
		virtual unsigned int			GetMaterialFlags() const;
		virtual ZEMaterialType			GetMaterialType() const;

		const char*						ConvertToString(unsigned int MaterialComponent);

		virtual bool					SetupMaterial(ZERenderOrder* RenderOrder, ZECamera* Camera) const;

		virtual bool					SetupPreLightning() const;
		virtual size_t					DoPreLightningPass() const;

		virtual bool					SetupLightning() const;
		virtual bool					SetupPointLightPass(bool Shadowed) const;
		virtual size_t					DoPointLightPass(ZEPointLight** Lights, size_t Count) const;

		virtual bool					SetupDirectionalLightPass(bool Shadowed) const;
		virtual size_t					DoDirectionalLightPass(ZEDirectionalLight** Lights, size_t Count) const;

		virtual bool					SetupProjectiveLightPass(bool Shadowed) const;
		virtual size_t					DoProjectiveLightPass(ZEProjectiveLight** Lights, size_t Count) const;

		virtual bool					SetupOmniProjectiveLightPass(bool Shadowed) const;
		virtual size_t					DoOmniProjectivePass(ZEOmniProjectiveLight** Lights, size_t Count) const;

		virtual bool					SetupCustomPass(unsigned int CustomPassId) const;
		virtual bool					DoCustomPass(unsigned int CustomPassId, void* CustomData) const;

		virtual bool					SetupShadowPass() const;	
		virtual size_t					DoShadowPass() const;

		virtual void					EndOfPasses() const;

		virtual void					UpdateMaterial();

		virtual void					Release();
};
#endif

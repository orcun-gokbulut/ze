//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESeaMaterial.h
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
#ifndef	__ZE_SEA_MATERIAL_H__
#define __ZE_SEA_MATERIAL_H__

#include "ZEMaterial.h"
#include "ZEMath/ZEVector.h"

class ZETexture2D;

ZE_CLASS(ZESeaMaterial)

class ZESeaMaterial : public ZEMaterial
{
	friend class ZESea;
	friend class ZED3D9Module;

	ZE_OBJECT
	
	protected:

		const ZETexture2D*				DiffuseTexture;
		const ZETexture2D*				NormalTexture;

		bool							GlobalAmbientEnabled;
		ZEVector4						AmbientColor;
		float							AmbientFactor;

		ZEVector4						DiffuseColor;
		float							DiffuseFactor;

		ZEVector4						SpecularColor;
		float							SpecularFactor;

		float							SpecularShineness;

		ZEVector2						DiffuseTextureOffset;
		ZEVector2						NormalTextureOffset;

		ZEVector2						NormalTextureTile;
		ZEVector2						DiffuseTextureTile;

		ZEVector2						EntityXZScale;

										ZESeaMaterial();
		virtual							~ZESeaMaterial();

	public:

		ZEMaterialFlags					GetMaterialFlags() const;

		void							SetGlobalAmbientEnabled(bool Value);
		bool							GetGlobalAmbientEnabled() const;

		void							SetAmbientColor(const ZEVector4& Color);
		const ZEVector4&				GetAmbientColor() const;

		void							SetAmbientFactor(float Value);
		float							GetAmbientFactor() const;

		void							SetDiffuseColor(const ZEVector4& Color);
		const ZEVector4&				GetDiffuseColor() const;

		void							SetDiffuseFactor(float Color);
		float							GetDiffuseFactor() const;

		void							SetSpecularColor(const ZEVector4& Color);
		const ZEVector4&				GetSpecularColor() const;

		void							SetSpecularFactor(float Color);
		float							GetSpecularFactor() const;

		void							SetSpecularShineness(float Color);
		float							GetSpecularShineness() const;

		void							SetDiffuseTexture(const ZETexture2D* Texture);
		const ZETexture2D*				GetDiffuseTexture() const;

		void							SetNormalTexture(const ZETexture2D* Texture);
		const ZETexture2D*				GetNormalTexture() const;

		static ZESeaMaterial*			CreateInstance();
};

#endif

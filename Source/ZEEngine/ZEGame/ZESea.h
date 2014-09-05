//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESea.h
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
#ifndef	__ZE_SEA_H__
#define __ZE_SEA_H__

#include "ZEEntity.h"
#include "ZEGraphics/ZERenderCommand.h"

class ZESeaMaterial;
class ZEVertexDeclaration;
class ZEStaticVertexBuffer;
class ZETexture2D;

ZE_CLASS(ZESea)

class ZESea : public ZEEntity
{
	ZE_OBJECT

	private:

		ZEVertexDeclaration*	VertexDeclaration;
		ZESeaMaterial*			Material;

		ZEStaticVertexBuffer*	VertexBuffer;

		ZEVector2				DiffuseVelocity;
		ZEVector2				NormalVelocity;

		ZEVector2				DiffuseOffset;
		ZEVector2				NormalOffset;

		ZEVector2				NormalTextureTile;
		ZEVector2				DiffuseTextureTile;

		ZERenderCommand			Command;

	protected:

								ZESea();
		virtual					~ZESea();

	public:
		void					SetDiffuseTexture(const ZETexture2D* Texture);
		void					SetNormalTexture(const ZETexture2D* Texture);

		void					SetAmbientColor(const ZEVector3& Color);
		ZEVector3				GetAmbientColor() const;
		void					SetAmbientFactor(float Factor);
		float					GetAmbientFactor() const;

		void					SetDiffuseVelocity(const ZEVector2& Velocity);
		ZEVector2				GetDiffuseVelocity() const;
		void					SetDiffuseTile(const ZEVector2& Tile);
		ZEVector2				GetDiffuseTile() const;
		void					SetDiffuseColor(const ZEVector3& Color);
		ZEVector3				GetDiffuseColor() const;
		void					SetDiffuseFactor(float Factor);
		float					GetDiffuseFactor() const;

		void					SetSpecularFactor(float Factor);
		float					GetSpecularFactor() const;
		void					SetSpecularShineness(float Shineness);
		float					GetSpecularShineness() const;
		void					SetSpecularColor(const ZEVector3& Color);
		ZEVector3				GetSpecularColor() const;

		void					SetNormalVelocity(const ZEVector2& Velocity);
		ZEVector2				GetNormalVelocity() const;
		void					SetNormalTile(const ZEVector2& Tile);
		ZEVector2				GetNormalTile() const;

		virtual void			Tick(float ElapsedTime);
		virtual void			Draw(ZEDrawParameters* DrawParameters);

		virtual bool			RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters);

		virtual ZEDrawFlags		GetDrawFlags() const;

		static ZESea*			CreateInstance();
};

#endif

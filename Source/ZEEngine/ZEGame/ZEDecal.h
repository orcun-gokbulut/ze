//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDecal.h
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
#ifndef __ZE_DECAL_H__
#define __ZE_DECAL_H__

#include "ZETypes.h"
#include "ZEEntity.h"
#include "ZEMath/ZEAABBox.h"
#include "ZERenderer/ZERenderCommand.h"
#include "ZEGraphics/ZEIndexBuffer.h"
#include "ZEGraphics/ZEVertexBuffer.h"

class ZEDecalMaterial;
class ZESimpleMaterial;
class ZETexture2DResource;
class ZEVertexLayout;
class ZEStaticVertexBuffer;

class ZEDecal : public ZEEntity
{
	private:

	protected:
		bool					DrawHelperBox;
		ZEInt					Priority;

		bool					IsStatic;
		bool					LoopingEnabled;
		bool					FadeInEnabled;
		float					FadeInTime;
		float					FadeInLeftTime;
		bool					LifeTimeEnabled;
		bool					InfiniteLifeEnabled;
		float					LifeTime;
		float					LifeLeftTime;
		bool					FadeOutEnabled;
		float					FadeOutTime;
		float					FadeOutLeftTime;
		float					FadeAmount;
		
		ZETexture2DResource*	DiffuseMapResource;
		ZETexture2DResource*	NormalMapResource;
		ZETexture2DResource*	SpecularMapResource;
		ZETexture2DResource*	OpacityMapResource;
		ZETexture2DResource*	EmissiveMapResource;

		ZEDecalMaterial*		DecalMaterial;
		ZERenderCommand			DecalRenderCommand;
		ZEStaticVertexBuffer*	DecalVertexBuffer;
		ZEVertexLayout*	DecalVertexDeclaration;

		ZESimpleMaterial*		HelperBoxMaterial;
		ZERenderCommand			HelperBoxRenderCommand;
		ZEStaticVertexBuffer*	HelperBoxVertexBuffer;
		ZEVertexLayout*	HelperBoxVertexDeclaration;
		
		ZEMatrix4x4				DecalProjectionMatrix;

								ZEDecal();
		virtual					~ZEDecal();

		void					CreateBuffers();
		void					DestroyBuffers();
		void					CalculateProjectionParameters();

	public:
		void					SetDrawHelperBox(bool Value);
		bool					GetDrawHelperBox() const;

		void					SetPriority(ZEInt Value);
		ZEInt					GetPriority() const;

		void					SetIsStatic(bool Value);
		bool					GetIsStatic() const;

		void					SetLoopingEnabled(bool Value);
		bool					GetLoopingEnabled();

		void					SetFadeInEnabled(bool Value);
		bool					GetFadeInEnabled() const;

		void					SetFadeInTime(float Value);
		float					GetFadeInTime() const;

		void					SetFadeOutEnabled(bool Value);
		bool					GetFadeOutEnabled() const;

		void					SetFadeOutTime(float Value);
		float					GetFadeOutTime() const;

		void					SetLifeTimeEnabled(bool Value);
		bool					GetLifeTimeEnabled() const;

		void					SetInfiniteLifeEnabled(bool Value);
		bool					GetInfiniteLifeEnabled() const;
		
		void					SetLifeTime(float Value);
		float					GetLifeTime() const;

		float					GetFadeAmount() const;

		virtual void			SetDiffuseMapFile(const ZEString& FileName);
		const ZEString			GetDiffuseMapFile() const;

		virtual void			SetNormalMapFile(const ZEString& FileName);
		const ZEString			GetNormalMapFile() const;

		virtual void			SetSpecularMapFile(const ZEString& FileName);
		const ZEString			GetSpecularMapFile() const;

		virtual void			SetOpacityMapFile(const ZEString& FileName);
		const ZEString			GetOpacityMapFile() const;

		virtual void			SetEmissiveMapFile(const ZEString& FileName);
		const ZEString			GetEmissiveMapFile() const;

		ZEDecalMaterial*		GetDecalMaterial() const;

		virtual ZEDrawFlags		GetDrawFlags() const;

		virtual bool			Initialize();
		virtual void			Deinitialize();

		virtual void			Draw(ZEDrawParameters* DrawParameters);
		virtual void			Tick(float Time);

		static ZEDecal*			CreateInstance();
};

#endif

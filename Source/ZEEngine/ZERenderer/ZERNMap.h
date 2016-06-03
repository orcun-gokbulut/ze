//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNMap.h
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

#include "ZEMeta/ZEObject.h"

#include "ZEPointer/ZEHolder.h"
#include "ZEGraphics/ZEGRTexture.h"

class ZEGRTexture;
class ZEGRSampler;
class ZETextureResource;
class ZEMLWriterNode;
class ZEMLReaderNode;

class ZERNMap : public ZEObject
{
	ZE_OBJECT
	private:
		ZETextureResource*				Resource;
		const ZEGRTexture*				Texture;
		const ZEGRSampler*				Sampler;

		void							Clean();

	public:
		void							SetSampler(const ZEGRSampler* Sampler);
		const ZEGRSampler*				GetSampler() const;

		void							SetTexture(const ZEGRTexture* Texture);
		const ZEGRTexture*				GetTexture() const;

		void							SetTextureResource(ZETextureResource* Resource);
		const ZETextureResource*		GetTextureResource() const;

		const ZEString&					GetTextureFile() const;

		bool							IsAvailable() const;

		void							Load2D(const ZEString& FileName, bool sRGB = true);
		void							Load3D(const ZEString& FileName, ZEUInt HorizontalTileCount, ZEUInt VerticalTileCount);
		void							LoadCube(const ZEString& FileName);

		void							Write(ZEMLWriterNode& ParentNode, const ZEString& Name);
		void							Read(ZEMLReaderNode& ParentNode, const ZEString& Name, bool sRGB = true);

										ZERNMap();
										ZERNMap(const ZEString& FileName, ZEGRTextureType Type = ZEGR_TT_2D, ZEGRSampler* Sampler = NULL);
										ZERNMap(ZEGRTexture* Texture, ZEGRSampler* Sampler = NULL);
										ZERNMap(ZETextureResource* Resource, ZEGRSampler* Sampler = NULL);
										~ZERNMap();
};

//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUIFontTrueType.h
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

#include "ZEDS\ZEArray.h"
#include "ZEMath\ZEVector.h"
#include "ZEPointer\ZEHolder.h"

#include "ZEUI\ZEUIFont.h"
#include "ZEUI\ZEUIRectangle.h"
#include "ZEGraphics\ZEGRTexture2D.h"

struct ZEFreeType;

class ZEUIFontTrueType : public ZEUIFont
{
	ZE_OBJECT
	private:
		ZEFreeType*							FreeType;

		ZEUInt32							HorizontalOutputDPI;
		ZEUInt32							VerticalOutputDPI;
		ZEInt32								PointFactor;

		ZEString							FontFile;
		ZEUInt32							FontSize;
		bool								FontSupportsKerning;

		ZEArray<ZEUIFontCharacter>			FontCharacters;
		ZEVector2							LastCharacterPosition;

		ZEUInt32							LastTextureId;
		ZEUInt32							LastTextureLine;

		ZEArray<ZEHolder<ZEGRTexture2D>>	Textures;

		void								SetFontFile(ZEString FontFilePath);
		void								SetFontSize(ZEUInt32 FontSize);
		void								CreateNewTexture(ZEUInt32 Width, ZEUInt32 Height);

											ZEUIFontTrueType();
											~ZEUIFontTrueType();

	public:
		virtual const char*					GetResourceType() const;
		virtual ZEUIFontType			GetFontResourceType() const;

		ZEUInt32							GetFontSize() const;

		virtual const ZEUIFontCharacter&		GetCharacter(char Character);
		virtual const ZEUIFontCharacter&		GetCharacter(char CurrentChar, char NextChar, ZEInt64& KerningDistance);

		const ZEGRTexture2D*				GetTexture(ZEUInt32 TextureId);

		static ZEUIFontTrueType*		LoadSharedResource(const ZEString& FileName, ZEUInt32 FontSize);
		static void							CacheResource(const ZEString& FileName, ZEUInt32 FontSize);

		static ZEUIFontTrueType*		LoadResource(const ZEString& FileName, ZEUInt32 FontSize);
		static ZEUIFontTrueType*		LoadResource(ZEFile* ResourceFile, ZEUInt32 FontSize);

};

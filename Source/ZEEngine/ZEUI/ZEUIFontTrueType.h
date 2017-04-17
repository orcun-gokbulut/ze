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

#include "ZEUI\ZEUIFont.h"

#include "ZEDS\ZEArray.h"
#include "ZEMath\ZEVector.h"
#include "ZEPointer\ZEHolder.h"
#include "ZEUI\ZEUIRectangle.h"
#include "ZEGraphics\ZEGRTexture.h"
#include "ZEExport.ZEEngine.h"

struct ZEFreeType;
class ZEUIFontTrueType;

class ZE_EXPORT_ZEENGINE ZEUIFontTrueTypeIdentifier : public ZERSResourceIdentifier
{
	friend class ZEUIFontTrueType;
	private:
		ZEUIFontTrueType*					Font;

											ZEUIFontTrueTypeIdentifier(ZEUIFontTrueType* Font);
	public:
		virtual bool						Equals(const ZERSResourceIdentifier* Identifier) const;
		virtual ZEString					ToString() const;
};

class ZE_EXPORT_ZEENGINE ZEUIFontTrueType : public ZEUIFont
{
	ZE_OBJECT
	ZE_DISALLOW_COPY(ZEUIFontTrueType)
	friend class ZERSTemplates;
	private:
		ZEUIFontTrueTypeIdentifier					Identifier;

		ZEFreeType*									FreeType;

		ZEUInt32									HorizontalOutputDPI;
		ZEUInt32									VerticalOutputDPI;
		ZEInt32										PointFactor;

		ZEString									FontFile;
		ZEUInt32									FontSize;
		bool										FontSupportsKerning;

		mutable ZEArray<ZEUIFontCharacter>			FontCharacters;
		mutable ZEVector2							LastCharacterPosition;

		mutable ZEUInt32							LastTextureId;
		mutable ZEUInt32							LastTextureLine;

		mutable ZEArray<ZEHolder<ZEGRTexture>>		Textures;

		mutable ZELock								Lock;

		void										SetFontFile(ZEString FontFilePath);
		void										SetFontSize(ZEUInt32 FontSize);
		void										CreateNewTexture(ZEUInt32 Width, ZEUInt32 Height) const;

		virtual ZETaskResult						LoadInternal();
		virtual ZETaskResult						UnloadInternal();

													ZEUIFontTrueType();
		virtual										~ZEUIFontTrueType();

		static ZERSResource*						Instanciator(const void* Parameters);

	public:
		virtual ZEUIFontType						GetFontResourceType() const;
		ZEUInt32									GetFontSize() const;

		virtual ZEUIFontCharacter					GetCharacter(char Character) const;
		virtual ZEUIFontCharacter					GetCharacter(char CurrentChar, char NextChar, ZEInt64& KerningDistance) const;

		static ZEHolder<ZEUIFontTrueType>			LoadResource(const ZEString& FileName, ZEUInt32 FontSize);
		static ZEHolder<const ZEUIFontTrueType>		LoadResourceShared(const ZEString& FileName, ZEUInt32 FontSize);

};

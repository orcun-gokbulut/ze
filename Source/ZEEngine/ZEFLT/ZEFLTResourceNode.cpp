//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFLTResourceNode.cpp
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

#include "ZEFLTResourceNode.h"

ZEFLTResourceNode::ZEFLTResourceNode()
{
	Parent = NULL;
	Record = NULL;
}

ZEFLTResourceNode::~ZEFLTResourceNode()
{
	this->Destroy();
}

void ZEFLTResourceNode::SetParent(ZEFLTResourceNode* Parent)
{
	this->Parent = Parent;
}

ZEFLTResourceNode* ZEFLTResourceNode::GetParent() const
{
	return Parent;
}

void ZEFLTResourceNode::RegisterChild(ZEFLTResourceNode* Child)
{
	Children.Add(Child);
	Child->SetParent(this);
}

void ZEFLTResourceNode::UnregisterChild(ZEFLTResourceNode* Child)
{
	Child->SetParent(NULL);
	Children.DeleteValue(Child);
}

ZEArray<ZEFLTResourceNode*>& ZEFLTResourceNode::GetChildren()
{
	return Children;
}

void ZEFLTResourceNode::RegisterAncillary(ZEFLTResourceNode* Ancillary)
{
	Ancillaries.Add(Ancillary);
}

void ZEFLTResourceNode::UnregisterAncillary(ZEFLTResourceNode* Ancillary)
{
	Ancillaries.DeleteValue(Ancillary);
}

ZEArray<ZEFLTResourceNode*>& ZEFLTResourceNode::GetAncillaries()
{
	return Ancillaries;
}

void ZEFLTResourceNode::SetRecord(ZEFLTRecordBase* Record)
{
	this->Record = Record;
}

ZEFLTRecordBase* ZEFLTResourceNode::GetRecord()
{
	return Record;
}

void ZEFLTResourceNode::Destroy()
{
	for (ZESize I = 0; I < Children.GetCount(); I++)
	{
		Children[I]->Destroy();
	}

	for (ZESize I = 0; I < Ancillaries.GetCount(); I++)
	{
		Ancillaries[I]->Destroy();
	}

	delete this;
}

ZEFLTResourceNode* ZEFLTResourceNode::CreateInstance()
{
	return new ZEFLTResourceNode();
}

void ZEFLTResourceNodeHeader::SetColorPalette(ZEFLTRecordPaletteColor* ColorPalette)
{
	this->ColorPalette = ColorPalette;
}

ZEFLTRecordPaletteColor* ZEFLTResourceNodeHeader::GetColorPalette()
{
	return ColorPalette;
}

ZEInt32 ZEFLTResourceNodeHeader::GetColor(ZESize Index)
{
	return ColorPalette->BrightestRGBOfColors[Index];
}

ZEInt32 ZEFLTResourceNodeHeader::GetColor(const char* ColorName)
{
	for (ZESize I = 0; I < ColorPalette->NumberofColorNames; I++)
	{
		if (strcmp(ColorPalette->ColorNames[I].ColorName, ColorName) == 0)
			return ColorPalette->BrightestRGBOfColors[ColorPalette->ColorNames[I].ColorIndex];
	}

	return 0;
}

void ZEFLTResourceNodeHeader::RegisterStandardMaterial(ZEFLTRecordPaletteMaterial* Material)
{
	MaterialPalette.Add(Material);
}

void ZEFLTResourceNodeHeader::UnregisterStandardMaterial(ZEFLTRecordPaletteMaterial* Material)
{
	MaterialPalette.DeleteValue(Material);
}

ZEArray<ZEFLTRecordPaletteMaterial*>& ZEFLTResourceNodeHeader::GetStandardMaterialPalette()
{
	return MaterialPalette;
}

ZEFLTRecordPaletteMaterial* ZEFLTResourceNodeHeader::GetStandardMaterial(ZESize Index)
{
	return MaterialPalette.GetItem(Index);
}

void ZEFLTResourceNodeHeader::RegisterTexture(ZEFLTRecordPaletteTexture* Texture)
{
	TexturePalette.Add(Texture);
}

void ZEFLTResourceNodeHeader::UnregisterTexture(ZEFLTRecordPaletteTexture* Texture)
{
	TexturePalette.DeleteValue(Texture);
}

ZEArray<ZEFLTRecordPaletteTexture*>& ZEFLTResourceNodeHeader::GetTexturePalette()
{
	return TexturePalette;
}

ZEFLTRecordPaletteTexture* ZEFLTResourceNodeHeader::GetTexture(ZESize Index)
{
	return TexturePalette.GetItem(Index);
}

ZEFLTResourceNodeHeader::ZEFLTResourceNodeHeader()
{
	ColorPalette = NULL;
}

ZEFLTResourceNodeHeader::~ZEFLTResourceNodeHeader()
{

}

ZEFLTResourceNodeHeader* ZEFLTResourceNodeHeader::CreateInstance()
{
	return new ZEFLTResourceNodeHeader();
}

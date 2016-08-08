//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMDResourceDraw.cpp
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

#include "ZEMDResourceDraw.h"

#include "ZEError.h"
#include "ZEML/ZEMLReader.h"
#include "ZEModelMeshLOD.h"
#include "ZEMDResourceLOD.h"
#include "ZERenderer/ZERNMaterial.h"

void ZEMDResourceDraw::SetOffset(ZESize PoligonOffset)
{
	this->Offset = PoligonOffset;
}

ZESize ZEMDResourceDraw::GetOffset() const 
{
	return Offset;
}

void ZEMDResourceDraw::SetCount(ZESize PoligonCount)
{
	this->Count = PoligonCount;
}

ZESize ZEMDResourceDraw::GetCount() const 
{
	return Count;
}

void ZEMDResourceDraw::SetMaterial(ZEHolder<const ZERNMaterial> Material)
{
	this->Material = Material;
}

ZEHolder<const ZERNMaterial> ZEMDResourceDraw::GetMaterial() const 
{
	return Material;
}

bool ZEMDResourceDraw::Unserialize(ZEMLReaderNode& DrawNode)
{
	zeCheckError(!DrawNode.IsValid(), false, "Invalid Draw node.");
	zeCheckError(DrawNode.GetName() != "Draw", false, "Invalid Draw node name.");

	SetOffset(DrawNode.ReadUInt32("Offset", 0));
	SetCount(DrawNode.ReadUInt32("Count", 0));

	return true;
}

bool ZEMDResourceDraw::Serialize(ZEMLWriterNode& DrawNode) const
{
	return false;
}

ZEMDResourceDraw::ZEMDResourceDraw()
{
	Offset = 0;
	Count = 0;
}

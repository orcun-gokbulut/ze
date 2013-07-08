//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETerrain.cpp
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

#include "ZETerrain.h"
#include "ZEGraphics\ZESimpleMaterial.h"
#include "ZEGame\ZEDrawParameters.h"
#include "ZETerrainLayer.h"

bool ZETerrain2::InitializeSelf()
{
	if (!ZEEntity::InitializeSelf())
		return false;

	if (!Drawer.Initialize())
	{
		zeError("Can not initialize.");
		return false;
	}

	ZESimpleMaterial* Material = ZESimpleMaterial::CreateInstance();
	Material->SetMaterialColor(ZEVector4::One);
	Material->SetVertexColor(false);
	Material->SetWireframe(true);
	Drawer.SetMaterial(Material);

	return true;
}

bool ZETerrain2::DeinitializeSelf()
{
	Drawer.Deinitialize();
	return false;
}

ZETerrain2::ZETerrain2()
{

}

ZETerrain2::~ZETerrain2()
{
	Deinitialize();
	for (ZESize I = 0; I < Layers.GetCount(); I++)
		delete Layers[I];
}

ZEDrawFlags ZETerrain2::GetDrawFlags() const
{
	return ZE_DF_DRAW;
}

ZETerrainDrawer& ZETerrain2::GetDrawer()
{
	return Drawer;
}

const ZEArray<ZETerrainLayer*>&	ZETerrain2::GetLayers()
{
	return Layers;
}

void ZETerrain2::AddLayer(ZETerrainLayer* Layer)
{
	if (Layers.Exists(Layer))
		return;

	Layers.Add(Layer);

	if (GetState() == ZE_ES_INITIALIZED)
		Layer->Initialize();	
}

void ZETerrain2::RemoveLayer(ZETerrainLayer* Layer)
{
	if (!Layers.Exists(Layer))
		return;

	if (GetState() == ZE_ES_INITIALIZED)
		Layer->Deinitialize();

	Layers.DeleteValue(Layer);
}

void ZETerrain2::SetPrimitiveSize(ZEUInt Size)
{
	Drawer.SetPrimitiveSize(Size);
}

ZEUInt ZETerrain2::GetPrimitiveSize()
{
	return Drawer.GetPrimitiveSize();
}

void ZETerrain2::Draw(ZEDrawParameters* DrawParameters)
{
	for (ZESize I = 0; I < Layers.GetCount(); I++)
		Layers[I]->Stream(DrawParameters->View->Position.x, DrawParameters->View->Position.y, 0, 6);

	Drawer.Draw(DrawParameters);
}

ZETerrain2* ZETerrain2::CreateInstance()
{
	return new ZETerrain2();
}

ZEEntityRunAt ZETerrain2Description::GetRunAt() const
{
	return ZE_ERA_BOTH;
}

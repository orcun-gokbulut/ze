//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETRTerrain.cpp
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

#include "ZETRTerrain.h"
#include "ZETRLayer.h"
#include "ZEGraphics\ZECamera.h"

#include "ZEGame\ZEDrawParameters.h"
#include "ZEGraphics\ZESimpleMaterial.h"
#include "ZEError.h"
#include "ZEGraphics\ZETerrainMaterial.h"

bool ZETRTerrain::InitializeSelf()
{
	if (!ZEEntity::InitializeSelf())
		return false;

	Drawer.SetTerrain(this);
	if (!Drawer.Initialize())
	{
		zeError("Can not initialize.");
		return false;
	}

	ZESimpleMaterial* Material = ZESimpleMaterial::CreateInstance();
	Material->SetWireframe(true);
	Material->SetMaterialColor(ZEVector4::One);
	Drawer.SetMaterial((ZETerrainMaterial*)Material);

	for (ZESize I = 0; I < Layers.GetCount(); I++)
	{
		if (!Layers[I]->Initialize())
		{
			zeError("Cannot initialize terrain layer. Layer Name: \"%s\".", Layers[I]->GetName().ToCString());
			return false;
		}
	}

	return true;
}

bool ZETRTerrain::DeinitializeSelf()
{
	Drawer.Deinitialize();
	return false;
}

ZETRTerrain::ZETRTerrain()
{
	BlockSize = 1024;
}

ZETRTerrain::~ZETRTerrain()
{
	Deinitialize();
	for (ZESize I = 0; I < Layers.GetCount(); I++)
		delete Layers[I];
}

ZEDrawFlags ZETRTerrain::GetDrawFlags() const
{
	return ZE_DF_DRAW;
}

ZETRDrawer& ZETRTerrain::GetDrawer()
{
	return Drawer;
}

const ZEArray<ZETRLayer*>&	ZETRTerrain::GetLayers()
{
	return Layers;
}

bool ZETRTerrain::AddLayer(ZETRLayer* Layer)
{
	if (Layers.Exists(Layer))
	{
		zeError("Layer is already registered.");
		return false;
	}

	Layers.Add(Layer);

	if (IsInitialized())
	{
		if (!Layer->Initialize())
		{
			zeError("Cannot initialize terrain layer. Layer Name: \"%s\".", Layer->GetName().ToCString());
			return false;
		}
	}

	return true;
}

void ZETRTerrain::RemoveLayer(ZETRLayer* Layer)
{
	if (IsInitialized())
		Layer->Deinitialize();

	Layers.RemoveValue(Layer);
}

void ZETRTerrain::SetBlockSize(ZESize Size)
{
	BlockSize = Size;
}

ZESize ZETRTerrain::GetBlockSize()
{
	return BlockSize;
}

void ZETRTerrain::SetPrimitiveSize(ZEUInt Size)
{
	Drawer.SetPrimitiveSize(Size);
}

ZEUInt ZETRTerrain::GetPrimitiveSize()
{
	return Drawer.GetPrimitiveSize();
}

void ZETRTerrain::Draw(ZEDrawParameters* DrawParameters)
{
	for (ZESize I = 0; I < Layers.GetCount(); I++)
	{
		ZETRLayer* CurrentLayer = Layers[I];
		
		if (!CurrentLayer->IsInitialized())
			continue;
		
		CurrentLayer->SetViewPosition(DrawParameters->View->Camera->GetWorldPosition());
		CurrentLayer->Process();
	}

	Drawer.Draw(DrawParameters);
}

ZETRTerrain* ZETRTerrain::CreateInstance()
{
	return new ZETRTerrain();
}

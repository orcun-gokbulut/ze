//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEToolComponentsMain.cpp
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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "ZEFile\ZEPathUtils.h"

#include <QtGui/QApplication>
#include "ZETypes.h"
#include "ZEProgressDialog/ZEProgressDialog.h"
#include "ZEProgressDialog/ZEProgressDialogTask.h"
#include "ZEResourceConfigurationWidget/ZEResourceConfigurationWidget.h"
#include "ZEMLVisualizer/ZEMLVisualizerWidget.h"
#include "ZEFile/ZEFile.h"
#include "ZEML/ZEMLNode.h"

ZEInt __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, ZEInt nCmdShow)
{	
	ZEInt argc = 0;
	char** argv = NULL;

	QApplication a(argc, argv);

 	ZEMLVisualizerWidget w;
	ZEFile File;
	
	File.Open("C:\\Users\\Can\\Desktop\\ZE\\branches\\v0.5.5-NewTools\\RunDir\\resources\\ZEMapTest\\test.ZEMAP", ZE_FOM_READ, ZE_FCM_NONE);
	//File.Open("C:\\Users\\Can\\Desktop\\asd.ZEMODEL", ZE_FOM_READ, ZE_FCM_NONE);
	//File.Open("C:\\Users\\Can\\Desktop\\Material #48.ZEMaterial", ZE_FOM_READ, ZE_FCM_NONE);
	//File.Open("C:\\Users\\Can\\Desktop\\Preset.zeml", ZE_FOM_READ, ZE_FCM_NONE);
	//File.Open("C:\\Users\\Can\\Desktop\\ExpTest.max.zecfg", ZE_FOM_READ, ZE_FCM_NONE);
	ZEMLNode* Node = new ZEMLNode("ZEMap");
	Node->Read(&File);
	w.SetZEMLNode(Node);
	w.Show();

// 	ZEResourceConfigurationWidget w;
// 	w.AddResource("Material1", "C:\\", "D:\\", ZE_ROAA_COPY_OVERWRITE, true);
// 	w.AddResource("Material2", "C:\\", "D:\\", ZE_ROAA_COPY_OVERWRITE, false);
// 	w.AddResource("Material2", "C:\\", "D:\\", ZE_ROAA_COPY_OVERWRITE, true);
// 	w.AddResource("Material2", "C:\\", "D:\\", ZE_ROAA_COPY_OVERWRITE, false);
// 
// 	w.Show();

 	a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
	return a.exec();
}

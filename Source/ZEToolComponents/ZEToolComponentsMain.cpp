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

#include <QtGui/QApplication>
#include "ZETypes.h"
#include "ZEProgressDialog/ZEProgressDialog.h"
#include "ZEProgressDialog/ZEProgressDialogTask.h"
#include "ZEResourceConfigurationWidget/ZEResourceConfigurationWidget.h"

ZEInt __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, ZEInt nCmdShow)
{
	ZEInt argc = 0;
	char** argv = NULL;

	QApplication a(argc, argv);

 	ZEResourceConfigurationWidget w;
	ZEResourceConfigurationWidget z;
	
	w.AddResource("Material-01.zemtl", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	w.AddResource("Material-02.zemtl","C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	w.AddResource("Material-03.zemtl","C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	w.AddResource("nvidia_ssao.jpg", "C:\\Users\\Can\\Desktop\\Test1.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	w.AddResource("Test2.png", "C:\\Users\\Can\\Desktop\\Test2.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	w.AddResource("Test3.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	w.AddResource("Test4.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	w.AddResource("Test5.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	w.AddResource("Test6.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	w.AddResource("Test7.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	w.AddResource("Test8.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	w.AddResource("Test9.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	w.AddResource("Test10.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	w.AddResource("Test11.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	w.AddResource("Test12.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	w.AddResource("Test13.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	w.AddResource("Test14.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	w.AddResource("Test15.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	w.AddResource("Test16.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	w.AddResource("Test17.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	w.AddResource("Test18.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	w.AddResource("Test19.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	w.AddResource("Test20.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	w.AddResource("Test21.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	w.AddResource("Test22.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	w.AddResource("Test23.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	w.Show();
	w.SavePresets("C:\\Users\\Can\\Desktop\\Preset.zeml");

	z.SetPresetFilePath("C:\\Users\\Can\\Desktop\\Preset.zeml");
	z.AddResource("Material-01.zemtl", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	z.AddResource("Material-02.zemtl","C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	z.AddResource("Material-03.zemtl","C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	z.AddResource("nvidia_ssao.jpg", "C:\\Users\\Can\\Desktop\\Test1.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	z.AddResource("Test2.png", "C:\\Users\\Can\\Desktop\\Test2.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	z.AddResource("Test3.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	z.AddResource("Test4.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	z.AddResource("Test5.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	z.AddResource("Test6.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	z.AddResource("Test7.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	z.AddResource("Test8.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	z.AddResource("Test9.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	z.AddResource("Test10.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	z.AddResource("Test11.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	z.AddResource("Test12.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	z.AddResource("Test13.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	z.AddResource("Test14.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	z.AddResource("Test15.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	z.AddResource("Test16.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	z.AddResource("Test17.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	z.AddResource("Test18.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	z.AddResource("Test19.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	z.AddResource("Test20.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	z.AddResource("Test21.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	z.AddResource("Test22.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	z.AddResource("Test23.png", "C:\\Users\\Can\\Desktop\\Test3.png", "C:\\Users\\Can\\Desktop\\ZE\\trunk\\Rundir\\resources", ZE_ROAA_COPY_OVERWRITE);
	z.Show();

 	a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
	return a.exec();
}

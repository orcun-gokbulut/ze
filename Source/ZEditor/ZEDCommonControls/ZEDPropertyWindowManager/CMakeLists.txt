#ZE_SOURCE_PROCESSOR_START(License, 1.0)
#[[*****************************************************************************
 Zinek Engine - CMakeLists.txt
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
*****************************************************************************]]
#ZE_SOURCE_PROCESSOR_END()

cmake_minimum_required (VERSION 2.8)

ze_add_source(ZEDBoolComboBox.cpp			Sources)
ze_add_source(ZEDBoolComboBox.h				Sources QtMocs)
ze_add_source(ZEDColorLabel.cpp				Sources)
ze_add_source(ZEDColorLabel.h				Sources)
ze_add_source(ZEDColorPickerRGB.cpp			Sources)
ze_add_source(ZEDColorPickerRGB.h			Sources QtMocs)
ze_add_source(ZEDColorPickerRGBA.cpp		Sources)
ze_add_source(ZEDColorPickerRGBA.h			Sources QtMocs)
ze_add_source(ZEDEnumComboBox.cpp			Sources)
ze_add_source(ZEDEnumComboBox.h				Sources QtMocs)
ze_add_source(ZEDFileBrowser.cpp			Sources)
ze_add_source(ZEDFileBrowser.h				Sources QtMocs)
ze_add_source(ZEDFloatIntLineEdit.cpp		Sources)
ze_add_source(ZEDFloatIntLineEdit.h			Sources QtMocs)
ze_add_source(ZEDLineEdit.cpp				Sources)
ze_add_source(ZEDLineEdit.h					Sources QtMocs)
ze_add_source(ZEDPropertyUndoRedo.cpp		Sources)
ze_add_source(ZEDPropertyUndoRedo.h			Sources)
ze_add_source(ZEDPropertyWidget.cpp			Sources)
ze_add_source(ZEDPropertyWidget.h			Sources)
ze_add_source(ZEDPropertyWindowManager.cpp	Sources)
ze_add_source(ZEDPropertyWindowManager.h	Sources QtMocs)
ze_add_source(ZEDRotationSpinBox3.cpp		Sources)
ze_add_source(ZEDRotationSpinBox3.h			Sources QtMocs)
ze_add_source(ZEDSpinBox1Float.cpp			Sources)
ze_add_source(ZEDSpinBox1Float.h			Sources QtMocs)
ze_add_source(ZEDSpinBox1Integer.cpp		Sources)
ze_add_source(ZEDSpinBox1Integer.h			Sources QtMocs)
ze_add_source(ZEDSpinBox2.cpp				Sources)
ze_add_source(ZEDSpinBox2.h					Sources QtMocs)
ze_add_source(ZEDSpinBox3.cpp				Sources)
ze_add_source(ZEDSpinBox3.h					Sources QtMocs)
ze_add_source(ZEDSpinBox4.cpp				Sources)
ze_add_source(ZEDSpinBox4.h					Sources QtMocs)

qt4_add_resources (QtResourceFiles ${QtResources})
qt4_wrap_ui (QtUIFiles ${QtUI})
qt4_wrap_cpp (QtMocFiles  ${QtMocs})

ze_add_library(ZEDPropertyWindowManager 
	SOURCES ${Sources} ${QtMocFiles} ${QtUIFiles} ${QtResourceFiles}
	LIBS libQt)

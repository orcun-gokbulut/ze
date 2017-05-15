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

include_directories(${CMAKE_CURRENT_BINARY_DIR})

ze_add_source(ZEExport.ZECRCrashReporter.cpp				Sources)
ze_add_source(ZEExport.ZECRCrashReporter.h					Sources ZEMC)

ze_add_source(ZECRReport.cpp								Sources)
ze_add_source(ZECRReport.h									Sources ZEMC)
ze_add_source(ZECRReportParameters.cpp						Sources)
ze_add_source(ZECRReportParameters.h						Sources)
ze_add_source(ZECRCollector.cpp								Sources)
ze_add_source(ZECRCollector.h								Sources ZEMC)
ze_add_source(ZECRCollectorFile.cpp							Sources)
ze_add_source(ZECRCollectorFile.h							Sources ZEMC)
ze_add_source(ZECRCollectorUserFeedback.cpp					Sources)
ze_add_source(ZECRCollectorUserFeedback.h					Sources ZEMC)
ze_add_source(ZECRCollectorProductInfo.cpp					Sources)
ze_add_source(ZECRCollectorProductInfo.h					Sources ZEMC)
ze_add_source(ZECRCollectorSystemInformation.cpp			Sources)
ze_add_source(ZECRCollectorSystemInformation.h				Sources ZEMC)
ze_add_source(ZECRCollectorMemoryDump.cpp					Sources)
ze_add_source(ZECRCollectorMemoryDump.h						Sources ZEMC)
ze_add_source(ZECRSender.cpp 								Sources)
ze_add_source(ZECRSender.h									Sources ZEMC)

ze_add_source(ZECRWindow.cpp								Sources)
ze_add_source(ZECRWindow.h									Sources QtMocs)
ze_add_source(ZECRWindow.ui									Sources QtUI)
ze_add_source(ZECRWindowPage.cpp							Sources)
ze_add_source(ZECRWindowPage.h								Sources)
ze_add_source(ZECRWindowGeneratingReport.cpp				Sources)
ze_add_source(ZECRWindowGeneratingReport.h					Sources QtMocs)
ze_add_source(ZECRWindowGeneratingReport.ui					Sources QtUI)
ze_add_source(ZECRWindowInformation.cpp						Sources)
ze_add_source(ZECRWindowInformation.h						Sources QtMocs)
ze_add_source(ZECRWindowInformation.ui						Sources QtUI)
ze_add_source(ZECRWindowUserFeedback.cpp					Sources)
ze_add_source(ZECRWindowUserFeedback.h						Sources QtMocs)
ze_add_source(ZECRWindowUserFeedback.ui						Sources QtUI)
ze_add_source(ZECRWindowViewPrivacyPolicy.cpp 				Sources)
ze_add_source(ZECRWindowViewPrivacyPolicy.h 				Sources QtMocs)
ze_add_source(ZECRWindowViewPrivacyPolicy.ui 				Sources QtUI)
ze_add_source(ZECRWindowTransfering.cpp						Sources)
ze_add_source(ZECRWindowTransfering.h 						Sources QtMocs)
ze_add_source(ZECRWindowTransfering.ui 						Sources QtUI)
ze_add_source(ZECRWindowTransferCompleted.cpp 				Sources)
ze_add_source(ZECRWindowTransferCompleted.h 				Sources QtMocs)
ze_add_source(ZECRWindowTransferCompleted.ui 				Sources QtUI)

ze_add_source(ZECRReportViewer.cpp							Sources)
ze_add_source(ZECRReportViewer.h 							Sources QtMocs)
ze_add_source(ZECRReportViewer.ui 							Sources QtUI)

ze_add_source(ZECRResources.qrc								Sources QtResources)
ze_add_source(ZECRMain.cpp									Sources)

qt5_add_resources(QtResourceFiles ${QtResources})
qt5_wrap_ui(QtUIFiles ${QtUI})
qt5_wrap_cpp(QtMocFiles  ${QtMocs})

source_group("Generated" FILES ${QtResourceFiles} ${QtMocFiles} ${QtUIFiles})

ze_add_library(TARGET ZECRCrashReporter DLL
	SOURCES ${Sources} ${QtMocFiles} ${QtUIFiles} ${QtResourceFiles}
	ZEMC ${ZEMC}
	LIBS ZEFoundation libTinyXML libZLib libCurl Ws2_32)

qt5_use_modules(ZECRCrashReporter Widgets)

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

ze_add_source(ZECrashReport.cpp										Sources)
ze_add_source(ZECrashReport.h										Sources)
ze_add_source(ZECrashReportProvider.cpp								Sources)
ze_add_source(ZECrashReportProvider.h								Sources)
ze_add_source(ZECrashReportPackager.cpp								Sources)
ze_add_source(ZECrashReportPackager.h								Sources)
ze_add_source(ZECrashReportSender.cpp 								Sources)
ze_add_source(ZECrashReportSender.h									Sources)
ze_add_source(ZECrashReportApplicationInformationProvider.cpp		Sources)
ze_add_source(ZECrashReportApplicationInformationProvider.h			Sources)
ze_add_source(ZECrashReportSystemInformationProvider.cpp			Sources)
ze_add_source(ZECrashReportSystemInformationProvider.h				Sources)
ze_add_source(ZECrashReportDumpProvider.cpp							Sources)
ze_add_source(ZECrashReportDumpProvider.h							Sources)
ze_add_source(ZECIM.h 												Sources)
ze_add_source(ZECIM.cpp												Sources)
ze_add_source(ZECrashReportFileProvider.cpp							Sources)
ze_add_source(ZECrashReportFileProvider.h							Sources)
ze_add_source(ZECrashReportUserCommentProvider.cpp					Sources)
ze_add_source(ZECrashReportUserCommentProvider.h					Sources)

ze_add_source(ZECrashReportUIClickableLabel.h						Sources QtMocs)
ze_add_source(ZECrashReportUIClickableLabel.cpp						Sources)
ze_add_source(ZECrashReportUIMainWindow.h							Sources QtMocs)
ze_add_source(ZECrashReportUIMainWindow.cpp							Sources)
ze_add_source(CrashMainWindow.ui									Sources QtUI)
ze_add_source(ZECrashReportUIPrivacyDialog.h 						Sources QtMocs)
ze_add_source(ZECrashReportUIPrivacyDialog.cpp 						Sources)
ze_add_source(PrivacyDialog.ui 										Sources QtUI)
ze_add_source(ZECrashReportUIProgressWindow.h 						Sources QtMocs)
ze_add_source(ZECrashReportUIProgressWindow.cpp 					Sources)
ze_add_source(ProgressWindow.ui 									Sources QtUI)
ze_add_source(ZECrashReportUIReportDialog.h 						Sources QtMocs)
ze_add_source(ZECrashReportUIReportDialog.cpp 						Sources)
ze_add_source(ReportDialog.ui 										Sources QtUI)
ze_add_source(ZECrashReportUploadThread.h							Sources QtMocs)
ze_add_source(ZECrashReportUploadThread.cpp							Sources)
ze_add_source(ZECrashReportUICompletedWindow.h						Sources QtMocs)
ze_add_source(ZECrashReportUICompletedWindow.cpp					Sources)
ze_add_source(CompletedWindow.ui									Sources QtUI)
ze_add_source(zecrashreportui.qrc									Sources QtResources)
ze_add_source(ZECrashReportUIMain.cpp								Sources)

qt4_add_resources (QtResourceFiles ${QtResources})
qt4_wrap_ui (QtUIFiles ${QtUI})
qt4_wrap_cpp (QtMocFiles  ${QtMocs})

ze_add_executable(ZECrashReport
	SOURCES ${Sources} ${QtMocFiles} ${QtUIFiles} ${QtResourceFiles}
	LIBS ZEFoundation libQt libTinyXML libZLib libCurl Ws2_32)

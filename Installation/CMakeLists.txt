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


set(ZEBUILD_BUILD_INSTALLER		TRUE CACHE BOOL "Build Installer.")

if (ZEBUILD_BUILD_INSTALLER)
	install(DIRECTORY Dll/${ZEBUILD_PLATFORM}/
		DESTINATION Bin
		CONFIGURATIONS Release
		FILES_MATCHING
			PATTERN "*.dll"
			PATTERN "Debug" EXCLUDE
			PATTERN "Release" EXCLUDE)

	install(DIRECTORY Dll/${ZEBUILD_PLATFORM}/Release/
		DESTINATION Bin
		CONFIGURATIONS Release)

	include(InstallRequiredSystemLibraries)

	set(CPACK_PACKAGE_NAME "Zinek Engine")
	set(CPACK_PACKAGE_VENDOR "Zinek Code House and Game Studio")
	set(CPACK_PACKAGE_VERSION_MAJOR ${ZINEK_VERSION_MAJOR})
	set(CPACK_PACKAGE_VERSION_MINOR ${ZINEK_VERSION_MINOR})
	set(CPACK_PACKAGE_VERSION_PATCH "0")
	set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Zinek")
	set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/Installation/Description.txt")
	set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})
	set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}\\\\Installation\\\\InstallIcon.bmp")

	set(CPACK_NSIS_DISPLAY_NAME ${CPACK_PACKAGE_NAME})
	set(CPACK_NSIS_PACKAGE_NAME ${CPACK_PACKAGE_NAME})
	set(CPACK_NSIS_INSTALLED_ICON_NAME "ZE.exe")
	set(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.zinekengine.com\\\\en\\\\docs\\\\install")
	set(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.zinekengine.com")
	set(CPACK_NSIS_CONTACT "contact@zinekengine.com")
	set(CPACK_NSIS_MODIFY_PATH 0)

	set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/Installation/License.txt")
	set(CPACK_RESOURCE_FILE_WELCOME "${CMAKE_SOURCE_DIR}/Installation/Welcome.txt")
	set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/Installation/ReadMe.txt")

	set(CPACK_COMPONENTS_ALL_GROUPS_IN_ONE_PACKAGE 1)

	set(CPACK_COMPONENT_RUNTIME_DISPLAY_NAME "Runtime")
	set(CPACK_COMPONENT_RUNTIME_DESCRIPTION "Runtime Required to Run Zinek Engine base Projects")
	set(CPACK_COMPONENT_RUNTIME_GROUP "Zinek Engine")
	set(CPACK_COMPONENT_RUNTIME_REQUIRED 1)

	set(CPACK_COMPONENT_DEMOS_DISPLAY_NAME "Demos")
	set(CPACK_COMPONENT_DEMOS_DESCRIPTION "Demos that are developed by Zinek Engine")
	set(CPACK_COMPONENT_DEMOS_GROUP "Zinek Engine")

	set(CPACK_COMPONENT_ZEEDITOR_DISPLAY_NAME "ZEEditor")
	set(CPACK_COMPONENT_ZEEDITOR_DESCRIPTION "Zinek Engine Editor")
	set(CPACK_COMPONENT_ZEEDITOR_GROUP "Content Creation")

	set(CPACK_COMPONENT_ZE3DSMAX_EXPORTERS_DISPLAY_NAME "Autodesk 3ds Max Exporters")
	set(CPACK_COMPONENT_ZE3DSMAX_DESCRIPTION "3ds Max plugins for exporting ZEModel and ZEMap files.")
	set(CPACK_COMPONENT_ZE3DSMAX_GROUP "Content Creation")

	set(CPACK_COMPONENT_ZEMAYA_EXPORTERS_DISPLAY_NAME "Autodesk Maya Exporters")
	set(CPACK_COMPONENT_ZEMAYA_DESCRIPTION "Autodesk Maya Exporters plugins for exporting ZEModel and ZEMap files.")
	set(CPACK_COMPONENT_ZEMAYA_GROUP "Content Creation")

	set(CPACK_COMPONENT_ZESOFTIMAGE_EXPORTERS_DISPLAY_NAME "Autodesk Softimage XSI Exporters")
	set(CPACK_COMPONENT_ZESOFTIMAGE_DESCRIPTION "Autodesk Softimage XSI plugins for exporting ZEModel and ZEMap files.")
	set(CPACK_COMPONENT_ZESOFTIMAGE_GROUP "Content Creation")

	set(CPACK_COMPONENT_ZEBLENDER_EXPORTERS_DISPLAY_NAME "Blender Exporters")
	set(CPACK_COMPONENT_ZEBLENDER_DESCRIPTION "Blender plugins for exporting ZEModel and ZEMap files.")
	set(CPACK_COMPONENT_ZEBLENDER_GROUP "Content Creation")

	set(CPACK_COMPONENT_ZECONENTSAMPLES_EXPORTERS_DISPLAY_NAME "Sample and Base Contents")
	set(CPACK_COMPONENT_ZECONENTSAMPLES_DESCRIPTION "Sample and base conent files for creating and deriving new content.")
	set(CPACK_COMPONENT_ZECONENTSAMPLES_GROUP "Content Creation")

	set(CPACK_COMPONENT_ZESDK_DISPLAY_NAME "ZESDK")
	set(CPACK_COMPONENT_ZESDK_DESCRIPTION "Zinek Engine C++ Software Development Kit.")
	set(CPACK_COMPONENT_ZESDK_GROUP "C++ Development")

	set(CPACK_COMPONENT_ZESDKSAMPLES_DISPLAY_NAME "Samples")
	set(CPACK_COMPONENT_ZESDKSAMPLES_DESCRIPTION "Sample applications that are developed by ZESDK.")
	set(CPACK_COMPONENT_ZESDKSAMPLES_GROUP "C++ Development")

	set(CPACK_COMPONENT_ZESDKDOCUMENTS_DISPLAY_NAME "Documentation")
	set(CPACK_COMPONENT_ZESDKDOCUMENTS_DESCRIPTION "Developers Guide and referance documentations.")
	set(CPACK_COMPONENT_ZESDKDOCUMENTS_GROUP "C++ Development")

	include(CPack)
endif()

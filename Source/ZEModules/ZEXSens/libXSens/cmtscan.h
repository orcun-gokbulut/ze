//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - cmtscan.h
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

#ifndef XSENS_MONOLITHIC
/*! \file
	\brief	Contains the ScanPorts interface

	This file contains the interfaces for the functions that are required for scanning ports
	for Xsens devices.

	\section FileCopyright Copyright Notice 
	Copyright (C) Xsens Technologies B.V., 2006.  All rights reserved.

	This source code is intended for use only by Xsens Technologies BV and
	those that have explicit written permission to use it from
	Xsens Technologies BV.

	THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
	PARTICULAR PURPOSE.

	\section FileChangelog	Changelog
	\par 2006-06-09, v0.0.1
	\li Job Mulder:	Created ScanPorts.h
	\par 2006-07-21, v0.1.0
	\li Job Mulder:	Updated file for release 0.1.0
*/
#endif

#ifndef CMTSCAN_H
#define CMTSCAN_H

#ifndef XSENS_MONOLITHIC
#	include "xsens_list.h"
#endif

namespace xsens {

/*! \brief Scan COM ports for connected Xsens devices.

	The cmtScanPorts function will scan registered Xsens USB converters and serial COM ports
	for connected Xsens devices. If the baudrate parameter is 0 (default), it will try to
	connect at all supported baud rates, starting with the most common 115k2, 460k8 and 
	58k6. If the baudrate parameter is non-zero, only the specified baudrate is tried.
	Any detected devices are returned in the ports list, which is sorted by port nr.
	
*/
bool cmtScanPorts(List<CmtPortInfo>& ports, uint32_t baudrate = 0, uint32_t singleScanTimeout = 1000, uint32_t scanTries = 1, bool ignoreNonXsensDevices = true);

/*! \brief Scan a single COM port for connected Xsens devices.

	The cmtScanPort function will scan a single port for connected Xsens devices. If the
	baudrate parameter is 0 (default), it will try to connect at all supported baud 
	rates, starting with the most common 115k2, 460k8 and 58k6. If the baudrate parameter
	is non-zero, only the specified baud rate is tried. Any detected devices are returned
	in the portInfo parameter.
	
*/
bool cmtScanPort(CmtPortInfo& portInfo, uint32_t baudrate = 0, uint32_t singleScanTimeout = 1000, uint32_t scanTries = 1);

//! Set to true from another thread to abort any scan currently in progress.
extern bool abortScan;

} // end of xsens namespace

#endif	// CMTSCAN_H

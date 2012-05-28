//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureFileJPEGMarkerReader.h
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
#ifndef __ZE_TEXTURE_FILE_JPEG_MARKER_READER_H__
#define __ZE_TEXTURE_FILE_JPEG_MARKER_READER_H__

#include "ZETextureFileJPEGCommonDefinitions.h"
#include "ZETypes.h"

class ZEFile;
struct ZEJpegDeCompressionInfo;

// Buffers data from file and finds markers
class ZEJpegFileMarkerBuffer
{

	private:
		ZEFile*					ImageFile;

		ZEUInt8					Buffer[ZE_JPEG_INPUT_BUFFER_SIZE];
		ZESize					BytesRead;
		ZESize					BytesAvailable;

		bool					FillBuffer(bool ForceFill = false);

								ZEJpegFileMarkerBuffer();
								~ZEJpegFileMarkerBuffer();
	public :
		ZESize					GetAvailableByteCount() const;

		bool					GetOneByte(ZEUInt8& Byte);
		bool					GetTwoByte(ZEUInt16& TwoByte);
		bool					GetTwoByte(ZEUInt8& Byte1, ZEUInt8& Byte2);

		bool					RewindOneByte();
		bool					RewindTwoByte();

		bool					SkipVariableLenghtMarker();
		bool					SkipBufferData(ZEUInt32 DataLenght);

		bool					GetNextMarker(ZEJpegFileMarker& Marker);

		void					Initialize(ZEFile* ImageFile);
		void					Deinitialize();

		void					Destroy();
		static ZEJpegFileMarkerBuffer* CreateInstance();

};


class ZEJpegFileMarkerReader
{

	private:
		ZEFile*						ImageFile;
		ZEJpegFileMarkerBuffer*		MarkerBuffer;
		ZEJpegDeCompressionInfo*	Info;

		bool						SOFFound;
		bool						SOIFound;
		bool						EOIFound;
		ZEUInt8						NextRestart;

		// Not active yet
		ZEUInt						SeenSOSCount;

		bool						ProcessSOIMarker();
		bool						ProcessEOIMarker();

		bool						ProcessAPP0Marker();
		bool						ProcessAPP14Marker();

		bool						ProcessSOFMarker(bool BaseLine, bool Progressive);
		bool						ProcessSOSMarker();
		bool						ProcessDHTMarker();
		bool						ProcessDQTMarker();
		bool						ProcessDRIMarker();
		bool						ProcessRSTMarker(ZEJpegFileMarker RstMarker);

									ZEJpegFileMarkerReader();
									~ZEJpegFileMarkerReader();

	public:
		bool						ReadMarkers(ZEJpegFileMarker& StoppedAt);

		bool						GetSOFFound() const;
		bool						GetSOIFound() const;
		bool						GetEOIFound() const;
		ZEUInt						GetSeenSOSCount() const;
		
		ZEJpegFileMarkerBuffer*		GetMarkerBuffer() const;

		void						Initialize(ZEFile* ImageFile, ZEJpegDeCompressionInfo* Info);
		void						Deinitialize();

		void						Destroy();
		static ZEJpegFileMarkerReader* CreateInstance();

};

#endif // __ZE_TEXTURE_FILE_JPEG_MARKER_READER_H__

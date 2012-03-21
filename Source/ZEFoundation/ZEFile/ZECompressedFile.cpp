//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECompressedFile.cpp
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

#include "ZETDefinitions.h"
#include "ZECompressedFile.h"

#include <zlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <fstream>
#include <io.h>



#define CHUNK_SIZE 1048576
#pragma warning(push)    
#pragma warning(disable:4996 4267)


/////////////////////////////////////////////////////////////////////////
////////////////////////		ZEFile		/////////////////////////////
/////////////////////////////////////////////////////////////////////////

struct Index
{
	ZEUInt64				NextHolderPosition;
	ZEUInt64				UnCompressedDataSize;
};


ZECompressedFile::ZECompressedFile() : File(NULL), FileCursor(0)
{
	// Empty
}

ZECompressedFile::~ZECompressedFile()
{
	Close();
}

const char* ZECompressedFile::GetFileName() const
{
	return FileName;
}

void* ZECompressedFile::GetFileHandle() const
{
	return File;
}

bool ZECompressedFile::Eof()
{
	if(!File)
		return false;

	return feof((FILE*)File) == 0;
}

bool ZECompressedFile::Seek(ZEInt64 Offset, ZESeekFrom Origin)
{
	//If file is not opened return false
	if(!File)
		return false;

	//Store current cursor position
	ZEUInt64 CursorPositionBeforeSeek = _ftelli64((FILE*)File);
	_fseeki64((FILE*)File,0,SEEK_SET);
	ZEUInt64 CursorPosition = 0;
	
	switch(Origin)
	{
		case ZE_SF_BEGINING:
			CursorPosition = Offset;
			break;
		case ZE_SF_CURRENT:
			CursorPosition += Offset;
			break;
		case ZE_SF_END:
			return false;

	}

	ZEUInt64 TotalReadData = 0;
	ZEUInt64 ReadDataSize;
	Index CurrentIndex;

	while(true)
	{
		//Store read data size
		ReadDataSize = fread(&CurrentIndex,sizeof(Index),1,(FILE*)File);

		//Calculate total read data size
		TotalReadData += CurrentIndex.UnCompressedDataSize;

		//If cant read data return 0
		if(ReadDataSize <= 0)
			return false;

		//Such a position Exists in file
		else if(TotalReadData > CursorPosition)
		{

			//Set new file cursor
			if(Origin == ZE_SF_CURRENT)
				FileCursor += CursorPosition;

			else if(Origin == ZE_SF_BEGINING)
				FileCursor = CursorPosition;

			return true;

		}

		//Go to next index
		_fseeki64((FILE*)File,CurrentIndex.UnCompressedDataSize,SEEK_CUR);
	}

	//Go to stored file cursor position
	_fseeki64((FILE*)File,CursorPositionBeforeSeek,SEEK_SET);

	//It gone all wrong set file cursor to 0,return false
	FileCursor = 0;
	return false;
}

ZEUInt64 ZECompressedFile::Tell()
{
	if(!File)
		return 0;

	//Return file cursor position
	return FileCursor;
}

void ZECompressedFile::Close()
{
	if (File != NULL)
	{	
		fclose((FILE*)File);
		File = NULL;
		FileCursor = 0;
	}
}

void ZECompressedFile::Flush()
{
	if(!File)
		return;

	fflush((FILE*)File);
}

bool ZECompressedFile::IsOpen()
{
	if(File && FileName[0])
		return true;
	return false;
}

bool ZECompressedFile::Open(const char* FileName, ZEFileMode Mode, bool Binary)
{
	strncpy(this->FileName, FileName, ZE_MAX_FILE_NAME_SIZE);

	const char* StrMode = NULL;
	const char*	AltStrMode = NULL;

	switch(Mode)
	{
		case ZE_FM_READ_ONLY:
		{
			if (Binary)		
				StrMode = "rb";
			else
				StrMode = "r";
			break;
		}
			
		case ZE_FM_WRITE_ONLY:
		{
			if (Binary)
			{
				StrMode = "r+b";
				AltStrMode = "w+b";
			}
			else
			{
				StrMode = "r+";
				AltStrMode = "w+";
			}
			break;
		}
			
		case ZE_FM_APPEND_ONLY:
		{
			if (Binary)
				StrMode = "ab";
			else
				StrMode = "a";
			break;
		}

		case ZE_FM_READ_APPEND:
		{
			if(Binary)
				StrMode = "a+b";
			else
				StrMode = "a+";
			break;
		}
			
		case ZE_FM_READ_WRITE:
		{
			if(Binary)
			{
				StrMode = "r+b";
				AltStrMode = "w+b";
			}
			else
			{
				StrMode = "r+";
				AltStrMode = "w+";
			}
			break;
		}
			
		default:
			return false;
			break;	
	}

	// Try to open file 
	File = fopen(this->GetFileName(),StrMode);
	if(File == NULL) // If cant open
	{
		if(AltStrMode) // If there is an alternative opening mode
		{
			// Try to open with alternative mode
			File = fopen(this->FileName, AltStrMode);
			
			if(File != NULL) // If opened
			{
				return true;
			}
			else
			{
				//zeError("Could not open file \"%s\".", FileName);
				return false;
			}
		}
		else
		{
			//zeError("Could not open file \"%s\".", FileName);
			return false;
		}
	}
	else
		return true;
	return false;
}    

ZEUInt64 ZECompressedFile::Read(void* Buffer, ZEUInt64 Size,ZEUInt64 Count)
{
	ZEInt64 BufferPointer;
	ZEUInt64 DataReadSize;
	ZEUInt64 TotalPassedData = 0;
	ZEUInt64 DataStartPosition = 0;
	ZEUInt64 FirstIndexPosition;
	ZEUInt64 LastIndexPosition;
	ZEUInt64 FirstBlockTampon = 0;
	ZEUInt64 FileCursorBeforeRead = 0;
	bool FirstBlockInTheProcess = true;
	bool LastBlock = false;
	Index CurrentIndex;
	Index FirstIndex;
	Index LastIndex;
	FileCursorBeforeRead = _ftelli64((FILE*)File);
	bool FirstIndexFound = false;
	bool LastIndexFound = false;

	while(true)
	{
		
		//Get index from file
		DataReadSize = fread(&CurrentIndex,sizeof(Index),1,(FILE*)File);

		//Calculate total passed data 
		TotalPassedData += CurrentIndex.UnCompressedDataSize;

		//If cant read any data return false
		if(DataReadSize == 0)
			return false;

		//Check if first index to compress data from is found
		if(!FirstIndexFound)
		{

			//Check if we passed cursor position
			if(TotalPassedData > FileCursor)
			{

				//if so, store current index as first index
				memcpy(&FirstIndex,&CurrentIndex,sizeof(Index));

				//first index to compress data from is found
				FirstIndexFound = true;

				//Go to start position of current index
				_fseeki64((FILE*)File, - (ZEInt64)sizeof(Index),SEEK_CUR);

				//Store first indexes start position
				FirstIndexPosition = _ftelli64((FILE*)File);

				//Calculate how much data must be ignored from current index
				DataStartPosition = TotalPassedData - CurrentIndex.UnCompressedDataSize;
				FirstBlockTampon = FileCursor - DataStartPosition;

			}
			else
			{
				//If current index is not the one we are looking for
				_fseeki64((FILE*)File,CurrentIndex.NextHolderPosition + FileCursorBeforeRead,SEEK_SET);
			}

		}

		//If first index is found , search for last index
		else
		{

			//Check is wanted data end position is within current index
			if(TotalPassedData >= FileCursor + Size)
			{

				//If so, store current index as last index
				memcpy(&LastIndex,&CurrentIndex,sizeof(Index));

				//last index is found 
				LastIndexFound = true;

				//Go to current indexes start position
				_fseeki64((FILE*)File, - (ZEInt64)sizeof(Index),SEEK_CUR);

				//Store this position
				LastIndexPosition = _ftelli64((FILE*)File);

			}
			else
			{
				//If current is not the last index we are looking for
				_fseeki64((FILE*)File,CurrentIndex.NextHolderPosition + FileCursorBeforeRead,SEEK_SET);
			}
		}

		//If first and last index is found break while
		if(FirstIndexFound && LastIndexFound)
			break;

	}

	ZEInt InflateReturnValue = 0;
	ZEInt Flush;
	Index ActiveIndex;
	ZEUInt64 DeCompressedDataSize = 0;
	ZEUInt64 TotalDeCompressedDataSize = 0;
	ZEUInt64 DataWillBeReadSize = CHUNK_SIZE;
//	ZEUInt64 DataWillBeReadTotalSize;
	ZEUInt64 TotalReadData = 0;

	//Create in and out buffers for inflate function
	unsigned char* InputBuffer = new unsigned char[CHUNK_SIZE];
	unsigned char* OutputBuffer= new unsigned char[CHUNK_SIZE];

	//Create stream for inflate function
	z_stream Stream;
	Stream.zfree = Z_NULL;
	Stream.zalloc = Z_NULL;
	Stream.opaque = Z_NULL;
	Stream.avail_in = 0;
	Stream.next_in = Z_NULL;

	// Initialize inflate function
	inflateInit2(&Stream,-15);

	//Go to chunks start position
	_fseeki64((FILE*)File,FileCursorBeforeRead,SEEK_SET);

	//Search for index to start compress
	while(true)
	{

		//Get index from file
		DataReadSize = fread(&ActiveIndex,sizeof(Index),1,(FILE*)File);

		//If cant return false
		if(DataReadSize == 0)
			return false;

		//If next index is first index success , else go to next index
		if(ActiveIndex.NextHolderPosition == FirstIndex.NextHolderPosition)
			break;
		else
			_fseeki64((FILE*)File,ActiveIndex.NextHolderPosition + FileCursorBeforeRead,SEEK_SET);
		
	}

	//If first index equals to last index last block is true
	if(FirstIndex.NextHolderPosition == LastIndex.NextHolderPosition && LastIndex.UnCompressedDataSize == FirstIndex.UnCompressedDataSize)
		LastBlock=true;

	//Turn  until all input data will be decompressed
	while(true)
	{
		//If current index is not last block 
		if(!LastBlock)
			Flush = Z_BLOCK;

		//If current index is last block
		else
		{
			//If so, that is last piece of data to decompress
			Flush = Z_FINISH;
			InflateReturnValue = Z_STREAM_END;

		}
		
		//Calculate how much data must be read from file
		ZEUInt64 naynay = _ftelli64((FILE*)File);
		DataWillBeReadSize = ActiveIndex.NextHolderPosition - _ftelli64((FILE*)File) + FileCursorBeforeRead;


		//Say stream how much data must be decompressed
		Stream.avail_in = (uInt)DataWillBeReadSize;

		//Put data which will be decompressed to in buffer
		fread(InputBuffer,DataWillBeReadSize,1,(FILE*)File);

		//Calculate total read data size
		TotalReadData += DataWillBeReadSize;

		//Put in buffer to stream
		Stream.next_in = InputBuffer;

		//Turn until all input are decompressed
		while(true)
		{

			//Say stream that how much output will be taken 
			Stream.avail_out = CHUNK_SIZE;

			//Say stream output buffer
			Stream.next_out = OutputBuffer;

			//INFLATE 
			InflateReturnValue = inflate(&Stream,Flush);

			//calculate decompressed data size
			DeCompressedDataSize = CHUNK_SIZE - Stream.avail_out;

			//Add decompressed data size to total decompressed data size
			TotalDeCompressedDataSize += DeCompressedDataSize;

			//Check if total output is bigger than wanted data size and current block is the first
			//decompressed block
			if(TotalDeCompressedDataSize >= Size && !FirstBlockInTheProcess)
			{

				//Calculate size of data must be taken from buffer
				TotalDeCompressedDataSize -= DeCompressedDataSize;
				DeCompressedDataSize = Size - TotalDeCompressedDataSize;

				//Recalculate total decompressed data size
				TotalDeCompressedDataSize += DeCompressedDataSize;

				//Final block is decompressed
				InflateReturnValue = Z_STREAM_END;

			}

			//Check if first block is in the process and got output from inflate
			if(FirstBlockInTheProcess && DeCompressedDataSize != 0)
			{

				//Calculate how much data must be taken from buffer
				ZEUInt64 sizeToCopy;
				if(DeCompressedDataSize-FirstBlockTampon > Size)
					sizeToCopy = Size;
				else
					sizeToCopy = DeCompressedDataSize-FirstBlockTampon;

				//Ignore unwanted data from output buffer
				OutputBuffer += FirstBlockTampon;

				//Get data from output buffer
				memcpy(Buffer,OutputBuffer,sizeToCopy);

				//Cancel the changes made at output buffer
				OutputBuffer -= FirstBlockTampon;

				//Calculate datas new pointer
				BufferPointer = (ZEInt64)Buffer;
				BufferPointer += sizeToCopy;
				Buffer = (void*)BufferPointer;

				//Set total decompressed data size to size of taken data from buffer
				TotalDeCompressedDataSize = sizeToCopy;

				//First block is decompressed 
				FirstBlockInTheProcess = false;
			}

			else
			{

				//Copy all output buffer to data
				memcpy(Buffer,OutputBuffer,DeCompressedDataSize);

				//Recalculate datas pointer
				BufferPointer = (ZEInt64)Buffer;
				BufferPointer += DeCompressedDataSize;
				Buffer = (void*)BufferPointer;

			}

			//If all data is decompressed break
			if(Stream.avail_out != 0)
				break;
		}

		//Current Index is last block to compress break while, process is done

		if(LastBlock)
			break;
		if(ActiveIndex.NextHolderPosition == LastIndex.NextHolderPosition)
			LastBlock = true;

		//Go to next index position
		_fseeki64((FILE*)File,ActiveIndex.NextHolderPosition + FileCursorBeforeRead,SEEK_SET);

		//Get index from file
		fread(&ActiveIndex,sizeof(Index),1,(FILE*)File);

	} 

	//Go to stored position
	_fseeki64((FILE*)File,FileCursorBeforeRead,SEEK_SET);

	//Recalculate data pointer 
	BufferPointer = (ZEInt64)Buffer;
	BufferPointer -= TotalDeCompressedDataSize;
	Buffer = (void*)BufferPointer;

	//End inflate
	inflateEnd(&Stream);

	//All went well, return totaldecompresseddatasize
	FileCursor += TotalDeCompressedDataSize;
	return TotalDeCompressedDataSize;

}

ZEUInt64 ZECompressedFile::Write( const void* Buffer, ZEUInt64 Size, ZEUInt64 Count)
{
	if(!File)
		return 0;
	ZEInt Flush;
	ZEInt DeflateReturnValue;
	ZEInt64 BufferPointer;
	ZEUInt64 CompressedDataSize = 0;
	ZEUInt64 TotalCompressedDataSize = 0;
	ZEUInt64 BufferSizeToCopy = CHUNK_SIZE;
	ZEUInt64 BufferStartPointToCopy = 0;
	Index ActiveIndex;
	ZEUInt64 ActiveIndexPosition;
	ZEUInt64 PreviousIndexPosition = 0;
	ZEUInt64 FileCursorBeforeWrite;
	ZEUInt64 ReturnValue;
	z_stream Stream;

	//Create in and out buffers for compress 
	unsigned char* InputBuffer = new unsigned char[CHUNK_SIZE];
	unsigned char* OutputBuffer = new unsigned char[CHUNK_SIZE];

	Stream.zfree=Z_NULL;
	Stream.zalloc=Z_NULL;
	Stream.opaque=Z_NULL;

	//Init deflate function
	deflateInit2(&Stream,5,Z_DEFLATED,-15,9,Z_DEFAULT_STRATEGY);

	//Store current cursor position
	FileCursorBeforeWrite = _ftelli64((FILE*)File);
	ReturnValue = _ftelli64((FILE*)File);

	while(true)
	{
		//Check if data size which will be compressed is bigger than Size
		if(BufferStartPointToCopy + BufferSizeToCopy > Size)
		{
			//if so calculate size which must be compressed
			BufferSizeToCopy = Size - BufferStartPointToCopy;

			//tell deflate that is last piece of data
			Flush = Z_FINISH;

		}
		else
		{
			//set buffer size to chunk size 
			BufferSizeToCopy = CHUNK_SIZE;

			//Partial compress !!!
			Flush = Z_FULL_FLUSH;

		}

		//Update Compressed data size
		BufferStartPointToCopy += BufferSizeToCopy;

		//Put data to inBuffer 
		memcpy(InputBuffer, Buffer,BufferSizeToCopy);

		//Update Buffer pointer 
		BufferPointer = (ZEInt64)Buffer;
		BufferPointer += BufferSizeToCopy;
		Buffer = (void*)BufferPointer;

		//Tell stream that how many data will be compressed
		Stream.avail_in = (uInt)BufferSizeToCopy;

		//Put data that will be compressed to stream
		Stream.next_in = InputBuffer;

		//Store position for new position holder
		PreviousIndexPosition = _ftelli64((FILE*)File);


		ActiveIndex.NextHolderPosition = NULL;
		ActiveIndex.UnCompressedDataSize = NULL;
		
		//Write index to its position
		fwrite(&ActiveIndex,sizeof(Index),1,(FILE*)File);

		while(true)
		{
			//Tell stream that it will give output as big as chunks size
			Stream.avail_out = CHUNK_SIZE;

			//Set streams output buffer
			Stream.next_out = OutputBuffer;

			//Deflate 
			DeflateReturnValue = deflate(&Stream,Flush);

			//Calculate compressed data size
			CompressedDataSize = CHUNK_SIZE - Stream.avail_out;

			//Write output to file
			fwrite(OutputBuffer,CompressedDataSize,1,(FILE*)File);

			//Add compressed data size to tatal compressed data size
			TotalCompressedDataSize += CompressedDataSize;

			//If stream.avail_out != 0 , all input compressed , if so break while
			if(Stream.avail_out != 0)
				break;

		}
		//Store this position
		ActiveIndexPosition = _ftelli64((FILE*)File);
		
		//Go to last indexs position
		_fseeki64((FILE*)File,PreviousIndexPosition,SEEK_SET);

		//Get last index
		fread(&ActiveIndex,sizeof(Index),1,(FILE*)File);

		//Set variables of last index
		ActiveIndex.UnCompressedDataSize = BufferSizeToCopy;
		ActiveIndex.NextHolderPosition = (ZEUInt64)(ActiveIndexPosition - FileCursorBeforeWrite);

		//Go to last index position
		_fseeki64((FILE*)File,PreviousIndexPosition,SEEK_SET);

		//Write index
		fwrite(&ActiveIndex,sizeof(Index),1,(FILE*)File);

		//Go to active index position
		_fseeki64((FILE*)File,ActiveIndexPosition,SEEK_SET);

		//Check if that is the last piece of data
		if(Flush == Z_FINISH)
			break;
	}

	//end deflate function
	deflateEnd(&Stream);

	//Calculate new file cursor
	FileCursor += Size;

	//Calculate compressed size
	ReturnValue = (ZEInt64) (_ftelli64((FILE*)File) - ReturnValue);

	//retrun compressed size
	return ReturnValue;

}

ZEUInt64 ZECompressedFile::GetFileSize(const char* FileName)
{
	struct stat FileStatus;

	if(stat( FileName, &FileStatus ) != 0)
		return 0;

	return FileStatus.st_size;
}
ZEUInt64 ZECompressedFile::GetFileSize()
{
	if (!File)
		return 0;
	ZEUInt64 FieCursorBeforeGetFileSize = _ftelli64((FILE*)File);

	_fseeki64((FILE*)File, 0, SEEK_END);
	ZEUInt64 EndCursor = _ftelli64((FILE*)File);
	_fseeki64((FILE*)File, FieCursorBeforeGetFileSize, SEEK_SET);

	return EndCursor;
}


/////////////////////////////////////////////////////////////////////////
//////////////////		ZEPartialFile		/////////////////////////////
/////////////////////////////////////////////////////////////////////////

ZEPartialCompressedFile::ZEPartialCompressedFile() : StartPosition(0), EndPosition(0), IsEof(false)
{
	// Empty
}

ZEPartialCompressedFile::~ZEPartialCompressedFile()
{
	File = NULL;
}

// Should not be used
bool ZEPartialCompressedFile::Open(const char* FileName, ZEFileMode Mode, bool Binary)
{
	return false;
}

bool ZEPartialCompressedFile::Open(ZECompressedFile* ParentFile, ZEUInt64 Offset, ZEUInt64 Size)
{
	//zeAssert(Size == 0, "Cannot open a PartialFile with 0 size");

	IsEof			= false;
	StartPosition	= Offset;
	FileCursor		= StartPosition;
	EndPosition		= Offset + Size;
	this->File		= ParentFile->GetFileHandle();
	this->BaseFile	= ParentFile;

	strncpy(this->FileName, ParentFile->GetFileName(), ZE_MAX_FILE_NAME_SIZE);

	return true;
}

void ZEPartialCompressedFile::Close()
{
	StartPosition	= 0;
	EndPosition		= 0;
	File			= NULL;
	IsEof			= false;
}

ZEUInt64 ZEPartialCompressedFile::GetFileSize()
{
	return EndPosition - StartPosition;	
}

ZEUInt64 ZEPartialCompressedFile::Read(void* Buffer, ZEUInt64 Size, ZEUInt64 Count)
{
	//If file is not opened return false
	if(!File)
		return 0;

	// File cursor is out of bounds return 0
	if(FileCursor < StartPosition || EndPosition < FileCursor)
		return 0;

	// Calculate read end position
	ZEUInt64 ReadEndPosition = FileCursor + Size * Count;

	//If trying to read outside of bounds
	if (ReadEndPosition > EndPosition)
	{

		//Calculate how much allowed to read
		Size = EndPosition - FileCursor;

		//End of Partial File
		IsEof = true;
	}

	// Set ZEFiles file cursor to partial files cursor
	BaseFile->FileCursor = FileCursor;

	//Read
	ZEUInt64 ReadCount = BaseFile->Read(Buffer,Size,Count);

	//Calculate file cursor
	FileCursor += Count * Size;

	//Return read count
	return ReadCount;

}

ZEUInt64 ZEPartialCompressedFile::Write(void* Buffer, ZEUInt64 Size, ZEUInt64 Count)
{
	//WRITE IS NOT ALLOWED !!!
	return 0;
}

bool ZEPartialCompressedFile::Seek(ZEInt64 Offset, ZESeekFrom Origin)
{
	//If file is not opened return 0
	if(!File)
		return 0;

	//Store cursor position
	ZEUInt64 FileCursorPosition = _ftelli64((FILE*)File);

	switch(Origin)
	{
	case SEEK_SET:
		{
			//If out of bounds return false
			if(Offset < 0 || EndPosition < StartPosition + Offset)
				return false;

			//If cant seek return false
			if(!BaseFile->Seek(Offset, ZE_SF_BEGINING))
				return false;

			//Calculate File cursor position
			FileCursor = Offset + StartPosition;

			//Done, break
			break;

		}

	case SEEK_END:
		{

			//If out of bounds return false
			if(FileCursor + Offset < StartPosition || EndPosition < FileCursor + Offset)
				return false;

			//If cant seek return false
			if(!BaseFile->Seek(Offset, ZE_SF_CURRENT))
				return false;

			//Calculate cursor position
			FileCursor += Offset;

			//Done, break
			break;

		}

	case SEEK_CUR:
		{
			//CANT SEEK FROM END
			return false;
			break;
		}

	default:
		return false;
		break;
	}

	//Seek to stored position
	_fseeki64((FILE*)File,FileCursorPosition,SEEK_SET);

	//If end of file or out of bounds retun false
	if(IsEof == true && FileCursor >= StartPosition && FileCursor < EndPosition)
		IsEof = false;

	//All went well, return true
	return true;
}

ZEUInt64 ZEPartialCompressedFile::Tell()
{
	return FileCursor - StartPosition;
}

bool ZEPartialCompressedFile::Eof()
{
	return IsEof;
}

#pragma warning(pop)


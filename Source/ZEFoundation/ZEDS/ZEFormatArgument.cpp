//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFormatArgument.cpp
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

#include "ZEFormatArgument.h"

#include "ZEStringWriter.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEError.h"

#ifdef ZE_PLATFORM_UNIX
    #define sprintf_s snprintf
#endif


static bool ParseArgumentFormat(const char* ArgumentFormat, char** Parameters, ZESize Count)
{
	ZESize ParameterIndex = 0;

	char ParameterString[255];
	ZESize ParameterStringIndex = 0;

	const char* Current = ArgumentFormat;
	ZEUInt Index = 0;
	while(*Current != '\0')
	{
		if (Index >= 254)
			return false;

		if (*Current == ':')
		{
			if (ParameterIndex >= Count)
				return false;

			ParameterString[ParameterStringIndex] = '\0';
			if (ParameterStringIndex != 0)
				strcpy(Parameters[ParameterIndex], ParameterString);
			ParameterIndex++;
			
			ParameterString[0] = '\0';
			ParameterStringIndex = 0;
		}
		else
		{
			ParameterString[ParameterStringIndex] = *Current;
			ParameterStringIndex++;
		}

		Current++;
		Index++;
	}

	if (ParameterStringIndex != 0)
	{
		ParameterString[ParameterStringIndex] = '\0';
		strcpy(Parameters[ParameterIndex], ParameterString);
	}

	return true;
}

static bool ParseArgumentFormat(const char* ArgumentFormat, char* Parameter0)
{
	char* ArgumentFormatParameters[] = {Parameter0};
	return ParseArgumentFormat(ArgumentFormat, ArgumentFormatParameters, 1);
}

static bool ParseArgumentFormat(const char* ArgumentFormat, char* Parameter0, char* Parameter1)
{
	char* ArgumentFormatParameters[] = {Parameter0, Parameter1};
	return ParseArgumentFormat(ArgumentFormat, ArgumentFormatParameters, 2);
}

static bool ParseArgumentFormat(const char* ArgumentFormat, char* Parameter0, char* Parameter1, char* Parameter2)
{
	char* ArgumentFormatParameters[] = {Parameter0, Parameter1, Parameter2};
	return ParseArgumentFormat(ArgumentFormat, ArgumentFormatParameters, 3);
}

static bool ParseArgumentFormat(const char* ArgumentFormat, char* Parameter0, char* Parameter1, char* Parameter2, char* Parameter3)
{
	char* ArgumentFormatParameters[] = {Parameter0, Parameter1, Parameter2, Parameter3};
	return ParseArgumentFormat(ArgumentFormat, ArgumentFormatParameters, 4);
}

static bool ParseArgumentFormat(const char* ArgumentFormat, char* Parameter0, char* Parameter1, char* Parameter2, char* Parameter3, char* Parameter4)
{
	char* ArgumentFormatParameters[] = {Parameter0, Parameter1, Parameter2, Parameter3, Parameter4};
	return ParseArgumentFormat(ArgumentFormat, ArgumentFormatParameters, 4);
}

static bool CheckArgumentFormat(const char* ArgumentFormat)
{
	if (ArgumentFormat == NULL)
		return true;

	while(*ArgumentFormat != 0)
	{
		if (!isdigit(*ArgumentFormat) && *ArgumentFormat != '.' && *ArgumentFormat != '#' && *ArgumentFormat != '.' && *ArgumentFormat != '-' && *ArgumentFormat != '+')
			return false;
		ArgumentFormat++;
	}

	return true;
}

static void PrintFloat(ZEStringWriter& Output, float Argument, const char* FormattingOptions)
{
	char Temp[256];
	char Format[256];
	if (FormattingOptions == NULL)
		sprintf_s(Format, 256, "%%f");
	else
		sprintf_s(Format, 256, "%%%sf", FormattingOptions);
	sprintf_s(Temp, 256, Format, Argument);
	Output.Append(Temp);
}

bool ZEFormatArgument::Format(ZEStringWriter& Output, ZEInt Argument, const char* ArgumentFormat)
{
	char Type[256] = "d";
	char FormattingOptions[256] = "";

	if(ArgumentFormat != NULL)
		if (!ParseArgumentFormat(ArgumentFormat, Type, FormattingOptions))
			return false;

	if (Type[0] != 'd' && Type[0] != 'x' && Type[0] != 'X' && Type[1] != '\0')
		return false;

	if (!CheckArgumentFormat(FormattingOptions))
		return false;

	char Temp[256];
	char Format[256];
	if (FormattingOptions == NULL)
		sprintf_s(Format, 256, "%%%s", Type);
	else
		sprintf_s(Format, 256, "%%%s%s", FormattingOptions, Type);
	sprintf_s(Temp, 256, Format, Argument);
	Output.Append(Temp);

	return true;
}


bool ZEFormatArgument::Format(ZEStringWriter& Output, ZEUInt Argument, const char* ArgumentFormat)
{
	char Type[256] = "d";
	char FormattingOptions[256] = "";

	if(ArgumentFormat != NULL)
		if (!ParseArgumentFormat(ArgumentFormat, Type, FormattingOptions))
			return false;

	if (Type[0] != 'd' && Type[0] != 'x' && Type[0] != 'X' && Type[1] != '\0')
		return false;

	if (!CheckArgumentFormat(FormattingOptions))
		return false;

	if (Type[0] == 'd')
		Type[0] = 'u';

	char Temp[256];
	char Format[256];
	if (FormattingOptions == NULL)
		sprintf_s(Format, 256, "%%%s", Type);
	else
		sprintf_s(Format, 256, "%%%s%s", FormattingOptions, Type);
	sprintf_s(Temp, 256, Format, Argument);
	Output.Append(Temp);

	return true;
}

bool ZEFormatArgument::Format(ZEStringWriter& Output, ZEInt64 Argument,	const char* ArgumentFormat)
{
	char Type[256] = "d";
	char FormattingOptions[256] = "";

	if(ArgumentFormat != NULL)
		if (!ParseArgumentFormat(ArgumentFormat, Type, FormattingOptions))
			return false;

	if (Type[0] != 'd' && Type[0] != 'x' && Type[0] != 'X' && Type[1] != '\0')
		return false;

	if (!CheckArgumentFormat(FormattingOptions))
		return false;

	const char* InnerType;
	if (Type[0] == 'd')
		InnerType = "lld";
	else if (Type[0] == 'x')
		InnerType = "llx";
	else if (Type[0] == 'X')
		InnerType = "llX";
	
	char Temp[256];
	char Format[256];
	if (FormattingOptions == NULL)
		sprintf_s(Format, 256, "%%%s", InnerType);
	else
		sprintf_s(Format, 256, "%%%s%s", FormattingOptions, InnerType);
	sprintf_s(Temp, 256, Format, Argument);
	Output.Append(Temp);

	return true;
}

bool ZEFormatArgument::Format(ZEStringWriter& Output, ZEUInt64 Argument, const char* ArgumentFormat)
{
	char Type[256] = "d";
	char FormattingOptions[256] = "";

	if(ArgumentFormat != NULL)
		if (!ParseArgumentFormat(ArgumentFormat, Type, FormattingOptions))
			return false;

	if (Type[0] != 'd' && Type[0] != 'x' && Type[0] != 'X' && Type[1] != '\0')
		return false;

	if (!CheckArgumentFormat(FormattingOptions))
		return false;
	
	const char* InnerType;
	if (Type[0] == 'd')
		InnerType = "lld";
	else if (Type[0] == 'x')
		InnerType = "llx";
	else if (Type[0] == 'X')
		InnerType = "llX";

	char Temp[256];
	char Format[256];
	if (FormattingOptions == NULL)
		sprintf_s(Format, 256, "%%%s", InnerType);
	else
		sprintf_s(Format, 256, "%%%s%s", FormattingOptions, InnerType);
	sprintf_s(Temp, 256, Format, Argument);
	Output.Append(Temp);

	return true;
}

bool ZEFormatArgument::Format(ZEStringWriter& Output, float Argument, const char* ArgumentFormat)
{
	char FormattingOptions[256] = "";

	if(ArgumentFormat != NULL)
		if (!ParseArgumentFormat(ArgumentFormat, FormattingOptions))
			return false;

	if (!CheckArgumentFormat(FormattingOptions))
		return false;

	char Temp[256];
	char Format[256];
	if (FormattingOptions == NULL)
		sprintf_s(Format, 256, "%%f");
	else
		sprintf_s(Format, 256, "%%%sf", FormattingOptions);
	sprintf_s(Temp, 256, Format, Argument);
	Output.Append(Temp);

	return true;
}

bool ZEFormatArgument::Format(ZEStringWriter& Output, bool Argument, const char* ArgumentFormat)
{
	char True[256] = "true";
	char False[256] = "false";
	
	if(ArgumentFormat != NULL)
		if (!ParseArgumentFormat(ArgumentFormat, True, False))
			return false;

	Output.Append(Argument ? True : False);

	return true;
}

bool ZEFormatArgument::Format(ZEStringWriter& Output, const ZEVector2& Argument, const char* ArgumentFormat)
{
	char PreFix[256] = "<";
	char PostFix[256] = ">";
	char Comma[256] = ", ";
	char FormattingOptions[256] = ".03";

	if(ArgumentFormat != NULL)
		if (!ParseArgumentFormat(ArgumentFormat, FormattingOptions, PreFix, PostFix, Comma))
			return false;

	if (!CheckArgumentFormat(FormattingOptions))
		return false;

	Output.Append(PreFix);

	PrintFloat(Output, Argument.x, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.y, FormattingOptions);
	
	Output.Append(PostFix);

	return true;
}

bool ZEFormatArgument::Format(ZEStringWriter& Output, const ZEVector3& Argument, const char* ArgumentFormat)
{
	char PreFix[256] = "<";
	char PostFix[256] = ">";
	char Comma[256] = ", ";
	char FormattingOptions[256] = ".03";

	if(ArgumentFormat != NULL)
		if (!ParseArgumentFormat(ArgumentFormat, FormattingOptions, PreFix, PostFix, Comma))
			return false;

	if (!CheckArgumentFormat(FormattingOptions))
		return false;

	Output.Append(PreFix);

	PrintFloat(Output, Argument.x, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.y, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.z, FormattingOptions);
	
	Output.Append(PostFix);

	return true;
}

bool ZEFormatArgument::Format(ZEStringWriter& Output, const ZEVector4& Argument, const char* ArgumentFormat)
{
	char PreFix[256] = "<";
	char PostFix[256] = ">";
	char Comma[256] = ", ";
	char FormattingOptions[256] = ".03";

	if(ArgumentFormat != NULL)
		if (!ParseArgumentFormat(ArgumentFormat, FormattingOptions, PreFix, PostFix, Comma))
			return false;

	if (!CheckArgumentFormat(FormattingOptions))
		return false;

	Output.Append(PreFix);

	PrintFloat(Output, Argument.x, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.y, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.z, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.w, FormattingOptions);
	
	Output.Append(PostFix);

	return true;
}

bool ZEFormatArgument::Format(ZEStringWriter& Output, const ZEQuaternion& Argument, const char* ArgumentFormat)
{
	char PreFix[256] = "<";
	char PostFix[256] = ">";
	char Comma[256] = ", ";
	char FormattingOptions[256] = ".03";

	if(ArgumentFormat != NULL)
		if (!ParseArgumentFormat(ArgumentFormat, FormattingOptions, PreFix, PostFix, Comma))
			return false;

	if (!CheckArgumentFormat(FormattingOptions))
		return false;

	Output.Append(PreFix);

	PrintFloat(Output, Argument.x, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.y, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.z, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.w, FormattingOptions);
	
	Output.Append(PostFix);

	return true;
}

bool ZEFormatArgument::Format(ZEStringWriter& Output, const ZEMatrix3x3& Argument, const char* ArgumentFormat)
{
	char PreFix[256] = "<";
	char PostFix[256] = ">";
	char Comma[256] = ", ";
	char FormattingOptions[256] = ".03";
	char Seperator[256] = ", ";

	if(ArgumentFormat != NULL)
		if (!ParseArgumentFormat(ArgumentFormat, FormattingOptions, PreFix, PostFix, Comma, Seperator))
			return false;

	if (!CheckArgumentFormat(FormattingOptions))
		return false;

	Output.Append(PreFix);

	PrintFloat(Output, Argument.M11, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.M12, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.M13, FormattingOptions);

	Output.Append(Seperator);

	PrintFloat(Output, Argument.M21, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.M22, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.M23, FormattingOptions);

	Output.Append(Seperator);

	PrintFloat(Output, Argument.M31, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.M32, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.M33, FormattingOptions);

	Output.Append(PostFix);

	return true;
}

bool ZEFormatArgument::Format(ZEStringWriter& Output, const ZEMatrix4x4& Argument,const char* ArgumentFormat)
{
	char PreFix[256] = "<";
	char PostFix[256] = ">";
	char Comma[256] = ", ";
	char FormattingOptions[256] = ".03";
	char Seperator[256] = ", ";

	if(ArgumentFormat != NULL)
		if(!ParseArgumentFormat(ArgumentFormat, FormattingOptions, PreFix, PostFix, Comma, Seperator))
			return false;

	Output.Append(PreFix);
	
	PrintFloat(Output, Argument.M11, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.M12, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.M13, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.M14, FormattingOptions);
	
	Output.Append(Seperator);
	
	PrintFloat(Output, Argument.M21, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.M22, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.M23, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.M24, FormattingOptions);
	
	Output.Append(Seperator);
	
	PrintFloat(Output, Argument.M31, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.M32, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.M33, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.M34, FormattingOptions);

	Output.Append(Seperator);

	PrintFloat(Output, Argument.M41, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.M42, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.M43, FormattingOptions);
	Output.Append(Comma);
	PrintFloat(Output, Argument.M44, FormattingOptions);	
	Output.Append(PostFix);

	return true;
}

bool ZEFormatArgument::Format(ZEStringWriter& Output, const ZEString& Argument, const char* ArgumentFormat)
{
	Output.Append(Argument);
	return true;
}

bool ZEFormatArgument::Format(ZEStringWriter& Output, ZEObject* Argument, const char* ArgumentFormat)
{
	return "";
}

bool ZEFormatArgument::FormatVariant(ZEStringWriter& Output, const ZEVariant& Argument, const char* ArgumentFormat)
{
	switch(Argument.GetType())
	{
		default:
		case ZE_VRT_UNDEFINED:
		case ZE_VRT_NULL:
			return false;

		case ZE_VRT_FLOAT:
			return Format(Output, Argument.GetFloat(), ArgumentFormat);

		case ZE_VRT_DOUBLE:
			return Format(Output, (float)Argument.GetDouble(), ArgumentFormat);

		case ZE_VRT_INTEGER_8:
			return Format(Output, (ZEInt)Argument.GetInt8(), ArgumentFormat);

		case ZE_VRT_INTEGER_16:
			return Format(Output, (ZEInt)Argument.GetInt16(), ArgumentFormat);

		case ZE_VRT_INTEGER_32:
			return Format(Output, (ZEInt)Argument.GetInt32(), ArgumentFormat);

		case ZE_VRT_INTEGER_64:
			return Format(Output, (ZEInt64)Argument.GetInt64(), ArgumentFormat);

		case ZE_VRT_UNSIGNED_INTEGER_8:
			return Format(Output, (ZEUInt)Argument.GetUInt8(), ArgumentFormat);

		case ZE_VRT_UNSIGNED_INTEGER_16:
			return Format(Output, (ZEUInt)Argument.GetUInt16(), ArgumentFormat);

		case ZE_VRT_UNSIGNED_INTEGER_32:
			return Format(Output, (ZEUInt)Argument.GetUInt32(), ArgumentFormat);

		case ZE_VRT_UNSIGNED_INTEGER_64:
			return Format(Output, (ZEUInt64)Argument.GetUInt64(), ArgumentFormat);

		case ZE_VRT_BOOLEAN:
			return Format(Output, Argument.GetBoolean(), ArgumentFormat);

		case ZE_VRT_STRING:
			return Format(Output, Argument.GetString(), ArgumentFormat);

		case ZE_VRT_QUATERNION:
			return Format(Output, Argument.GetQuaternion(), ArgumentFormat);

		case ZE_VRT_VECTOR2:
			return Format(Output, Argument.GetVector2(), ArgumentFormat);

		case ZE_VRT_VECTOR3:
			return Format(Output, Argument.GetVector3(), ArgumentFormat);

		case ZE_VRT_VECTOR4:
			return Format(Output, Argument.GetVector4(), ArgumentFormat);

		case ZE_VRT_MATRIX3X3:
			return Format(Output, Argument.GetMatrix3x3(), ArgumentFormat);

		case ZE_VRT_MATRIX4X4:
			return Format(Output, Argument.GetMatrix4x4(), ArgumentFormat);

		case ZE_VRT_CLASS:
			return Format(Output, Argument.GetClass(), ArgumentFormat);
	}
}

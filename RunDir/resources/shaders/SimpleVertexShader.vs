/*ZEHEADER_START*/
//////////////////////////////////////////////////////////////////////////////////////
//                                                                                  //
//  MapResource.h - Zinek Engine v0.05.00 Build 1024 Source Code                                 
// -------------------------------------------------------------------------------- //
//  Copyright (c) 2007-2009 Y. Or�un G�KBULUT. All rights reserved.                 //
//                                                                                  //
//                 READ TERMS BELLOW BEFORE TAKING ANY ACTION !                     //
//                                                                                  //
//  These coded instructions, statements, and computer programs contain             //
//  unpublished proprietary information belongs to Y. Or�un G�KBULUT and they are   //
//  protected by international copyright laws. This file is intellectual property   //
//  of the Y. Or�un G�KBULUT. Storing this file, using this file, changing content  //
//  of this file, copying or duplicating this file, compiling this file, publishing // 
//  this file to anykind media or network, editing  this file, selling this file,   //
//  booking this file, reading this file are probited without notice and written    //
//  permision of Y. Or�un G�KBULUT.                                                 //
//  These terms at above can only be changed by Y. Or�un G�KBULUT.                  //
//                                                                                  //
//  Contact Information:                                                            //
//     Adress : Zinek Code House & Game Studio                                      //                                             
//              Aydinlar Mahallesi.                                                 //
//              Mimar Sokak. 33/5                                                   //
//              06450  Ankara/TURKEY                                                //
//     Phone  : +90 (533) 734 21 22                                                 //
//     E-Mail : contact@zinekengine.com                                             //
//     WWW    : http://www.zinekengine.com                                          //
//                                                                                  //
//*                                                                                *//
//  DESCRIPTION         :                                                           //
//  AUTHOR(S)           : Y. Or�un G�KBULUT                                         //
//*                                                                                *//
//////////////////////////////////////////////////////////////////////////////////////
/*ZEHEADER_END*/

float4x4	TransformMatrix				: register(c0);

struct VS_INPUT 
{
	float4 Position             : POSITION0;
	float4 Color                : TEXCOORD0;

};

struct VS_OUTPUT 
{
	float4 Position             : POSITION0;
	float4 Color				: TEXCOORD0;
};

VS_OUTPUT vs_main(VS_INPUT Input)
{
	VS_OUTPUT Output;

	Output.Position = mul(Input.Position, TransformMatrix);
	Output.Color = Input.Color;
	
	return Output;
}
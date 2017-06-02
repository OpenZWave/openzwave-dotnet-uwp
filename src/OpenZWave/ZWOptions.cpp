//-----------------------------------------------------------------------------
//
//      ZWOptions.cpp
//
//      CLI/C++ and WinRT wrapper for the C++ OpenZWave Manager class
//
//      Original Work by:
//      Copyright (c) 2010 ) Amer Harb <harb_amer@hotmail.com> (
//      Intial UWP Port by Donald Hanson - https://github.com/donald-hanson
//      Maintained by Morten Nielsen https://github.com/dotMorten
//
//      SOFTWARE NOTICE AND LICENSE
//
//      This file is part of OpenZWave.
//
//      OpenZWave is free software: you can redistribute it and/or modify
//      it under the terms of the GNU Lesser General Public License as published
//      by the Free Software Foundation, either version 3 of the License,
//      or (at your option) any later version.
//
//      OpenZWave is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU Lesser General Public License for more details.
//
//      You should have received a copy of the GNU Lesser General Public License
//      along with OpenZWave.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
#include "pch.h"
#include "ZWOptions.h"

using namespace OpenZWave;

#if !__cplusplus_cli
static ZWOptions^ s_instance = nullptr;
#endif

ZWOptions^ ZWOptions::Instance::get()
{
	if (s_instance == nullptr)
		s_instance = gcnew ZWOptions();
	return s_instance;
}
//-----------------------------------------------------------------------------
//	<ZWOptions::Create>
//	Create the unmanaged Options singleton object
//-----------------------------------------------------------------------------
void ZWOptions::Initialize()
{
#if __cplusplus_cli
	Initialize("config/", Environment::GetFolderPath(Environment::SpecialFolder::LocalApplicationData), "");
#else
	Initialize("config/", Windows::Storage::ApplicationData::Current->LocalFolder->Path, "");
#endif
}

void ZWOptions::Initialize
(
	String^ configPath,
	String^	userPath,
	String^	commandLine
)
{
	// Create the Manager singleton
	std::string config = ConvertString(configPath);
	std::string user = ConvertString(userPath);
	std::string command = ConvertString(commandLine);
	
	Options::Create(config, user, command);
}

//-----------------------------------------------------------------------------
// <ZWOptions::AddOptionBool>
// Add a boolean option to the program
//-----------------------------------------------------------------------------
bool ZWOptions::AddOptionBool
(
	String^ name,
	bool value
)
{
	std::string sname = ConvertString(name);
	return Options::Get()->AddOptionBool(sname, value);
}

//-----------------------------------------------------------------------------
// <ZWOptions::AddOptionInt>
// Add an integer option to the program
//-----------------------------------------------------------------------------
bool ZWOptions::AddOptionInt
(
	String^ name,
	int32 value
)
{
	std::string sname = ConvertString(name);
	return Options::Get()->AddOptionInt(sname, value);
}

//-----------------------------------------------------------------------------
// <ZWOptions::AddOptionString>
// Add a string option to the program
//-----------------------------------------------------------------------------
bool ZWOptions::AddOptionString
(
	String^ name,
	String^ value,
	bool append
)
{
	std::string sname = ConvertString(name);
	std::string defaultStr = ConvertString(value);
	return Options::Get()->AddOptionString(sname, defaultStr, append);
}

//-----------------------------------------------------------------------------
// <ZWOptions::GetOptionAsBool>
// Gets the value of a boolean option
//-----------------------------------------------------------------------------
bool ZWOptions::GetOptionAsBool
(
	String^ name,
	bool *o_value
)
{
	bool value;
	std::string sname = ConvertString(name);
	if (Options::Get()->GetOptionAsBool(sname, &value))
	{
		*o_value = value;
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// <ZWOptions::GetOptionAsInt>
// Gets the value of an integer option
//-----------------------------------------------------------------------------
bool ZWOptions::GetOptionAsInt
(
	String^ name,
	int *o_value
)
{
	int32 value;
	std::string sname = ConvertString(name);
	if (Options::Get()->GetOptionAsInt(sname, &value))
	{
		*o_value = value;
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// <ZWOptions::GetOptionAsString>
// Gets the value of a string option
//-----------------------------------------------------------------------------
bool ZWOptions::GetOptionAsString
(
	String^ name,
	String^ *o_value
)
{
	std::string value;
	std::string sname = ConvertString(name);
	if (Options::Get()->GetOptionAsString(sname, &value))
	{
		*o_value = ConvertStdString(value);
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// <ZWOptions::GetOptionType>
// Gets the type of the value stored by the option
//-----------------------------------------------------------------------------
ZWOptionType ZWOptions::GetOptionType
(
	String^ name
)
{
	std::string sname = ConvertString(name);
	return (ZWOptionType)Options::Get()->GetOptionType(sname);
}
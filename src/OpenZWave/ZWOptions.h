//-----------------------------------------------------------------------------
//
//      ZWOptions.h
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

#pragma once
#include "ZWEnums.h"

using namespace OpenZWave;
#if __cplusplus_cli
using namespace System;
#else
using namespace Platform;
#endif

namespace OpenZWave
{
	/// <summary>
	/// A class that manages program options read from XML files or the command line.
	/// </summary>
	/// <remarks>
	/// The OpenZWave Manager class requires a complete and locked Options
	/// object when created.  The Options are therefore one of the first things that
	/// any OpenZWave application must deal with.
	/// Options are first read from an XML file called options.xml located in the
	/// User data folder (the path to which is supplied to the Options::Create method).
	/// This is the same folder that will be used by the Manager to save the state of
	/// each controller in the Z-Wave network, to avoid querying them for their entire
	/// state every time the application starts up.
	/// The second source of program options is a string, which will normally be the
	/// command line used to launch the application.
	/// In this way, common options can be specified in the XML, but over-ridden if
	/// necessary by the command line.<br/>
	/// The Options process is as follows:<br/>
	/// 1) Create an Options object, providing paths to the OpenZWave config folder,
	/// the User data folder and any command line string containing program options.<br/>
	/// 2) Call Options::AddOptionBool, Options::AddOptionInt or Options::AddOptionString
	/// to add any application-specific configurable options
	/// The OpenZWave options will already have been added during construction of the
	/// Options object.<br/>
	/// 3) Call Options::Lock.  This will cause the option values to be read from
	/// the options.xml file and the command line string, and will lock the options<br/>
	/// so that no more calls aside from GetOptionAs may be made.
	/// 4) Create the OpenZWave Manager object.
	/// </remarks>
	public ref class ZWOptions sealed
	{
	private:
#if __cplusplus_cli
		static ZWOptions^ s_instance = nullptr;
#endif
		ZWOptions() { }
	public:
		/// <summary>Gets the singleton instance of the options</summary>
		static property ZWOptions^ Instance { ZWOptions^ get(); }
		
		/// <summary>Creates an object to manage the program options.</summary>
		/// <param name="_configPath">a string containing the path to the OpenZWave library config
		/// folder, which contains XML descriptions of Z-Wave manufacturers and products.</param>
		/// <param name="_userPath">a string containing the path to the application's user data
		/// folder where the OpenZWave should store the Z-Wave network configuration and state.</param>
		/// <param name="_commandLine">A string containing the program's command line options.
		/// Command line options are parsed after the options.xml file, and so take precedence.
		/// Options are identified by a leading -- (two minus signs). The following items
		/// in the string are treated as values for this option, until the next -- is
		/// reached. For boolean options only, it is possible to omit the value, in which case
		/// the value is assumed to be "true".  Note that option names are case insensitive, and
		/// that option values should be separated by a space.</param>
		/// <remarks>
		/// The _userPath is also the folder where OpenZWave will look for the file Options.xml
		/// which contains program option values.  The file should be in the form outlined below,
		/// with one or more Option elements containing a name and value attribute.  Multiple
		/// values with the same option name should be listed separately. Note that option names
		/// are case insensitive.</remarks>
		/// <code><![CDATA[
		/// <?xml version="1.0" encoding="utf-8"?>
		/// <Options>
		///   <Option name="logging" value="true" />
		///   <Option name="ignore" value="COMMAND_CLASS_BASIC" />
		///   <Option name="ignore" value="COMMAND_CLASS_VERSION" />
		/// </Options>
		/// ]]></code>
		void Initialize(String^ _configPath, String^ _userPath, String^ _commandLine);
		
		/// <summary>Creates an object to manage the program options using the default data paths.</summary>
		void Initialize();

		/// <summary>
		/// Deletes the Options and cleans up any associated objects.
		/// The application is responsible for destroying the Options object,
		/// but this must not be done until after the Manager object has been
		/// destroyed.</summary>
		/// <returns>true if the Options object was destroyed. If the manager
		/// object still exists, this call will return false.</returns>
		/// <seealso cref="Initialize" />
		bool Destroy() { return Options::Destroy(); }

		/// <summary>Locks the options.</summary>
		/// <remarks>Reads in option values from  the XML options file and command line string and
		/// marks the options as locked.  Once locked, no more calls to AddOptionXXX
		/// can be made.<br/>
		/// The options must be locked before the ZWManager.Initialize() method is called.</remarks>
		bool Lock() { return Options::Get()->Lock(); }

		/// <summary>Add a boolean option to the program.</summary>
		/// <remarks>Adds an option to the program whose value can then be read from a file or command line.
		/// All calls to AddOptionInt must be made before Lock.</remarks>
		/// <param name="_name"> the name of the option.  Option names are case insensitive and must be unique.</param>
		/// <param name="_default">the default value for this option.</param>
		/// <seealso cref="GetOptionAsBool" />
		bool AddOptionBool(String^ _name, bool _default);

		/// <summary>Add an integer option to the program.</summary>
		/// <remarks>Adds an option to the program whose value can then be read from a file or command line.
		/// All calls to AddOptionInt must be made before Lock.</remarks>
		/// <param name="_name">the name of the option. Option names are case insensitive and must be unique.</param>
		/// <param name="_default">the default value for this option.</param>
		/// <seealso cref="GetOptionAsInt" />
		bool AddOptionInt(String^ _name, int32 _default);

		/// <summary>Add a string option to the program.</summary>
		/// <remarks>Adds an option to the program whose value can then be read from a file or command line.
		/// All calls to AddOptionString must be made before Lock.</remarks>
		/// <param name="_name">the name of the option. Option names are case insensitive and must be unique.</param>
		/// <param name="_default">the default value for this option.</param>
		/// <param name="_append">Setting append to true will cause values read from the command line</param>
		/// or XML file to be concatenated into a comma delimited list. If _append is false,
		/// newer values will overwrite older ones.</param>
		/// <seealso cref="GetOptionAsString" />
		bool AddOptionString(String^ _name, String^ _default, bool _append);

		/// <summary>Get the value of a boolean option.</summary>
		/// <param name="_name">the name of the option.  Option names are case insensitive.</param>
		/// <param name="o_value">the item that will be filled with the option value.</param>
		/// <returns>true if the option value was fetched successfully, false if the
		/// option does not exist, or does not contain a boolean value</returns>
		bool GetOptionAsBool(String^ _name, bool *o_value);

		/// <summary>Get the value of an integer option.</summary>
		/// <param name="_name">the name of the option.  Option names are case insensitive.</param>
		/// <param name="o_value">the item that will be filled with the option value.</param>
		/// <returns>true if the option value was fetched successfully, false if the
		/// option does not exist, or does not contain an integer value</returns>
		bool GetOptionAsInt(String^ _name, int *o_value);

		/// <summary>Get the value of a string option.</summary>
		/// <param name="_name">the name of the option. Option names are case insensitive.</param>
		/// <param name="o_value">the item that will be filled with the option value.</param>
		/// <returns>true if the option value was fetched successfully, false if the
		/// option does not exist, or does not contain a string value</returns>
		bool GetOptionAsString(String^ _name, String^ *o_value);

		/// <summary>Get the type of value stored in an option.</summary>
		/// <param name="_name">the name of the option.  Option names are case insensitive.</param>
		/// <returns>An enum value representing the type of the option value. If the
		/// option does not exist, OptionType_Invalid is returned.</returns>
		ZWOptionType GetOptionType(String^ _name);

		/// <summary>Test whether the options have been locked.</summary>
		/// <returns><c>true</c> if the options have been locked.</returns>
		/// <seealso cref="Lock" />
		property bool AreLocked { bool get() { return Options::Get()->AreLocked(); } }

	private:
		std::string ConvertString(String^ value) {
#if __cplusplus_cli
			return msclr::interop::marshal_as<std::string>(value);
#else
			std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
			return convert.to_bytes(value->Data());
#endif
		}

		String^ ConvertStdString(std::string value) {
#if __cplusplus_cli
			return gcnew String(value.c_str());
#else
			std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
			std::wstring intermediateForm = convert.from_bytes(value);
			return ref new Platform::String(intermediateForm.c_str());
#endif
		}
	};
}
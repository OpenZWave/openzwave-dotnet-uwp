//-----------------------------------------------------------------------------
//
//      ZWValueID.h
//
//      UWP wrapper for the C++ OpenZWave Manager class
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

#include "ValueID.h"

using namespace OpenZWave;

namespace OpenZWave
{
	/** <summary>The classification of a value to enable low level system or configuration parameters to be filtered by the application.</summary> */
	public enum class ZWValueGenre
	{
		/** <summary>The 'level' as controlled by basic commands.  Usually duplicated by another command class.</summary> */
		Basic = ValueID::ValueGenre_Basic,
		/** <summary>Basic values an ordinary user would be interested in.</summary> */
		User = ValueID::ValueGenre_User,
		/** <summary>Device-specific configuration parameters.  These cannot be automatically discovered via Z-Wave, and are usually described in the user manual instead.</summary> */
		Config = ValueID::ValueGenre_Config,
		/** <summary>Values of significance only to users who understand the Z-Wave protocol.</summary> */
		System = ValueID::ValueGenre_System
	};

	//* <summary>The type of data represented by the value object.</summary>
	public enum class ZWValueType
	{
		//* <summary>Boolean, true or false</summary> */
		Bool = ValueID::ValueType_Bool,
		//* <summary>8-bit unsigned value</summary> */
		Byte = ValueID::ValueType_Byte,			
		//* <summary>Represents a non-integer value as a string, to avoid floating point accuracy issues.</summary> */
		Decimal = ValueID::ValueType_Decimal,
		//* <summary>32-bit signed value</summary> */
		Int = ValueID::ValueType_Int,			   
		//* <summary>List from which one item can be selected</summary> */
		List = ValueID::ValueType_List,			   
		//* <summary>Complex type used with the Climate Control Schedule command class</summary> */
		Schedule = ValueID::ValueType_Schedule,
		//* <summary>16-bit signed value</summary> */
		Short = ValueID::ValueType_Short,
		//* <summaryText string></summary> */
		String = ValueID::ValueType_String,
		//* <summary>A write-only value that is the equivalent of pressing a button to send a command to a device</summary> */
		Button = ValueID::ValueType_Button,
		//* <summary>A collection of bytes</summary> */
		Raw = ValueID::ValueType_Raw		
	};

    public ref class ZWValueID sealed
    {
    public:
		/**
		 * Create a ZWValue ID from its component parts.
		 * This method is provided only to allow ValueIDs to be saved and recreated by the application.  Only
		 * ValueIDs that have been reported by OpenZWave notifications should ever be used.
		 * \param homeId Home ID of the PC Z-Wave Controller that manages the device.
		 * \param nodeId Node ID of the device reporting the value.
		 * \param genre classification of the value to enable low level system or configuration parameters to be filtered out.
		 * \param commandClassId ID of command class that creates and manages this value.
		 * \param instance Instance index of the command class.
		 * \param valueIndex Index of the value within all the values created by the command class instance.
		 * \param type Type of value (bool, byte, string etc).
		 * \return The ValueID.
		 * \see ValueID
		 */
		ZWValueID
		(
			uint32 homeId,
			uint8 nodeId,
			ZWValueGenre genre,
			uint8 commandClassId,
			uint8 instance,
			uint8 valueIndex,
			ZWValueType type,
			uint8 pollIntensity
		)
		{
			m_valueId = new ValueID(homeId, nodeId, (ValueID::ValueGenre)genre, commandClassId, instance, valueIndex, (ValueID::ValueType)type);
			
		}

		uint32		GetHomeId() { return m_valueId->GetHomeId(); }
		uint8		GetNodeId() { return m_valueId->GetNodeId(); }
		ZWValueGenre	GetGenre() { return (ZWValueGenre)m_valueId->GetGenre(); }
		uint8		GetCommandClassId() { return m_valueId->GetCommandClassId(); }
		uint8		GetInstance() { return m_valueId->GetInstance(); }
		uint8		GetIndex() { return m_valueId->GetIndex(); }
		ZWValueType	GetType() { return (ZWValueType)m_valueId->GetType(); }
		uint64		GetId() { return m_valueId->GetId(); }

	private:
		ValueID* m_valueId;
		~ZWValueID()
		{
			delete m_valueId;
		}

	internal:
		ZWValueID(ValueID const& valueId)
		{
			m_valueId = new ValueID(valueId);
		}

		ValueID CreateUnmanagedValueID() { return ValueID(*m_valueId); }

		// Comparison Operators
		bool operator ==	(ZWValueID^ _other) { return((*m_valueId) == (*_other->m_valueId)); }
		bool operator !=	(ZWValueID^ _other) { return((*m_valueId) != (*_other->m_valueId)); }
		bool operator <		(ZWValueID^ _other) { return((*m_valueId) < (*_other->m_valueId)); }
		bool operator >		(ZWValueID^ _other) { return((*m_valueId) > (*_other->m_valueId)); }
    };
}

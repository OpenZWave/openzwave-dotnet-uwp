//-----------------------------------------------------------------------------
//
//      ZWValueID.h
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

namespace OpenZWave
{
	/// <summary>Provides a unique ID for a value reported by a Z-Wave device.</summary>
	/// <remarks>
    /// <para>The ValueID is used to uniquely identify a value reported by a Z-Wave device.</para>
    /// <para>The ID is built by packing various identifying characteristics into a single 32 - bit number - the
	/// Z-Wave driver index, device node ID, the command class and command class instance that handles the value,
	/// plus an index for the value to distinguish it among all the other values managed by that command class
	/// instance.The type (bool, byte, string etc) of the value is also stored.</para>
    /// <para>The packing of the ID is such that a list of Values sorted by ValueID will be in a sensible order for display to the user.</para>
	/// </remarks>
    public ref class ZWValueID sealed
    {
    public:		
		/// <summary>Create a ZWValue ID from its component parts.</summary>
		/// <remarks>This method is provided only to allow ValueIDs to be saved and recreated by the application. Only
		/// ValueIDs that have been reported by OpenZWave notifications should ever be used.</remarks>
		/// <param name="homeId">Home ID of the PC Z-Wave Controller that manages the device.</param>
		/// <param name="nodeId">Node ID of the device reporting the value.</param>
		/// <param name="genre">classification of the value to enable low level system or configuration parameters to be filtered out.</param>
		/// <param name="commandClassId">ID of command class that creates and manages this value.</param>
		/// <param name="instance">Instance index of the command class.</param>
		/// <param name="valueIndex">Index of the value within all the values created by the command class instance.</param>
		/// <param name="type">Type of value (bool, byte, string etc).</param>
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

		/// <summary>Gets the Home ID of the driver that controls the node containing the value.</summary>
		property uint32 HomeId { uint32 get() { return m_valueId->GetHomeId(); } }

		/// <summary>Gets the Node ID of the node containing the value.</summary>
		property uint8	NodeId { uint8 get() { return m_valueId->GetNodeId(); } }

		/// <summary>Get the genre of the value.  The genre classifies a value to enable
		/// low - level system or configuration parameters to be filtered out by the application</summary>
		property ZWValueGenre Genre { ZWValueGenre get() { return (ZWValueGenre)m_valueId->GetGenre(); } }

		/// <summary>Get the Z-Wave command class that created and manages this value.  Knowledge of 
		/// command classes is not required to use OpenZWave, but this information is
		/// exposed in case it is of interest.</summary>
		property uint8 CommandClassId { uint8 get() { return m_valueId->GetCommandClassId(); } }

		/// <summary>Get the command class instance of this value.  It is possible for there to be
		/// multiple instances of a command class, although currently it appears that
		/// only the SensorMultilevel command class ever does this.  Knowledge of
		/// instances and command classes is not required to use OpenZWave, but this
		/// information is exposed in case it is of interest.</summary>
		property uint8 Instance { uint8 get() { return m_valueId->GetInstance(); } }
		
		/// <summary>Get the value index.  The index is used to identify one of multiple
		/// values created and managed by a command class.  In the case of configurable
		/// parameters (handled by the configuration command class), the index is the
		/// same as the parameter ID.  Knowledge of command classes is not required
		/// to use OpenZWave, but this information is exposed in case it is of interest.</summary>
		property uint16 Index { uint16 get() { return m_valueId->GetIndex(); } }
		
		/// <summary>Get the type of the value.  The type describes the data held by the value
		/// and enables the user to select the correct value accessor method in the
		/// Manager class.</summary>
		property ZWValueType Type { ZWValueType get() { return (ZWValueType)m_valueId->GetType(); } }
		
		/// <summary>Get a 64Bit Integer that represents this ValueID. This Integer is not guaranteed to be valid
		/// across restarts of OpenZWave.</summary>
		property uint64	Id { uint64 get() { return m_valueId->GetId(); } }

	private:
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

	private:
		ValueID* m_valueId;
    };
}

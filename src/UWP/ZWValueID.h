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
#include "ZWEnums.h"

using namespace OpenZWave;

namespace OpenZWave
{
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

		property uint32 HomeId { uint32 get() { return m_valueId->GetHomeId(); } }
		property uint8	NodeId { uint8 get() { return m_valueId->GetNodeId(); } }
		property ZWValueGenre Genre { ZWValueGenre get() { return (ZWValueGenre)m_valueId->GetGenre(); } }
		property uint8 CommandClassId { uint8 get() { return m_valueId->GetCommandClassId(); } }
		property uint8 Instance { uint8 get() { return m_valueId->GetInstance(); } }
		property uint8 Index { uint8 get() { return m_valueId->GetIndex(); } }
		property ZWValueType Type { ZWValueType get() { return (ZWValueType)m_valueId->GetType(); } }
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

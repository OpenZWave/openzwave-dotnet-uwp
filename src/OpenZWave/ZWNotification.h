//-----------------------------------------------------------------------------
//
//      ZWNotification.h
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
#include "ZWValueID.h"

using namespace OpenZWave;

namespace OpenZWave
{
	/// <summary>
	/// Provides a container for data sent via the notification callback
	/// handler from the ZWManager.OnNotification event.
	// </summary>
	public ref class ZWNotification sealed
	{
	internal:
		ZWNotification(Notification* notification)
		{
			m_type = (NotificationType)notification->GetType();
			m_byte = notification->GetByte();

			//Check if notification is either NodeEvent or ControllerCommand, otherwise GetEvent() will fail
			if ((m_type == NotificationType::NodeEvent) || (m_type == NotificationType::ControllerCommand))
			{
				m_event = notification->GetEvent();
			}

			m_valueId = gcnew ZWValueID(notification->GetValueID());
		}

	public:
		/// <summary>Get the type of this notification.</summary>
		property NotificationType Type { NotificationType get() { return m_type; } }
		/// <summary>Gets the notification code</summary>
		property NotificationCode Code { NotificationCode get() { return (NotificationCode)m_byte; } }
		/// <summary>Get the Home ID of the driver sending this notification.</summary>
		property uint32 HomeId { uint32 get() { return m_valueId->HomeId; } }
		/// <summary>Get the ID of any node involved in this notification.</summary>
		property uint8 NodeId { uint8 get() { return m_valueId->NodeId; } }
		/// <summary>Get the unique ValueID of any value involved in this notification.</summary>
		property ZWValueID^ ValueID { ZWValueID^ get() { return m_valueId; } }
		/// <summary>Get the index of the association group that has been changed.  Only valid in Notification::Type_Group notifications.</summary>
		property uint8 GroupIdx { uint8 get() { assert(NotificationType::Group == m_type); return m_byte; } }
		/// <summary>Get the event value of a notification.  Only valid in Notification::Type_NodeEvent and Notification::Type_ControllerCommand notifications.</summary>
		property uint8 Event { uint8 get() { return m_event; } }

	private:
		NotificationType		m_type;
		ZWValueID^	m_valueId;
		uint8		m_byte;
		uint8		m_event;
	};
}

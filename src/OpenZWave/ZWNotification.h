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
#include "ZWValueId.h"

using namespace OpenZWave;

namespace OpenZWave
{
	ref class ZWNotification;

	/// <summary>
	/// The event args returned by the ZWManager.NotificationReceived event
	/// </summary>
	public ref class NotificationReceivedEventArgs sealed
	{
	internal:
		NotificationReceivedEventArgs(ZWNotification^ notification) : m_notification(notification)
		{
		}
	public:
		/// <summary>Get the notification from the event argument.</summary>
		property ZWNotification^ Notification { ZWNotification^ get() { return m_notification; } }

	private:
		ZWNotification^ m_notification;
	};

	/// <summary>
	/// Provides a container for data sent via the notification callback
	/// handler from the ZWManager.NotificationReceived event.
	/// </summary>
	public ref class ZWNotification sealed
	{
	internal:
		ZWNotification(Notification* notification)
		{
			m_type = (ZWNotificationType)notification->GetType();
			m_byte = notification->GetByte();

			//Check if notification is either NodeEvent or ControllerCommand, otherwise GetEvent() will fail
			if ((m_type == ZWNotificationType::NodeEvent) || (m_type == ZWNotificationType::ControllerCommand))
			{
				m_event = notification->GetEvent();
			}

			m_valueId = gcnew ZWValueId(notification->GetValueID());
		}

	public:
		/// <summary>Gets the type of notification.</summary>
		property ZWNotificationType Type { ZWNotificationType get() { return m_type; } }
		/// <summary>Gets the notification code</summary>
		property ZWNotificationCode Code { ZWNotificationCode get() { return (ZWNotificationCode)m_byte; } }
		/// <summary>Gets the Home ID of the driver sending this notification.</summary>
		property uint32 HomeId { uint32 get() { return m_valueId->HomeId; } }
		/// <summary>Gets the ID of any node involved in this notification.</summary>
		property uint8 NodeId { uint8 get() { return m_valueId->NodeId; } }
		/// <summary>Gets the unique ValueID of any value involved in this notification.</summary>
		property ZWValueId^ ValueId { ZWValueId^ get() { return m_valueId; } }
		/// <summary>Gets the index of the association group that has been changed.  Only valid in ZWNotificationType.Group notifications.</summary>
		property uint8 GroupIndex { uint8 get() { assert(ZWNotificationType::Group == m_type); return m_byte; } }
		/// <summary>Gets the event value of a notification.  Only valid in Notification::Type_NodeEvent and Notification::Type_ControllerCommand notifications.</summary>
		property uint8 Event { uint8 get() { return m_event; } }

	private:
		ZWNotificationType		m_type;
		ZWValueId^	m_valueId;
		uint8		m_byte;
		uint8		m_event;
	};
}

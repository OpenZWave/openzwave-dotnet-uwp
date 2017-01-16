//-----------------------------------------------------------------------------
//
//      ZWNotification.h
//
//      Cli/C++ wrapper for the C++ OpenZWave Manager class
//
//      Copyright (c) 2010 Amer Harb <harb_amer@hotmail.com>
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
#include "Windows.h"
#include "Manager.h"
#include "Value.h"
#include "ValueStore.h"
#include "ValueID.h"
#include "ValueBool.h"
#include "ValueInt.h"
#include "ValueByte.h"
#include "ValueString.h"
#include "ValueShort.h"
#include "ValueDecimal.h"
#include "Notification.h"
#include "stdio.h"

#include <msclr/auto_gcroot.h>
#include <msclr/lock.h>

using namespace System;
using namespace System::Threading;
using namespace System::Collections::Generic;
using namespace OpenZWave;
using namespace Runtime::InteropServices;

namespace OpenZWaveDotNet
{
	public ref class ZWNotification
	{
	public:
		enum class Type
		{
		/** <summary>A new node value has been added to OpenZWave's list. These notifications occur after a node has been discovered, and details of its command classes have been received.  Each command class may generate one or more values depending on the complexity of the item being represented.</summary> */
			ValueAdded						= Notification::Type_ValueAdded,	
		/** <summary>A node value has been removed from OpenZWave's list.  This only occurs when a node is removed.</summary> */
			ValueRemoved					= Notification::Type_ValueRemoved,		
		/** <summary>A node value has been updated from the Z-Wave network and it is different from the previous value.</summary> */
			ValueChanged					= Notification::Type_ValueChanged,		
		/** <summary>A node value has been updated from the Z-Wave network.</summary> */
			ValueRefreshed					= Notification::Type_ValueRefreshed,
		/** <summary>The associations for the node have changed. The application should rebuild any group information it holds about the node.</summary> */
			Group							= Notification::Type_Group,				
		/** <summary>A new node has been found (not already stored in zwcfg*.xml file) </summary>*/
			NodeNew							= Notification::Type_NodeNew,
		/** <summary>A new node has been added to OpenZWave's list.  This may be due to a device being added to the Z-Wave network, or because the application is initializing itself.</summary> */
			NodeAdded						= Notification::Type_NodeAdded,			
		/** <summary>A node has been removed from OpenZWave's list.  This may be due to a device being removed from the Z-Wave network, or because the application is closing.</summary> */
			NodeRemoved						= Notification::Type_NodeRemoved,
		/** <summary>Basic node information has been received, such as whether the node is a listening device, a routing device and its baud rate and basic, generic and specific types. It is after this notification that you can call Manager::GetNodeType to obtain a label containing the device description.</summary> */
			NodeProtocolInfo				= Notification::Type_NodeProtocolInfo,
		/** <summary> One of the node names has changed (name, manufacturer, product).</summary> */
			NodeNaming						= Notification::Type_NodeNaming,
		/** <summary>A node has triggered an event.  This is commonly caused when a node sends a Basic_Set command to the controller.  The event value is stored in the notification.</summary> */
			NodeEvent						= Notification::Type_NodeEvent,
		/** <summary></summary> */
			PollingDisabled					= Notification::Type_PollingDisabled,	
		/** <summary></summary> */
			PollingEnabled					= Notification::Type_PollingEnabled,	
		/** <summary></summary> */
			SceneEvent						= Notification::Type_SceneEvent,
		/** <summary></summary> */
			CreateButton					= Notification::Type_CreateButton,
		/** <summary></summary> */
			DeleteButton					= Notification::Type_DeleteButton,
		/** <summary></summary> */
			ButtonOn						= Notification::Type_ButtonOn,
		/** <summary></summary> */
			ButtonOff						= Notification::Type_ButtonOff,
		/** <summary>A driver for a PC Z-Wave controller has been added and is ready to use.  The notification will contain the controller's Home ID, which is needed to call most of the Manager methods.</summary> */
			DriverReady						= Notification::Type_DriverReady,		
		/** <summary>Driver failed to load</summary> */
			DriverFailed					= Notification::Type_DriverFailed,
		/** <summary>All nodes and values for this driver have been removed.  This is sent instead of potentially hundreds of individual node and value notifications.</summary> */
			DriverReset						= Notification::Type_DriverReset,
		/** <summary>The queries on a node that are essential to its operation have been completed. The node can now handle incoming messages.</summary> */
			EssentialNodeQueriesComplete	= Notification::Type_EssentialNodeQueriesComplete,
		/** <summary>All the initialization queries on a node have been completed.</summary> */
			NodeQueriesComplete				= Notification::Type_NodeQueriesComplete,
		/** <summary></summary> */
			AwakeNodesQueried				= Notification::Type_AwakeNodesQueried,
		/** <summary></summary> */
			AllNodesQueriedSomeDead			= Notification::Type_AllNodesQueriedSomeDead,
		/** <summary></summary> */
			AllNodesQueried					= Notification::Type_AllNodesQueried,
		/** <summary></summary> */
			Notification					= Notification::Type_Notification,
		/** <summary></summary> */
			DriverRemoved					= Notification::Type_DriverRemoved,
		/** <summary></summary> */
		ControllerCommand = Notification::Type_ControllerCommand,
		/** <summary>The Device has been reset and thus removed from the NodeList in OZW</summary> */
		NodeReset = Notification::Type_NodeReset,
		/** <summary>Warnings and Notifications Generated by the library that should be displayed to the user (eg, out of date config files) </summary>*/
		UserAlerts = Notification::Type_UserAlerts,					
		/** <summary>The ManufacturerSpecific Database Is Ready</summary> */
		ManufacturerSpecificDBReady = Notification::Type_ManufacturerSpecificDBReady		
		};

	public:
		enum class Code
		{
			MsgComplete = Notification::Code_MsgComplete,
			Timeout = Notification::Code_Timeout,
			NoOperation = Notification::Code_NoOperation,
			Awake = Notification::Code_Awake,
			Sleep = Notification::Code_Sleep,
			Dead = Notification::Code_Dead,
			Alive = Notification::Code_Alive
		};

		ZWNotification( Notification* notification )
		{
			m_type = (Type)Enum::ToObject( Type::typeid, notification->GetType() );
			m_byte = notification->GetByte();

			//Check if notification is either NodeEvent or ControllerCommand, otherwise GetEvent() will fail
			if ((m_type == Type::NodeEvent) || (m_type == Type::ControllerCommand))
			{
				m_event = notification->GetEvent();
			}			

			m_valueId = gcnew ZWValueID( notification->GetValueID() );
		}

		Type GetType(){ return m_type; }
		Code GetCode() { return (Code)Enum::ToObject( Code::typeid, m_byte); }
		uint32 GetHomeId(){ return m_valueId->GetHomeId(); }
		uint8 GetNodeId(){ return m_valueId->GetNodeId(); }
		ZWValueID^ GetValueID(){ return m_valueId; }
		uint8 GetGroupIdx(){ assert(Type::Group==m_type); return m_byte; } 
		uint8 GetEvent(){ return m_event; }
		uint8 GetByte(){ return m_byte; }

	internal:
		Type		m_type;
		ZWValueID^	m_valueId;
		uint8		m_byte;
		uint8		m_event;
	};
}

//-----------------------------------------------------------------------------
//
//      ZWManager.cpp
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

#include "pch.h"
#include "ZWManager.h"

using namespace OpenZWave;

#if !__cplusplus_cli
static ZWManager^ s_instance = nullptr;
#endif

ZWManager^ ZWManager::Instance::get()
{
	if (s_instance == nullptr)
		s_instance = gcnew ZWManager();
	return s_instance;
}

//-----------------------------------------------------------------------------
//	<ZWManager::Create>
//	Create the unmanaged Manager singleton object, and add a watcher
//-----------------------------------------------------------------------------
void ZWManager::Initialize()
{
	if (m_isInitialized)
		return;
	// Create the Manager singleton
	Manager::Create();

	// Add a notification handler
#if __cplusplus_cli
	m_onNotification = gcnew OnNotificationFromUnmanagedDelegate(this, &ZWManager::OnNotificationFromUnmanaged);
	m_gchNotification = GCHandle::Alloc(m_onNotification);
	IntPtr ip = Marshal::GetFunctionPointerForDelegate(m_onNotification);
	Manager::Get()->AddWatcher((Manager::pfnOnNotification_t)ip.ToPointer(), NULL);
#else
	Manager::Get()->AddWatcher(OnNotificationFromUnmanaged, reinterpret_cast<void*>(this));
#endif

	m_isInitialized = true;
}

//-----------------------------------------------------------------------------
//	<ZWManager::OnNotificationFromUnmanaged>
//	Trigger an event from the unmanaged notification callback
//-----------------------------------------------------------------------------
#if __cplusplus_cli


//-----------------------------------------------------------------------------
//	<ZWManager::OnNotificationFromUnmanaged>
//	Trigger an event from the unmanaged notification callback
//-----------------------------------------------------------------------------
void ZWManager::OnNotificationFromUnmanaged
(
	Notification* _notification,
	void* _context
)
{
	ZWNotification^ notification = gcnew ZWNotification(_notification);
	OnNotification(notification);
}

#else
void ZWManager::OnNotificationFromUnmanaged(Notification const* _notification, void* _context)
{
	ZWManager^ manager = reinterpret_cast<ZWManager^>(_context);
	ZWNotification^ notification = gcnew ZWNotification((Notification *)_notification);
	manager->OnNotification(notification);
}
#endif

//-----------------------------------------------------------------------------
// <ZWManager::GetValueAsBool>
// Gets a value as a Bool
//-----------------------------------------------------------------------------
bool ZWManager::GetValueAsBool
(
	ZWValueID^ id,
#if __cplusplus_cli
	[Out] System::Boolean %
#else
	bool *
#endif
	o_value
)
{
	bool value;
	if (Manager::Get()->GetValueAsBool(id->CreateUnmanagedValueID(), &value))
	{
#if __cplusplus_cli
		o_value = value;
#else
		*o_value = value;
#endif
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// <ZWManager::GetValueAsByte>
// Gets a value as a Byte
//-----------------------------------------------------------------------------
bool ZWManager::GetValueAsByte
(
	ZWValueID^ id,
#if __cplusplus_cli
	[Out] System::Byte %
#else
	byte *
#endif
	o_value
)
{
	uint8 value;
	if (Manager::Get()->GetValueAsByte(id->CreateUnmanagedValueID(), &value))
	{
#if __cplusplus_cli
		o_value = value;
#else
		*o_value = value;
#endif
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// <ZWManager::GetValueAsDecimal>
// Gets a value as a Decimal
//-----------------------------------------------------------------------------
/*
bool ZWManager::GetValueAsDecimal
(
	ZWValueID^ id,
	[Out] System::Decimal %o_value
)
{
	string value;
	if (Manager::Get()->GetValueAsString(id->CreateUnmanagedValueID(), &value))
	{
		String^ decimal = gcnew String(value.c_str());
		o_value = Decimal::Parse(decimal);
		return true;
	}
	return false;
}
*/

//-----------------------------------------------------------------------------
// <ZWManager::GetValueAsInt>
// Gets a value as an Int32
//-----------------------------------------------------------------------------
bool ZWManager::GetValueAsInt
(
	ZWValueID^ id,
#if __cplusplus_cli
	[Out] System::Int32 %
#else
	int32 *
#endif
	o_value
)
{
	int32 value;
	if (Manager::Get()->GetValueAsInt(id->CreateUnmanagedValueID(), &value))
	{
#if __cplusplus_cli
		o_value = value;
#else
		*o_value = value;
#endif
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// <ZWManager::GetValueAsShort>
// Gets a value as an Int16
//-----------------------------------------------------------------------------
bool ZWManager::GetValueAsShort
(
	ZWValueID^ id,
#if __cplusplus_cli
	[Out] System::Int16 %
#else
	int16 *
#endif
	o_value
)
{
	int16 value;
	if (Manager::Get()->GetValueAsShort(id->CreateUnmanagedValueID(), &value))
	{
#if __cplusplus_cli
		o_value = value;
#else
		*o_value = value;
#endif
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// <ZWManager::GetValueAsString>
// Gets a value as a String
//-----------------------------------------------------------------------------
bool ZWManager::GetValueAsString
(
	ZWValueID^ id,
#if __cplusplus_cli
	[Out] String^ %
#else
	String^ *
#endif
	o_value
)
{
	string value;
	if (Manager::Get()->GetValueAsString(id->CreateUnmanagedValueID(), &value))
	{
#if __cplusplus_cli
		o_value = ConvertString(value);
#else
		*o_value = ConvertString(value);
#endif
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// <ZWManager::GetValueListSelection>
// Gets the selected item from a list value (returning a string)
//-----------------------------------------------------------------------------
bool ZWManager::GetValueListSelection
(
	ZWValueID^ id,
#if __cplusplus_cli
	[Out] String^ %
#else
	String^ *
#endif
	o_value
)
{
	string value;
	if (Manager::Get()->GetValueListSelection(id->CreateUnmanagedValueID(), &value))
	{
#if __cplusplus_cli
		o_value = ConvertString(value);
#else
		*o_value = ConvertString(value);
#endif
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// <ZWManager::GetValueListSelection>
// Gets the selected item from a list value (returning the value)
//-----------------------------------------------------------------------------
bool ZWManager::GetValueListSelection
(
	ZWValueID^ id,
#if __cplusplus_cli
	[Out] System::Int32 %
#else
	int32 *
#endif
	o_value
)
{
	int32 value;
	if (Manager::Get()->GetValueListSelection(id->CreateUnmanagedValueID(), &value))
	{
#if __cplusplus_cli
		o_value = value;
#else
		*o_value = value;
#endif
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// <ZWManager::GetValueListItems>
// Gets the list of items from a list value
//-----------------------------------------------------------------------------
bool ZWManager::GetValueListItems
(
	ZWValueID^ id,
#if __cplusplus_cli
	[Out] cli::array<String^>^ %o_value
)
{
	vector<string> items;
	if (Manager::Get()->GetValueListItems(id->CreateUnmanagedValueID(), &items))
	{
		o_value = gcnew cli::array<String^>(items.size());
		for (uint32 i = 0; i<items.size(); ++i)
		{
			o_value[i] = gcnew String(items[i].c_str());
		}
		return true;
	}
	return false;
}
#else
	Platform::Array<String^>^ *o_value
)
{
	vector<string> items;
	if (Manager::Get()->GetValueListItems(id->CreateUnmanagedValueID(), &items))
	{
		Platform::Array<String^>^ arr = gcnew Platform::Array<String^>(items.size());
		for (uint32 i = 0; i<items.size(); ++i)
		{
			arr[i] = ConvertString(items[i]);
		}
		*o_value = arr;
		return true;
	}
	return false;
}
#endif
//-----------------------------------------------------------------------------
// <ZWManager::GetValueListValues>
// Gets the list of values from a list value
//-----------------------------------------------------------------------------
bool ZWManager::GetValueListValues
(
	ZWValueID^ id,
#if __cplusplus_cli
	[Out] cli::array<int>^ %o_value
)
{
	vector<int32> items;
	if (Manager::Get()->GetValueListValues(id->CreateUnmanagedValueID(), &items))
	{
		o_value = gcnew cli::array<int>(items.size());
		for (uint32 i = 0; i<items.size(); ++i)
		{
			o_value[i] = items[i];
		}
		return true;
	}
	return false;
}

#else
	Platform::Array<int>^ *o_value
)
{
	vector<int32> items;
	if (Manager::Get()->GetValueListValues(id->CreateUnmanagedValueID(), &items))
	{
		Platform::Array<int>^ arr = gcnew Platform::Array<int>(items.size());
		for (uint32 i = 0; i<items.size(); ++i)
		{
			arr[i] = items[i];
		}
		*o_value = arr;
		return true;
	}
	return false;
}
#endif

//-----------------------------------------------------------------------------
// <ZWManager::GetNeighbors>
// Gets the neighbors for a node
//-----------------------------------------------------------------------------
uint32 ZWManager::GetNodeNeighbors
(
	uint32 homeId,
	uint8 nodeId,
#if __cplusplus_cli
	[Out] cli::array<Byte>^ %o_neighbors
)
{
	uint8* neighbors;
	uint32 numNeighbors = Manager::Get()->GetNodeNeighbors(homeId, nodeId, &neighbors);
	if (numNeighbors)
	{
		o_neighbors = gcnew cli::array<Byte>(numNeighbors);
		for (uint32 i = 0; i<numNeighbors; ++i)
		{
			o_neighbors[i] = neighbors[i];
		}
		delete[] neighbors;
	}

	return numNeighbors;
}
#else
	Platform::Array<byte>^ *o_neighbors
)
{
	uint8* neighbors;
	uint32 numNeighbors = Manager::Get()->GetNodeNeighbors(homeId, nodeId, &neighbors);
	if (numNeighbors)
	{
		
		Platform::Array<byte>^ arr = gcnew Platform::Array<byte>(numNeighbors);;
		for (uint32 i = 0; i<numNeighbors; ++i)
		{
			arr[i] = neighbors[i];
		}
		*o_neighbors = arr;
		delete[] neighbors;
	}

	return numNeighbors;
}
#endif


bool ZWManager::GetNodeClassInformation
(
	uint32 homeId,
	uint8 nodeId,
	uint8 commandClassId,
#if __cplusplus_cli
	[Out] String^ %className, [Out] System::Byte %classVersion
#else
	String^ *className, byte *classVersion
#endif
)
{
	string value;
	uint8 version;
	if (Manager::Get()->GetNodeClassInformation(homeId, nodeId, commandClassId, &value, &version))
	{
#if __cplusplus_cli
		className = gcnew String(value.c_str());
		classVersion = version;
#else
		*className = ConvertString(value);
		*classVersion = version;
#endif
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// <ZWManager::GetSwitchPoint>
// Get switchpoint data from the schedule
//-----------------------------------------------------------------------------
bool ZWManager::GetSwitchPoint
(
	ZWValueID^ id,
	uint8 idx,
#if __cplusplus_cli
	[Out] System::Byte %o_hours, [Out] System::Byte %o_minutes, [Out] System::SByte %o_setback
#else
	byte *o_hours, byte *o_minutes, byte *o_setback
#endif
)
{
	uint8 hours;
	uint8 minutes;
	int8 setback;
	if (Manager::Get()->GetSwitchPoint(id->CreateUnmanagedValueID(), idx, &hours, &minutes, &setback))
	{
#if __cplusplus_cli
		o_hours = hours;
		o_minutes = minutes;
		o_setback = setback;
#else
		*o_hours = hours;
		*o_minutes = minutes;
		*o_setback = setback;
#endif
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// <ZWManager::GetAssociations>
// Gets the associations for a group
//-----------------------------------------------------------------------------
uint32 ZWManager::GetAssociations
(
	uint32 homeId,
	uint8 nodeId,
	uint8 groupIdx,
#if __cplusplus_cli
	[Out] cli::array<Byte>^ %o_associations
)
{
	uint8* associations;
	uint32 numAssociations = Manager::Get()->GetAssociations(homeId, nodeId, groupIdx, &associations);
	if (numAssociations)
	{
		o_associations = gcnew cli::array<Byte>(numAssociations);
		for (uint32 i = 0; i<numAssociations; ++i)
		{
			o_associations[i] = associations[i];
		}
		delete[] associations;
	}

	return numAssociations;
}

#else
	Platform::Array<byte>^ *o_associations
)
{
	uint8* associations;
	uint32 numAssociations = Manager::Get()->GetAssociations(homeId, nodeId, groupIdx, &associations);
	if (numAssociations)
	{
		Platform::Array<byte>^ arr = gcnew Platform::Array<byte>(numAssociations);
		for (uint32 i = 0; i<numAssociations; ++i)
		{
			arr[i] = associations[i];
		}
		*o_associations = arr;
		delete[] associations;
	}

	return numAssociations;
}
#endif


//-----------------------------------------------------------------------------
// <ZWManager::GetAllScenes>
// Gets a list of all the SceneIds
//-----------------------------------------------------------------------------
uint8 ZWManager::GetAllScenes
(
#if __cplusplus_cli
	[Out] cli::array<Byte>^ %o_sceneIds
)
{
	uint8* sceneIds;
	uint32 numScenes = Manager::Get()->GetAllScenes(&sceneIds);
	if (numScenes)
	{
		o_sceneIds = gcnew cli::array<Byte>(numScenes);
		for (uint32 i = 0; i<numScenes; ++i)
		{
			o_sceneIds[i] = sceneIds[i];
		}
		delete[] sceneIds;
	}

	return numScenes;
}

#else
	Platform::WriteOnlyArray<byte>^ o_sceneIds
)
{
	uint8* sceneIds;
	uint32 numScenes = Manager::Get()->GetAllScenes(&sceneIds);
	if (numScenes)
	{
		o_sceneIds = gcnew Platform::Array<byte>(numScenes);
		for (uint32 i = 0; i<numScenes; ++i)
		{
			o_sceneIds[i] = sceneIds[i];
		}
		delete[] sceneIds;
	}

	return numScenes;
}
#endif

//-----------------------------------------------------------------------------
// <ZWManager::SceneGetValues>
// Retrieves the scene's list of values
//-----------------------------------------------------------------------------
int ZWManager::SceneGetValues
(
	uint8 sceneId,
#if __cplusplus_cli
	[Out] cli::array<ZWValueID ^>^ %o_values
)
{
	vector<ValueID> values;
	uint32 numValues = Manager::Get()->SceneGetValues(sceneId, &values);
	if (numValues)
	{
		o_values = gcnew cli::array<ZWValueID ^>(numValues);
		for (uint32 i = 0; i<numValues; ++i)
		{
			o_values[i] = gcnew ZWValueID(values[i]);
		}
	}

	return numValues;
}
#else
	Platform::WriteOnlyArray<ZWValueID^>^ o_values
)
{
	vector<ValueID> values;
	uint32 numValues = Manager::Get()->SceneGetValues(sceneId, &values);
	if (numValues)
	{
		o_values = gcnew Platform::Array<ZWValueID^>(numValues);
		for (uint32 i = 0; i<numValues; ++i)
		{
			o_values[i] = gcnew ZWValueID(values[i]);
		}
	}

	return numValues;
}
#endif

//-----------------------------------------------------------------------------
// <ZWManager::SceneGetValueAsBool>
// Retrieves a scene's value as a bool
//-----------------------------------------------------------------------------
bool ZWManager::SceneGetValueAsBool
(
	uint8 sceneId,
	ZWValueID^ valueId,
	bool *o_value
)
{
	bool value;
	if (Manager::Get()->SceneGetValueAsBool(sceneId, valueId->CreateUnmanagedValueID(), &value))
	{
		*o_value = value;
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// <ZWManager::SceneGetValueAsByte>
// Retrieves a scene's value as an 8-bit unsigned integer
//-----------------------------------------------------------------------------
bool ZWManager::SceneGetValueAsByte
(
	uint8 sceneId,
	ZWValueID^ valueId,
	byte *o_value
)
{
	uint8 value;
	if (Manager::Get()->SceneGetValueAsByte(sceneId, valueId->CreateUnmanagedValueID(), &value))
	{
		*o_value = value;
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// <ZWManager::SceneGetValueAsDecimal>
// Retrieves a scene's value as a decimal
//-----------------------------------------------------------------------------
/*
bool ZWManager::SceneGetValueAsDecimal
(
	uint8 sceneId,
	ZWValueID^ valueId,
	[Out] System::Decimal %o_value
)
{
	string value;
	if (Manager::Get()->SceneGetValueAsString(sceneId, valueId->CreateUnmanagedValueID(), &value))
	{
		String^ decimal = gcnew String(value.c_str());
		o_value = Decimal::Parse(decimal);
		return true;
	}
	return false;
}
*/

//-----------------------------------------------------------------------------
// <ZWManager::SceneGetValueAsInt>
// Retrieves a scene's value as a 32-bit signed integer
//-----------------------------------------------------------------------------
bool ZWManager::SceneGetValueAsInt
(
	uint8 sceneId,
	ZWValueID^ valueId,
	int *o_value
)
{
	int32 value;
	if (Manager::Get()->SceneGetValueAsInt(sceneId, valueId->CreateUnmanagedValueID(), &value))
	{
		*o_value = value;
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// <ZWManager::SceneGetValueAsShort>
// Retrieves a scene's value as a 16-bit signed integer
//-----------------------------------------------------------------------------
bool ZWManager::SceneGetValueAsShort
(
	uint8 sceneId,
	ZWValueID^ valueId,
	int16 *o_value
)
{
	int16 value;
	if (Manager::Get()->SceneGetValueAsShort(sceneId, valueId->CreateUnmanagedValueID(), &value))
	{
		*o_value = value;
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// <ZWManager::SceneGetValueAsString>
// Retrieves a scene's value as a string
//-----------------------------------------------------------------------------
bool ZWManager::SceneGetValueAsString
(
	uint8 sceneId,
	ZWValueID^ valueId,
	String^ *o_value
)
{
	string value;
	if (Manager::Get()->SceneGetValueAsString(sceneId, valueId->CreateUnmanagedValueID(), &value))
	{
		*o_value = ConvertString(value);
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// <ZWManager::SceneGetValueListSelection>
// Retrieves a scene's value in a list (as a string)
//-----------------------------------------------------------------------------
bool ZWManager::SceneGetValueListSelection
(
	uint8 sceneId,
	ZWValueID^ valueId,
	String^ *o_value
)
{
	string value;
	if (Manager::Get()->SceneGetValueListSelection(sceneId, valueId->CreateUnmanagedValueID(), &value))
	{
		*o_value = ConvertString(value);
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// <ZWManager::SceneGetValueListSelection>
// Retrieves a scene's value in a list (as a integer)
//-----------------------------------------------------------------------------
bool ZWManager::SceneGetValueListSelection
(
	uint8 sceneId,
	ZWValueID^ valueId,
	int *o_value
)
{
	int32 value;
	if (Manager::Get()->SceneGetValueListSelection(sceneId, valueId->CreateUnmanagedValueID(), &value))
	{
		*o_value = value;
		return true;
	}
	return false;
}



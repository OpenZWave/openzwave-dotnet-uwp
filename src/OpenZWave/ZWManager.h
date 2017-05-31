//-----------------------------------------------------------------------------
//
//      ZWManager.h
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
#include "ZWNotification.h"

using namespace OpenZWave;
#if __cplusplus_cli
using namespace System;
using namespace Runtime::InteropServices;
#else
using namespace Platform;
#endif


namespace OpenZWave
{
	ref class ZWManager;

	public delegate void NotificationReceivedEventHandler(ZWManager^ sender, NotificationReceivedEventArgs^ e);

#if __cplusplus_cli

	[UnmanagedFunctionPointer(CallingConvention::Cdecl)]
	private delegate void OnNotificationFromUnmanagedDelegate(Notification* _notification, void* _context);

#else
	private delegate void OnNotificationFromUnmanagedDelegate(void *_notification, void* _context);
#endif


	/// <summary>The main public interface to OpenZWave.</summary>
	/// <remarks><para>
	/// A singleton class providing the main public interface to OpenZWave.
	/// The Manager class exposes all the functionality required to add
	/// Z-Wave support to an application.  It handles the sending and receiving
	/// of Z-Wave messages as well as the configuration of a Z-Wave network
	/// and its devices, freeing the library user from the burden of learning
	/// the low-level details of the Z-Wave protocol.
	/// </para><para>
	/// All Z-Wave functionality is accessed via the Manager class.  While this
	/// does not make for the most efficient code structure, it does enable
	/// the library to handle potentially complex and hard-to-debug issues
	/// such as multi-threading and object lifespans behind the scenes.
	/// Application development is therefore simplified and less prone to bugs.
	/// </para><para>
	/// There can be only one instance of the Manager class, and all applications
	/// will start by calling Manager::Create static method to create that instance.
	/// From then on, a call to the Manager::Get static method will return the
	/// pointer to the Manager object.  On application exit, Manager::Destroy
	/// should be called to allow OpenZWave to clean up and delete any other
	/// objects it has created.
	/// </para><para>
	/// Once the Manager has been created, a call should be made to Manager::AddWatcher
	/// to install a notification callback handler.  This handler will receive
	/// notifications of Z-Wave network changes and updates to device values, and is
	/// an essential element of OpenZWave.
	/// </para><para>
	/// Next, a call should be made to Manager::AddDriver for each Z-Wave controller
	/// attached to the PC.  Each Driver will handle the sending and receiving of
	/// messages for all the devices in its controller's Z-Wave network.  The Driver
	/// will read any previously saved configuration and then query the Z-Wave controller
	/// for any missing information.  Once that process is complete, a DriverReady
	/// notification callback will be sent containing the Home ID of the controller,
	/// which is required by most of the other Manager class methods.
	/// </para><para>
	/// [After the DriverReady notification is sent, the Driver will poll each node on
	/// the network to update information about each node.  After all "awake" nodes
	/// have been polled, an "AllAwakeNodesQueried" notification is sent.  This is when
	/// a client application can expect all of the node information (both static
	/// information, like the physical device's capabilities, session information
	/// (like [associations and/or names] and dynamic information (like temperature or
	/// on/off state) to be available.  Finally, after all nodes (whether listening or
	/// sleeping) have been polled, an "AllNodesQueried" notification is sent.]
	/// </para></remarks>
	public ref class ZWManager sealed
	{
	private:

		bool m_isInitialized = false;
#if __cplusplus_cli
		static ZWManager^ s_instance = nullptr;
#endif
		ZWManager() { }

	public:
		/// <summary>Gets a reference to the single ZWManager instance</summary>
		/// <remarks>Before use, 'Initialize' must be called.</remarks>
		/// <returns>The ZWManager singleton instance</returns>
		static property ZWManager^ Instance { ZWManager^ get(); }

		/// <summary>Event fired when a notification is received from the controller or a node</summary>
		event NotificationReceivedEventHandler^ NotificationReceived;

		/// <summary>Creates the Manager singleton object.</summary>
		/// <remarks>
		/// The Manager provides the public interface to OpenZWave, exposing all the functionality required to add Z-Wave support to an application.
		/// There can be only one Manager in an OpenZWave application.  Once the Manager has been created, call AddWatcher to install a notification
		/// callback handler, and then call the AddDriver method for each attached PC Z-Wave controller in turn.</remarks>
		/// <seealso cref="Destroy" />
		/// <seealso cref="NotificationReceived" />
		/// <seealso cref="AddDriver" />
		void Initialize();

		/// <summary>Deletes the Manager and cleans up any associated objects.</summary>
		/// <seealso cref="Initialize" />
		void Destroy() { Manager::Get()->Destroy(); m_isInitialized = false; }

		/// <summary>Get the Version Number of OZW as a string</summary>
		/// <returns>A String representing the version number as MAJOR.MINOR.REVISION</returns>
		String^ GetVersionAsString() { return ConvertString(Manager::Get()->getVersionAsString()); }

		/// <summary>Sets the library logging state.</summary>
		/// <param name="bState">True to enable logging; false to disable logging.</param>
		/// <seealso cref="GetLoggingState" />
		void SetLoggingState(bool bState) { Log::SetLoggingState(bState); }

		/// <summary>Gets the current library logging state.</summary>
		/// <returns>True if logging is enabled; false otherwise</returns>
		/// <seealso cref="SetLoggingState" />
		bool GetLoggingState() { return Log::GetLoggingState(); }

		/// <summary>Sets the current library log file name to a new name</summary>
		/// <param name="filename">Name of the log file</param>
		void SetLogFileName(String^ _filename) { Log::SetLogFileName(ConvertString(_filename)); }

		/// <summary>Sends current driver statistics to the log file</summary>
		/// <param name="homeId">The Home ID of the Z-Wave controller.</param>
		void LogDriverStatistics(uint32 homeId) { Manager::Get()->LogDriverStatistics(homeId); }

		//-----------------------------------------------------------------------------
		// Configuration
		//-----------------------------------------------------------------------------
		/** \name Configuration
		*  For saving the Z-Wave network configuration so that the entire network does not need to be
		*  polled every time the application starts.
		*/
		/*@{*/
	public:
		/// <summary>Saves the configuration of a PC Controller's Z-Wave network to the application's user data folder.</summary>
		/// <remarks>
		/// This method does not normally need to be called, since OpenZWave will save the state automatically
		/// during the shutdown process.  It is provided here only as an aid to development.
		/// The configuration of each PC Controller's Z-Wave network is stored in a separate file.  The filename
		/// consists of the 8 digit hexadecimal version of the controller's Home ID, prefixed with the string 'zwcfg_'.
		/// This convention allows OpenZWave to find the correct configuration file for a controller, even if it is
		/// attached to a different serial port.</remarks>
		/// <param name="homeId">The Home ID of the Z-Wave controller to save.</param>
		void WriteConfig(uint32 homeId) { Manager::Get()->WriteConfig(homeId); }
		/*@}*/

		//-----------------------------------------------------------------------------
		//	Drivers
		//-----------------------------------------------------------------------------
		/** \name Drivers
		*  Methods for adding and removing drivers and obtaining basic controller information.
		*/
		/*@{*/
	public:
		/// <summary>Creates a new driver for a Z-Wave controller.</summary>
		/// <remarks>
		/// This method creates a Driver object for handling communications with a single Z-Wave controller.  In the background, the
		/// driver first tries to read configuration data saved during a previous run.  It then queries the controller directly for any
		/// missing information, and a refresh of the list of nodes that it controls.  Once this information
		/// has been received, a DriverReady notification callback is sent, containing the Home ID of the controller.  This Home ID is
		/// required by most of the OpenZWave Manager class methods.</remarks>
		/// <param name="serialPortName">The string used to open the serial port, for example "\\.\COM3".</param>
		/// <seealso cref="Create" />
		/// <seealso cref="Get" />
		/// <seealso cref="RemoveDriver" />
		bool AddDriver(String^ serialPortName) { return Manager::Get()->AddDriver(ConvertString(serialPortName)); }

		/// <summary>Creates a new driver for a Z-Wave controller.</summary>
		/// <remarks>
		/// This method creates a Driver object for handling communications with a single Z-Wave controller.  In the background, the
		/// driver first tries to read configuration data saved during a previous run.  It then queries the controller directly for any
		/// missing information, and a refresh of the list of nodes that it controls.  Once this information
		/// has been received, a DriverReady notification callback is sent, containing the Home ID of the controller.  This Home ID is
		/// required by most of the OpenZWave Manager class methods.</remarks>
		/// <param name="serialPortName">The string used to open the serial port, for example "\\.\COM3".</param>
		/// <param name="interfaceType">Specifies whether this is a serial or HID interface (default is serial).</param>
		/// <returns>True if a new driver was created, false if a driver for the controller already exists.</returns>
		/// <seealso cref="Create" />
		/// <seealso cref="Get" />
		/// <seealso cref="RemoveDriver" />
		bool AddDriver(String^ serialPortName, ZWControllerInterface interfaceType) { return Manager::Get()->AddDriver(ConvertString(serialPortName), (Driver::ControllerInterface) interfaceType); }

		/// <summary>Removes the driver for a Z-Wave controller, and closes the serial port.</summary>
		/// <remarks>
		/// Drivers do not need to be explicitly removed before calling Destroy - this is handled automatically.
		/// </remarks>
		/// <param name="serialPortName">The same string as was passed in the original call to AddDriver.</param>
		/// <returns>True if the driver was removed, false if it could not be found.</returns>
		/// <seealso cref="Destroy" />
		/// <seealso cref="AddDriver(serialPortName)" />
		bool RemoveDriver(String^ serialPortName) { return Manager::Get()->RemoveDriver(ConvertString(serialPortName)); }

		/// <summary>Get the node ID of the Z-Wave controller.</summary>
		/// <param name="homeId">The Home ID of the Z-Wave controller.</param>
		/// <returns>The node ID of the Z-Wave controller.</returns>
		uint8 GetControllerNodeId(uint32 homeId) { return Manager::Get()->GetControllerNodeId(homeId); }

		/// <summary>Get the node ID of the Static Update Controller.</summary>
		/// <param name="homeId">The Home ID of the Z-Wave controller.</param>
		/// <returns>The node ID of the Z-Wave controller.</returns>
		uint8 GetSucNodeId(uint32 homeId) { return Manager::Get()->GetSUCNodeId(homeId); }

		/// <summary>Query if the controller is a primary controller.</summary>
		/// <remarks>
		/// <para>The primary controller is the main device used to configure and control a Z-Wave network.
		/// There can only be one primary controller - all other controllers are secondary controllers.</para>
		/// <para>The only difference between a primary and secondary controller is that the primary is the
		/// only one that can be used to add or remove other devices.  For this reason, it is usually
		/// better for the primary controller to be portable, since most devices must be added when
		/// installed in their final location.</para>
		/// <para>Calls to BeginControllerCommand will fail if the controller is not the primary.</para>
		/// </remarks>
		/// <param name="homeId">The Home ID of the Z-Wave controller.</param>
		/// <returns>true if it is a primary controller, false if not.</returns>
		bool IsPrimaryController(uint32 homeId) { return Manager::Get()->IsPrimaryController(homeId); }

		/// <summary>Query if the controller is a static update controller.</summary>
		/// <remarks>A Static Update Controller (SUC) is a controller that must never be moved in normal operation
		/// and which can be used by other nodes to receive information about network changes.</remarks>
		/// <param name="homeId">The Home ID of the Z-Wave controller.</param>
		/// <returns>true if it is a static update controller, false if not.</returns>
		bool IsStaticUpdateController(uint32 homeId) { return Manager::Get()->IsStaticUpdateController(homeId); }

		/// <summary>Query if the controller is using the bridge controller library.</summary>
		/// <remarks>A bridge controller is able to create virtual nodes that can be associated
		/// with other controllers to enable events to be passed on.</remarks>
		/// <param name="homeId">The Home ID of the Z-Wave controller.</param>
		/// <returns>true if it is a bridge controller, false if not.</returns>
		bool IsBridgeController(uint32 homeId) { return Manager::Get()->IsBridgeController(homeId); }

		/// <summary>Get the version of the Z-Wave API library used by a controller.</summary>
		/// <param name="homeId">The Home ID of the Z-Wave controller.</param>
		/// <returns>a string containing the library version. For example, "Z-Wave 2.48".</returns>
		String^ GetLibraryVersion(uint32 homeId) { return ConvertString(Manager::Get()->GetLibraryVersion(homeId)); }

		/// <summary>Get a string containing the Z-Wave API library type used by a controller.</summary>
		/// <remarks><para>
		/// The possible library types are:<br/>
		/// - Static Controller<br/>
		/// - Controller<br/>
		/// - Enhanced Slave<br/>
		/// - Slave<br/>
		/// - Installer<br/>
		/// - Routing Slave<br/>
		/// - Bridge Controller<br/>
		/// - Device Under Test
		/// </para><para>
		/// The controller should never return a slave library type.
		/// For a more efficient test of whether a controller is a Bridge Controller, use
		/// the IsBridgeController method.</para></remarks>
		/// <param name="homeId">The Home ID of the Z-Wave controller.</param>
		/// <returns>A string containing the library type.</returns>
		/// <seealso cref="GetLibraryVersion" />
		/// <seealso cref="IsBridgeController" />
		String^ GetLibraryTypeName(uint32 homeId) { return ConvertString(Manager::Get()->GetLibraryTypeName(homeId)); }

		/// <summary>Get count of messages in the outgoing send queue.</summary>
		/// <param name="homeId">The Home ID of the Z-Wave controller.</param>
		/// <returns>a integer message count</returns>
		int32 GetSendQueueCount(uint32 homeId) { return Manager::Get()->GetSendQueueCount(homeId); }

		/// <summary>Obtain controller interface type</summary>
		/// <param name="homeId">The Home ID of the Z-Wave controller.</param>
		ZWControllerInterface GetControllerInterfaceType(uint32 homeId) { return (ZWControllerInterface)Manager::Get()->GetControllerInterfaceType(homeId); }

		/// <summary>Obtain controller interface path</summary>
		/// <param name="homeId">The Home ID of the Z-Wave controller.</param>
		String^ GetControllerPath(uint32 homeId) { return ConvertString(Manager::Get()->GetControllerPath(homeId)); }

		/*@}*/

		//-----------------------------------------------------------------------------
		//	Polling Z-Wave devices
		//-----------------------------------------------------------------------------
		/** \name Polling Z-Wave devices
		*  Methods for controlling the polling of Z-Wave devices.  Modern devices will not
		*  require polling.  Some old devices need to be polled as the only way to detect
		*  status changes.
		*/
		/*@{*/
	public:
		/// <summary>Get the time period between polls of a node's state.</summary>
		int32 GetPollInterval() { return Manager::Get()->GetPollInterval(); }

	    /// <summary>Set the time period between polls of a node's state.</summary>
	    /// <remarks><para>
	    /// Due to patent concerns, some devices do not report state changes automatically to the controller.
	    /// These devices need to have their state polled at regular intervals.  The length of the interval
	    /// is the same for all devices.  To even out the Z-Wave network traffic generated by polling, OpenZWave
	    /// divides the polling interval by the number of devices that have polling enabled, and polls each
	    /// in turn.  It is recommended that if possible, the interval should not be set shorter than the
	    /// number of polled devices in seconds (so that the network does not have to cope with more than one
	    /// poll per second).
	    /// </para><para>
	    /// Note that the polling interval cannot be set on a per-node basis.  Every node that is polled is
	    /// polled at the specified interval.</para></remarks>
	    /// <param name="milliseconds">The length of the polling interval in milliseconds.</param>
		void SetPollInterval(int32 milliseconds, bool bIntervalBetweenPolls) { Manager::Get()->SetPollInterval(milliseconds, bIntervalBetweenPolls); }

		/// <summary>Enable the polling of a device's state.</summary>
		/// <param name="valueId">The ID of the value to start polling.</param>
		/// <returns>True if polling was enabled.</returns>
		bool EnablePoll(ZWValueId^ valueId) { return Manager::Get()->EnablePoll(valueId->CreateUnmanagedValueID()); }

		/// <summary>Enable the polling of a device's state.</summary>		*
		/// <param name="valueId">The ID of the value to start polling.</param>
		/// <param name="intensity">number of polling for one polling interval.</param>
		/// <returns>True if polling was enabled.</returns>
		bool EnablePoll(ZWValueId^ valueId, uint8 intensity) { return Manager::Get()->EnablePoll(valueId->CreateUnmanagedValueID(), intensity); }

		/// <summary>Disable the polling of a device's state.</summary>
		/// <param name="valueId">The ID of the value to stop polling.</param>
		/// <returns>True if polling was disabled.</returns>
		bool DisablePoll(ZWValueId^ valueId) { return Manager::Get()->DisablePoll(valueId->CreateUnmanagedValueID()); }

		/// <summary>Determine the polling of a device's state.</summary>
		/// <param name="valueId">The ID of the value to check polling.</param>
		/// <returns>True if polling is active.</returns>
		bool IsPolled(ZWValueId^ valueId) { return Manager::Get()->isPolled(valueId->CreateUnmanagedValueID()); }

		/// <summary>Set the frequency of polling (0=none, 1=every time through the list, 2-every other time, etc)</summary>
		/// <param name="valueId">The ID of the value whose intensity should be set</param>
		/// <param name="intensity">The intensity to set</param>
		void SetPollIntensity(ZWValueId^ valueId, uint8 intensity) { Manager::Get()->SetPollIntensity(valueId->CreateUnmanagedValueID(), intensity); }

		/// <summary>Get the polling intensity of a device's state.</summary>
		/// <param name="valueId">The ID of the value to check polling.</param>
		/// <returns>Intensity, number of polling for one polling interval.</returns>
		// \throws OZWException with Type OZWException::OZWEXCEPTION_INVALID_VALUEID if the ValueID is invalid
		// \throws OZWException with Type OZWException::OZWEXCEPTION_INVALID_HOMEID if the Driver cannot be found
		uint8 GetPollIntensity(ZWValueId^ valueId) { return Manager::Get()->GetPollIntensity(valueId->CreateUnmanagedValueID()); }

		/*@}*/

		//-----------------------------------------------------------------------------
		//	Node information
		//-----------------------------------------------------------------------------
		/** \name Node information
		*  Methods for accessing information on indivdual nodes.
		*/
		/*@{*/
	public:
		/// <summary>Trigger the fetching of fixed data about a node.</summary>
		/// <remarks>
		/// Causes the node's data to be obtained from the Z-Wave network in the same way as if it had just been added.
		/// This method would normally be called automatically by OpenZWave, but if you know that a node has been
		/// changed, calling this method will force a refresh of the data held by the library.  This can be especially
		/// useful for devices that were asleep when the application was first run.</remarks>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to query.</param>
		/// <returns>True if the request was sent successfully.</returns>
		bool RefreshNodeInfo(uint32 homeId, uint8 nodeId) { return Manager::Get()->RefreshNodeInfo(homeId, nodeId); }

		/// <summary>Trigger the fetching of session and dynamic value data for a node.</summary>
		/// <remarks>Causes the node's values to be requested from the Z-Wave network.</remarks>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to query.</param>
		/// <returns>True if the request was sent successfully.</returns>
		void RequestNodeState(uint32 homeId, uint8 nodeId) { Manager::Get()->RequestNodeState(homeId, nodeId); }

		/// <summary>Trigger the fetching of just the dynamic value data for a node.</summary>
		/// <remarks>Causes the node's values to be requested from the Z-Wave network. This is the
		/// same as the query state starting from the dynamic state.</remarks>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to query.</param>
		/// <returns>True if the request was sent successfully.</returns>
		bool RequestNodeDynamic(uint32 homeId, uint8 nodeId) { return Manager::Get()->RequestNodeDynamic(homeId, nodeId); }

		/// <summary>Get whether the node is a listening device that does not go to sleep.</summary>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to query.</param>
		/// <returns>True if it is a listening node.</returns>
		bool IsNodeListeningDevice(uint32 homeId, uint8 nodeId) { return Manager::Get()->IsNodeListeningDevice(homeId, nodeId); }

		/// <summary>Get whether the node is a frequent listening device that goes to sleep but
		/// can be woken up by a beam. Useful to determine node and controller consistency.</summary>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to query.</param>
		/// <returns>True if it is a frequent listening node.</returns>
		bool IsNodeFrequentListeningDevice(uint32 homeId, uint8 nodeId) { return Manager::Get()->IsNodeFrequentListeningDevice(homeId, nodeId); }

		/// <summary>Get whether the node is a beam capable device.</summary>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to query.</param>
		/// <returns>True if it is a frequent listening node.</returns>
		bool IsNodeBeamingDevice(uint32 homeId, uint8 nodeId) { return Manager::Get()->IsNodeBeamingDevice(homeId, nodeId); }

		/// <summary>Get whether the node is a routing device that passes messages to other nodes.</summary>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to query.</param>
		/// <returns>True if the node is a routing device</returns>
		bool IsNodeRoutingDevice(uint32 homeId, uint8 nodeId) { return Manager::Get()->IsNodeRoutingDevice(homeId, nodeId); }

		/// <summary>Get the security attribute for a node. True if node supports security features.</summary>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to query.</param>
		/// <returns>true if security features implemented.</returns>
		bool IsNodeSecurityDevice(uint32 homeId, uint8 nodeId) { return Manager::Get()->IsNodeSecurityDevice(homeId, nodeId); }

		/// <summary>Is this a ZWave+ Supported Node?</summary>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to query.</param>
		/// <returns>If this node is a Z-Wave Plus Node</returns>
		bool IsNodeZWavePlus(uint32 homeId, uint8 nodeId) { return Manager::Get()->IsNodeZWavePlus(homeId, nodeId); }

		/// <summary>Get the maximum baud rate of a node's communications</summary>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to query.</param>
		/// <returns>the baud rate in bits per second.</returns>
		uint32 GetNodeMaxBaudRate(uint32 homeId, uint8 nodeId) { return Manager::Get()->GetNodeMaxBaudRate(homeId, nodeId); }

		/// <summary>Get the version number of a node</summary>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to query.</param>
		/// <returns>the node's version number</returns>
		uint8 GetNodeVersion(uint32 homeId, uint8 nodeId) { return Manager::Get()->GetNodeVersion(homeId, nodeId); }

		/// <summary>Get the security byte for a node.  Bit meanings are still to be determined.</summary>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to query.</param>
		/// <returns>the node's security byte</returns>
		uint8 GetNodeSecurity(uint32 homeId, uint8 nodeId) { return Manager::Get()->GetNodeSecurity(homeId, nodeId); }

		/// <summary>Get a node's "basic" type.</summary>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to query.</param>
		/// <returns>The basic type.</returns>
		uint8 GetNodeBasic(uint32 homeId, uint8 nodeId) { return Manager::Get()->GetNodeBasic(homeId, nodeId); }

		/// <summary>Get a node's "generic" type.</summary>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to query.</param>
		/// <returns>The generic type.</returns>
		uint8 GetNodeGeneric(uint32 homeId, uint8 nodeId) { return Manager::Get()->GetNodeGeneric(homeId, nodeId); }

		/// <summary>Get a node's "specific" type.</summary>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to query.</param>
		/// <returns>The specific type.</returns>
		uint8 GetNodeSpecific(uint32 homeId, uint8 nodeId) { return Manager::Get()->GetNodeSpecific(homeId, nodeId); }

		/// <summary>Get a human-readable label describing the node.</summary>
		/// <remarks>
		/// The label is taken from the Z-Wave specific, generic or basic type, depending on which of those values are specified by the node.
		/// </remarks>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to query.</param>
		/// <returns>A string containing the label text.</returns>
		String^ GetNodeType(uint32 homeId, uint8 nodeId) { return ConvertString(Manager::Get()->GetNodeType(homeId, nodeId)); }

		/// <summary>Get the bitmap of this node's neighbors</summary>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to query.</param>
		/// <param name="o_associations">An array of 29 uint8s to hold the neighbor bitmap</param>
		//uint32 GetNodeNeighbors(uint32 const homeId, uint8 const nodeId, [Out] cli::array<Byte>^ %o_associations);
		uint32 GetNodeNeighbors(
#if __cplusplus_cli
			uint32 homeId, uint8 nodeId, [Out] cli::array<Byte>^ %o_associations);
#else
			uint32 homeId, uint8 nodeId, Platform::Array<byte>^ *o_associations);
#endif

		/// <summary>Get the manufacturer name of a device.</summary>
		/// <remarks>
		/// The manufacturer name would normally be handled by the Manufacturer Specific commmand class,
		/// taking the manufacturer ID reported by the device and using it to look up the name from the
		/// manufacturer_specific.xml file in the OpenZWave config folder.
		/// However, there are some devices that do not support the command class, so to enable the user
		/// to manually set the name, it is stored with the node data and accessed via this method rather
		/// than being reported via a command class Value object.
		/// </remarks>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to query.</param>
		/// <returns>A string containing the node's manufacturer name.</returns>
		/// <seealso cref="SetNodeManufacturerName" />
		/// <seealso cref="GetNodeProductName" />
		/// <seealso cref="SetNodeProductName" />
		String^ GetNodeManufacturerName(uint32 homeId, uint8 nodeId) { return ConvertString(Manager::Get()->GetNodeManufacturerName(homeId, nodeId)); }

		/// <summary>Get the product name of a device.</summary>
		/// <remarks>
		/// The product name would normally be handled by the Manufacturer Specific commmand class,
		/// taking the product Type and ID reported by the device and using it to look up the name from the
		/// manufacturer_specific.xml file in the OpenZWave config folder.
		/// However, there are some devices that do not support the command class, so to enable the user
		/// to manually set the name, it is stored with the node data and accessed via this method rather
		/// than being reported via a command class Value object.
		/// </remarks>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to query.</param>
		/// <returns>A string containing the node's product name.</returns>
		/// <seealso cref="SetNodeProductName" />
		/// <seealso cref="GetNodeManufacturerName" />
		/// <seealso cref="SetNodeManufacturerName" />
		String^ GetNodeProductName(uint32 homeId, uint8 nodeId) { return ConvertString(Manager::Get()->GetNodeProductName(homeId, nodeId)); }

		/// <summary>Get the name of a node.</summary>
		/// <remarks>
		/// The node name is a user-editable label for the node that would normally be handled by the
		/// Node Naming commmand class, but many devices do not support it.  So that a node can always
		/// be named, OpenZWave stores it with the node data, and provides access through this method
		/// and SetNodeName, rather than reporting it via a command class Value object.
		/// The maximum length of a node name is 16 characters.</remarks>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to query.</param>
		/// <returns>A string containing the node's name.</returns>
		/// <seealso cref="SetNodeName" />
		/// <seealso cref="GetNodeLocation" />
		/// <seealso cref="SetNodeLocation" />
		String^ GetNodeName(uint32 homeId, uint8 nodeId) { return ConvertString(Manager::Get()->GetNodeName(homeId, nodeId)); }

		/// <summary>Get the location of a node.</summary>
		/// <remarks>
		/// The node location is a user-editable string that would normally be handled by the Node Naming
		/// commmand class, but many devices do not support it.  So that a node can always report its
		/// location, OpenZWave stores it with the node data, and provides access through this method
		/// and SetNodeLocation, rather than reporting it via a command class Value object.</remarks>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to query.</param>
		/// <returns>A string containing the node's location.</returns>
		/// <seealso cref="SetNodeLocation" />
		/// <seealso cref="GetNodeName" />
		/// <seealso cref="SetNodeName" />
		String^ GetNodeLocation(uint32 homeId, uint8 nodeId) { return ConvertString(Manager::Get()->GetNodeLocation(homeId, nodeId)); }

		/// <summary>Get the manufacturer ID of a device.</summary>
		/// <remarks>
		/// The manufacturer ID is a four digit hex code and would normally be handled by the Manufacturer
		/// Specific commmand class, but not all devices support it.  Although the value reported by this
		/// method will be an empty string if the command class is not supported and cannot be set by the
		/// user, the manufacturer ID is still stored with the node data (rather than being reported via a
		/// command class Value object) to retain a consistent approach with the other manufacturer specific data.</remarks>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to query.</param>
		/// <returns>A string containing the node's manufacturer ID, or an empty string if the manufactuer
		/// specific command class is not supported by the device.</returns>
		/// <seealso cref="GetNodeProductType" />
		/// <seealso cref="GetNodeProductId" />
		/// <seealso cref="GetNodeManufacturerName" />
		/// <seealso cref="GetNodeProductName" />
		String^ GetNodeManufacturerId(uint32 homeId, uint8 nodeId) { return ConvertString(Manager::Get()->GetNodeManufacturerId(homeId, nodeId)); }

		/// <summary>Get the product type of a device.</summary>
		/// <remarks>
		/// The product type is a four digit hex code and would normally be handled by the Manufacturer Specific
		/// commmand class, but not all devices support it.  Although the value reported by this method will
		/// be an empty string if the command class is not supported and cannot be set by the user, the product
		/// type is still stored with the node data (rather than being reported via a command class Value object)
		/// to retain a consistent approach with the other manufacturer specific data.</remarks>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to query.</param>
		/// <returns>A string containing the node's product type, or an empty string if the manufactuer
		/// specific command class is not supported by the device.</returns>
		/// <seealso cref="GetNodeManufacturerId" />
		/// <seealso cref="GetNodeProductId" />
		/// <seealso cref="GetNodeManufacturerName" />
		/// <seealso cref="GetNodeProductName" />
		String^ GetNodeProductType(uint32 homeId, uint8 nodeId) { return ConvertString(Manager::Get()->GetNodeProductType(homeId, nodeId)); }

		/// <summary>Get the product ID of a device.</summary>
		/// <remarks>
		/// The product ID is a four digit hex code and would normally be handled by the Manufacturer Specific
		/// commmand class, but not all devices support it.  Although the value reported by this method will
		/// be an empty string if the command class is not supported and cannot be set by the user, the product
		/// ID is still stored with the node data (rather than being reported via a command class Value object)
		/// to retain a consistent approach with the other manufacturer specific data.</remarks>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to query.</param>
		/// <returns>A string containing the node's product ID, or an empty string if the manufactuer
		/// specific command class is not supported by the device.</returns>
		/// <seealso cref="GetNodeManufacturerId" />
		/// <seealso cref="GetNodeProductType" />
		/// <seealso cref="GetNodeManufacturerName" />
		/// <seealso cref="GetNodeProductName" />
		String^ GetNodeProductId(uint32 homeId, uint8 nodeId) { return ConvertString(Manager::Get()->GetNodeProductId(homeId, nodeId)); }

		/// <summary>Set the manufacturer name of a device.</summary>
		/// <remarks>
		/// The manufacturer name would normally be handled by the Manufacturer Specific commmand class,
		/// taking the manufacturer ID reported by the device and using it to look up the name from the
		/// manufacturer_specific.xml file in the OpenZWave config folder.
		/// However, there are some devices that do not support the command class, so to enable the user
		/// to manually set the name, it is stored with the node data and accessed via this method rather
		/// than being reported via a command class Value object.</remarks>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to query.</param>
		/// <param name="manufacturerName">A string containing the node's manufacturer name.</param>
		/// <seealso cref="GetNodeManufacturerName" /> 
		/// <seealso cref="GetNodeProductName" />
		/// <seealso cref="SetNodeProductName" />
		void SetNodeManufacturerName(uint32 homeId, uint8 nodeId, String^ manufacturerName) { Manager::Get()->SetNodeManufacturerName(homeId, nodeId, ConvertString(manufacturerName)); }

		/// <summary>Set the product name of a device.</summary>
		/// <remarks>
		/// The product name would normally be handled by the Manufacturer Specific commmand class,
		/// taking the product Type and ID reported by the device and using it to look up the name from the
		/// manufacturer_specific.xml file in the OpenZWave config folder.
		/// However, there are some devices that do not support the command class, so to enable the user
		/// to manually set the name, it is stored with the node data and accessed via this method rather
		/// than being reported via a command class Value object.</remarks>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to query.</param>
		/// <param name="productName">A string containing the node's product name.</param>
		/// <seealso cref="GetNodeProductName" /> 
		/// <seealso cref="GetNodeManufacturerName" /> 
		/// <seealso cref="SetNodeManufacturerName" />
		void SetNodeProductName(uint32 homeId, uint8 nodeId, String^ productName) { Manager::Get()->SetNodeProductName(homeId, nodeId, ConvertString(productName)); }

		/// <summary>Set the name of a node.</summary>
		/// <remarks>The node name is a user-editable label for the node that would normally be handled by the
		/// Node Naming commmand class, but many devices do not support it.  So that a node can always
		/// be named, OpenZWave stores it with the node data, and provides access through this method
		/// and GetNodeName, rather than reporting it via a command class Value object.
		/// If the device does support the Node Naming command class, the new name will be sent to the node.
		/// The maximum length of a node name is 16 characters.</remarks>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to query.</param>
		/// <param name="nodeName">A string containing the node's name.</param>
		/// <seealso cref="GetNodeName" />
		/// <seealso cref="GetNodeLocation" />
		/// <seealso cref="SetNodeLocation" />
		void SetNodeName(uint32 homeId, uint8 nodeId, String^ nodeName) { Manager::Get()->SetNodeName(homeId, nodeId, ConvertString(nodeName)); }

		/// <summary>Set the location of a node.</summary>
		/// <remarks>The node location is a user-editable string that would normally be handled by the Node Naming
		/// commmand class, but many devices do not support it.  So that a node can always report its
		/// location, OpenZWave stores it with the node data, and provides access through this method
		/// and GetNodeLocation, rather than reporting it via a command class Value object.
		/// If the device does support the Node Naming command class, the new location will be sent to the node.</remarks>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to query.</param>
		/// <param name="location">A string containing the node's location.</param>
		void SetNodeLocation(uint32 homeId, uint8 nodeId, String^ location) { Manager::Get()->SetNodeLocation(homeId, nodeId, ConvertString(location)); }

		/// <summary>Get whether the node information has been received</summary>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to query.</param>
		/// <returns>True if the node information has been received yet</returns>
		bool IsNodeInfoReceived(uint32 homeId, uint8 nodeId) { return Manager::Get()->IsNodeInfoReceived(homeId, nodeId); }

		/// <summary>Get whether the node has the defined class available or not</summary>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to query.</param>
		/// <param name="commandClassId">Id of the class to test for</param>
		/// <param name="className">Name of the class</param>
		/// <param name="classVersion">Version of the class</param>
		/// <returns>True if the node does have the class instantiated, will return name and version</returns>
		bool GetNodeClassInformation(uint32 homeId, uint8 nodeId, uint8 commandClassId,
#if __cplusplus_cli
			[Out] String^ %className, [Out] System::Byte %classVersion
#else
			String^ *className, byte *classVersion
#endif
		);

		/// <summary>Get whether the node is awake or asleep</summary>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to query.</param>
		/// <returns>True if the node is awake</returns>
		bool IsNodeAwake(uint32 homeId, uint8 nodeId) { return Manager::Get()->IsNodeAwake(homeId, nodeId); }

		/// <summary>Get whether the node is working or has failed</summary>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to query.</param>
		/// <returns>True if the node has failed and is no longer part of the network</returns>
		bool IsNodeFailed(uint32 homeId, uint8 nodeId) { return Manager::Get()->IsNodeFailed(homeId, nodeId); }

		/// <summary>Get whether the node's query stage as a string</summary>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to query.</param>
		/// <returns>name of current query stage as a string.</returns>
		String^ GetNodeQueryStage(uint32 homeId, uint8 nodeId) { return ConvertString(Manager::Get()->GetNodeQueryStage(homeId, nodeId)); }

		/*@}*/

		//-----------------------------------------------------------------------------
		// Values
		//-----------------------------------------------------------------------------
		/** \name Values
		*  Methods for accessing device values.  All the methods require a ValueID, which will have been provided
		*  in the ValueAdded Notification callback when the the value was first discovered by OpenZWave.
		*/
		/*@{*/
	public:

		/// <summary>Gets the user-friendly label for the value.</summary>
		/// <param name="id">The unique identifier of the value.</param>
		/// <returns>The value label.</returns>
		/// <seealso cref="ZWValueId" />
		String^ GetValueLabel(ZWValueId^ id) { return ConvertString(Manager::Get()->GetValueLabel(id->CreateUnmanagedValueID())); }

		/// <summary>Sets the user-friendly label for the value.</summary>
		/// <param name="id">The unique identifier of the value.</param>
		/// <param name="value">The new value of the label.</param>
		/// <seealso cref="ZWValueId" />
		// \throws OZWException with Type OZWException::OZWEXCEPTION_INVALID_VALUEID if the ValueID is invalid
		// \throws OZWException with Type OZWException::OZWEXCEPTION_INVALID_HOMEID if the Driver cannot be found
		void SetValueLabel(ZWValueId^ id, String^ value) { Manager::Get()->SetValueLabel(id->CreateUnmanagedValueID(), ConvertString(value)); }

		/// <summary>Gets the units that the value is measured in.</summary>
		/// <param name="id">The unique identifier of the value.</param>
		/// <returns>The value units.</returns>
		/// <seealso cref="ZWValueId" />
		String^ GetValueUnits(ZWValueId^ id) { return ConvertString(Manager::Get()->GetValueUnits(id->CreateUnmanagedValueID())); }

		/// <summary>Gets a help string describing the value's purpose and usage.</summary>
		/// <param name="id">The unique identifier of the value.</param>
		/// <returns>The value help text.</returns>
		/// <seealso cref="ZWValueId" />
		String^ GetValueHelp(ZWValueId^ id) { return ConvertString(Manager::Get()->GetValueHelp(id->CreateUnmanagedValueID())); }

		/// <summary>Test whether the value is read-only.</summary>
		/// <param name="id">The unique identifier of the value.</param>
		/// <returns>true if the value cannot be changed by the user.</returns>
		/// <seealso cref="ZWValueID" />
		bool IsValueReadOnly(ZWValueId^ id) { return Manager::Get()->IsValueReadOnly(id->CreateUnmanagedValueID()); }

		/// <summary>Test whether the value has been set.</summary>
		/// <param name="id">The unique identifier of the value.</param>
		/// <returns>true if the value has actually been set by a status message from the device, rather than simply being the default.</returns>
		/// <seealso cref="ValueID" />
		bool IsValueSet(ZWValueId^ id) { return Manager::Get()->IsValueSet(id->CreateUnmanagedValueID()); }

		/// <summary>Test whether the value is currently being polled.</summary>
		/// <param name="id">The unique identifier of the value.</param>
		/// <returns>true if the value is being polled, false otherwise.</returns>
		/// <seealso cref="ValueID" />
		bool IsValuePolled(ZWValueId^ id) { return Manager::Get()->IsValuePolled(id->CreateUnmanagedValueID()); }

		/// <summary>Gets a value as a bool.</summary>
		/// <param name="id">The unique identifier of the value.</param>
		/// <param name="o_value">a Boolean that will be filled with the value.</param>
		/// <returns>true if the value was obtained.  Returns false if the value is not a ZWValueID::ValueType_Bool. The type can be tested with a call to ZWValueID::GetType.</returns>
		// \see ValueID::GetType, GetValueAsByte, GetValueAsDecimal, GetValueAsInt, GetValueAsShort, GetValueAsString, GetValueListSelection, GetValueListItems
		bool GetValueAsBool(ZWValueId^ id,
#if __cplusplus_cli
			[Out] System::Boolean %
#else
			bool *
#endif
			o_value);

		/// <summary>Gets a value as an 8-bit unsigned integer.</summary>
		/// <param name="id">The unique identifier of the value.</param>
		/// <param name="o_value">a Byte that will be filled with the value.</param>
		/// <returns>true if the value was obtained.  Returns false if the value is not a ZWValueID::ValueType_Byte. The type can be tested with a call to ZWValueID::GetType</returns>
		// \see ValueID::GetType, GetValueAsBool, GetValueAsDecimal, GetValueAsInt, GetValueAsShort, GetValueAsString, GetValueListSelection, GetValueListItems
		bool GetValueAsByte(ZWValueId^ id,
#if __cplusplus_cli
			[Out] System::Byte %
#else
			byte *
#endif
			o_value);

		// <summary>Gets a value as a decimal.</summary>
		// <param name="id">The unique identifier of the value.</param>
		// <param name="o_value">a Decimal that will be filled with the value.</param>
		// <returns>true if the value was obtained.  Returns false if the value is not a ZWValueID::ValueType_Decimal. The type can be tested with a call to ZWValueID::GetType</returns>
		// \see ValueID::GetType, GetValueAsBool, GetValueAsByte, GetValueAsInt, GetValueAsShort, GetValueAsString, GetValueListSelection, GetValueListItems
		//bool GetValueAsDecimal(ZWValueId^ id, System::Decimal %o_value);

		/// <summary>Gets a value as a 32-bit signed integer.</summary>
		/// <param name="id">The unique identifier of the value.</param>
		/// <param name="o_value">an Int32 that will be filled with the value.</param>
		/// <returns>true if the value was obtained.  Returns false if the value is not a ZWValueID::ValueType_Int. The type can be tested with a call to ZWValueID::GetType</returns>
		// \see ValueID::GetType, GetValueAsBool, GetValueAsByte, GetValueAsDecimal, GetValueAsShort, GetValueAsString, GetValueListSelection, GetValueListItems
		bool GetValueAsInt(ZWValueId^ id,
#if __cplusplus_cli
			[Out] System::Int32 %
#else
			int32 *
#endif
			o_value);

		/// <summary>Gets a value as a 16-bit signed integer.</summary>
		/// <param name="id">The unique identifier of the value.</param>
		/// <param name="o_value">an Int16 that will be filled with the value.</param>
		/// <returns>true if the value was obtained.  Returns false if the value is not a ZWValueID::ValueType_Short. The type can be tested with a call to ZWValueID::GetType</returns>
		// \see ValueID::GetType, GetValueAsBool, GetValueAsByte, GetValueAsDecimal, GetValueAsInt, GetValueAsString, GetValueListSelection, GetValueListItems
		bool GetValueAsShort(ZWValueId^ id,
#if __cplusplus_cli
			[Out] System::Int16 %
#else
			int16 *
#endif
			o_value);

		/// <summary>Gets a value as a string.</summary>
		/// <remarks>Creates a string representation of a value, regardless of type.</remarks>
		/// <param name="id">The unique identifier of the value.</param>
		/// <param name="o_value">a String that will be filled with the value.</param>
		/// <returns>true if the value was obtained.</returns>
		bool GetValueAsString(ZWValueId^ id,
#if __cplusplus_cli
			[Out] String^ %
#else
			String^ *
#endif
			o_value);

		/// <summary>Gets a value as a collection of bytes.</summary>
		/// <remarks>Returns a raw representation of a value, regardless of type.</remarks>
		/// <param name="id">The unique identifier of the value.</param>
		/// <param name="o_value">a String that will be filled with the value.</param>
		/// <returns><c>true</c> if the value was obtained. Returns <c>false</c> if the value is not a ValueType.Raw.
		/// The type can be tested with a call to ValueID.GetType.</returns>
		bool GetValueAsRaw(ZWValueId^ id,
#if __cplusplus_cli
			[Out] cli::array<Byte>^ %o_value);
#else
			Platform::Array<byte>^ *o_value);
#endif

	    /// <summary>Gets the selected item from a list value (as a string).</summary>
		/// <param nam="id">The unique identifier of the value.</param>
		/// <param nam="o_value">A String that will be filled with the selected item.</param>
		/// <returns>True if the value was obtained.  Returns false if the value is not a ZWValueID::ValueType_List. The type can be tested with a call to ZWValueID::GetType</returns>
		// \see ValueID::GetType, GetValueAsBool, GetValueAsByte, GetValueAsDecimal, GetValueAsInt, GetValueAsShort, GetValueAsString, GetValueListItems
		bool GetValueListSelection(ZWValueId^ id,
#if __cplusplus_cli
			[Out] String^ %
#else
			String^ *
#endif
			o_value);

		/// <summary>Gets the selected item from a list value (as an integer).</summary>
		/// <param name="id">The unique identifier of the value.</param>
		/// <param name="o_value">An integer that will be filled with the selected item.</param>
		/// <returns>True if the value was obtained.  Returns false if the value is not a ZWValueID::ValueType_List. The type can be tested with a call to ZWValueID::GetType</returns>
		// \see ValueID::GetType, GetValueAsBool, GetValueAsByte, GetValueAsDecimal, GetValueAsInt, GetValueAsShort, GetValueAsString, GetValueListItems
		bool GetValueListSelection(ZWValueId^ id,
#if __cplusplus_cli
			[Out] System::Int32 %
#else
			int32 *
#endif
			o_value);

		/// <summary>Gets the list of items from a list value.</summary>
		/// <param nam="id">The unique identifier of the value.</param>
		/// <param nam="o_value">List that will be filled with list items.</param>
		/// <returns>true if the list items were obtained.  Returns false if the value is not a ZWValueID::ValueType_List. The type can be tested with a call to ZWValueID::GetType</returns>
		// \see ValueID::GetType, GetValueAsBool, GetValueAsByte, GetValueAsDecimal, GetValueAsInt, GetValueAsShort, GetValueAsString, GetValueListSelection
		bool GetValueListItems(ZWValueId^ id,
#if __cplusplus_cli
			[Out] cli::array<String^>^ %o_value);
#else
		Platform::Array<String^>^ *o_value);
#endif

		/// <summary>Gets the list of values from a list value.</summary>
		/// <param name="id">The unique identifier of the value.</param>
		/// <param name="o_value">Pointer to a vector of integers that will be filled with list items. The vector will be cleared before the items are added.</param>
		/// <returns>true if the list values were obtained.  Returns false if the value is not a ValueID::ValueType_List. The type can be tested with a call to ValueID::GetType.</returns>
		// \see ValueID::GetType, GetValueAsBool, GetValueAsByte, GetValueAsFloat, GetValueAsInt, GetValueAsShort, GetValueAsString, GetValueListSelection, GetValueAsRaw
		bool GetValueListValues(ZWValueId^ id,
#if __cplusplus_cli
			[Out] cli::array<int>^ %o_value);
#else
			Platform::Array<int>^ *o_value);
#endif

		/// <summary>Sets the state of a bool.</summary>
		/// <remarks>
		/// Due to the possibility of a device being asleep, the command is assumed to suceeed, and the value
		/// held by the node is updated directly.  This will be reverted by a future status message from the device
		/// if the Z-Wave message actually failed to get through.  Notification callbacks will be sent in both cases.</remarks>
		/// <param name="id">The unique identifier of the bool value.</param>
		/// <param name="value">The new value of the bool.</param>
		/// <returns>true if the value was set.  Returns false if the value is not a ZWValueID::ValueType_Bool. The type can be tested with a call to ZWValueID::GetType</returns>
		bool SetValue(ZWValueId^ id, bool value) { return Manager::Get()->SetValue(id->CreateUnmanagedValueID(), value); }

		/// <summary>Sets the value of a byte.</summary>
		/// <remarks>
		/// Due to the possibility of a device being asleep, the command is assumed to suceeed, and the value
		/// held by the node is updated directly.  This will be reverted by a future status message from the device
		/// if the Z-Wave message actually failed to get through.  Notification callbacks will be sent in both cases.</remarks>
		/// <param name="id">The unique identifier of the byte value.</param>
		/// <param name="value">The new value of the byte.</param>
		/// <returns>true if the value was set.  Returns false if the value is not a ZWValueID::ValueType_Byte. The type can be tested with a call to ZWValueID::GetType</returns>
		bool SetValue(ZWValueId^ id, uint8 value) { return Manager::Get()->SetValue(id->CreateUnmanagedValueID(), value); }

		/// <summary>Sets the value of a decimal.</summary>
		/// <remarks>
		/// It is usually better to handle decimal values using strings rather than floats, to avoid floating point accuracy issues.
		/// Due to the possibility of a device being asleep, the command is assumed to suceeed, and the value
		/// held by the node is updated directly.  This will be reverted by a future status message from the device
		/// if the Z-Wave message actually failed to get through.  Notification callbacks will be sent in both cases.</remarks>
		/// <param name="id">The unique identifier of the decimal value.</param>
		/// <param name="value">The new value of the decimal.</param>
		/// <returns>true if the value was set.  Returns false if the value is not a ZWValueID::ValueType_Decimal. The type can be tested with a call to ZWValueID::GetType</returns>
		bool SetValue(ZWValueId^ id, float value) { return Manager::Get()->SetValue(id->CreateUnmanagedValueID(), value); }

		/// <summary>Sets the value of a 32-bit signed integer.</summary>
		/// <remarks>
		/// Due to the possibility of a device being asleep, the command is assumed to suceeed, and the value
		/// held by the node is updated directly.  This will be reverted by a future status message from the device
		/// if the Z-Wave message actually failed to get through.  Notification callbacks will be sent in both cases.</remarks>
		/// <param name="id">The unique identifier of the integer value.</param>
		/// <param name="value">The new value of the integer.</param>
		/// <returns>true if the value was set.  Returns false if the value is not a ZWValueID::ValueType_Int. The type can be tested with a call to ZWValueID::GetType</returns>
		bool SetValue(ZWValueId^ id, int32 value) { return Manager::Get()->SetValue(id->CreateUnmanagedValueID(), value); }

		/// <summary>Sets the value of a 16-bit signed integer.</summary>
		/// <remarks>
		/// Due to the possibility of a device being asleep, the command is assumed to suceeed, and the value
		/// held by the node is updated directly.  This will be reverted by a future status message from the device
		/// if the Z-Wave message actually failed to get through.  Notification callbacks will be sent in both cases.</remarks>
		/// <param name="id">The unique identifier of the integer value.</param>
		/// <param name="value">The new value of the integer.</param>
		/// <returns>true if the value was set.  Returns false if the value is not a ZWValueID::ValueType_Short. The type can be tested with a call to ZWValueID::GetType</returns>
		bool SetValue(ZWValueId^ id, int16 value) { return Manager::Get()->SetValue(id->CreateUnmanagedValueID(), value); }

		/// <summary>Sets the value from a string, regardless of type.</summary>
		/// <remarks>Due to the possibility of a device being asleep, the command is assumed to suceeed, and the value
		/// held by the node is updated directly.  This will be reverted by a future status message from the device
		/// if the Z-Wave message actually failed to get through.  Notification callbacks will be sent in both cases.</remarks>
		/// <param name="id">The unique identifier of the integer value.</param>
		/// <param name="value">The new value of the string.</param>
		/// <returns>true if the value was set.  Returns false if the value could not be parsed into the correct type for the value.</returns>
		bool SetValue(ZWValueId^ id, String^ value) { return Manager::Get()->SetValue(id->CreateUnmanagedValueID(), ConvertString(value)); }

		/// <summary>Sets the value of a collection of bytes.</summary>
		/// <remarks>Due to the possibility of a device being asleep, the command is assumed to succeed, and the value
		/// held by the node is updated directly. This will be reverted by a future status message from the device if
		/// the Z-Wave message actually failed to get through. Notification callbacks will be sent in both cases.</remarks>
		/// <param name="id">The unique identifier of the raw value.</param>
		/// <param name="value">The new collection of bytes.</param>
		/// <returns>true if the value was set.  Returns false if the value could not be parsed into the correct type for the value.</returns>
		bool SetValue(ZWValueId^ id,
#if __cplusplus_cli
			cli::array<Byte>^ value)
#else
		const Platform::Array<byte>^ value)
#endif
		{
#if __cplusplus_cli
			pin_ptr<uint8> p = &value[0];
			uint8* data = p;
#else
			uint8* data = value->Data;
#endif
			return Manager::Get()->SetValue(id->CreateUnmanagedValueID(), data, (uint8_t)value->Length);
		}

		/// <summary>Sets the selected item in a list.</summary>
		/// <remarks>
		/// Due to the possibility of a device being asleep, the command is assumed to suceeed, and the value
		/// held by the node is updated directly.  This will be reverted by a future status message from the device
		/// if the Z-Wave message actually failed to get through.  Notification callbacks will be sent in both cases.</remarks>
		/// <param name="id">The unique identifier of the list value.</param>
		/// <param name="value">A string matching the new selected item in the list.</param>
		/// <returns>true if the value was set.  Returns false if the selection is not in the list, or if the value is not a ZWValueID::ValueType_List.
		/// The type can be tested with a call to ZWValueID::GetType</returns>
		bool SetValueListSelection(ZWValueId^ id, String^ selectedItem) { return Manager::Get()->SetValueListSelection(id->CreateUnmanagedValueID(), ConvertString(selectedItem)); }

		/// <summary>Refreshes the specified value from the Z-Wave network.</summary>
		/// <remarks>A call to this function causes the library to send a message to the network to retrieve the current value
		/// of the specified ValueID (just like a poll, except only one-time, not recurring).</remarks>
		/// <param name="id">The unique identifier of the value to be refreshed.</param>
		/// <returns>true if the driver and node were found; false otherwise</returns>
		bool RefreshValue(ZWValueId^ id) { return Manager::Get()->RefreshValue(id->CreateUnmanagedValueID()); }

		/// <summary>Sets a flag indicating whether value changes noted upon a refresh should be verified.  If so, the
		/// library will immediately refresh the value a second time whenever a change is observed.  This helps to filter
		/// out spurious data reported occasionally by some devices.</summary>
		/// <param name="id">The unique identifier of the value whose changes should or should not be verified.</param>
		/// <param name="verify">if true, verify changes; if false, don't verify changes.</param>
		void SetChangeVerified(ZWValueId^ id, bool verify) { Manager::Get()->SetChangeVerified(id->CreateUnmanagedValueID(), verify); }

		/// <summary>Starts an activity in a device.</summary>
		/// <remarks>Since buttons are write-only values that do not report a state, no notification callbacks are sent.</remarks>
		/// <param name="id">The unique identifier of the integer value.</param>
		/// <returns>true if the activity was started.  Returns false if the value is not a ZWValueID::ValueType_Button. The type can be tested with a call to ZWValueID::GetType</returns>
		bool PressButton(ZWValueId^ id) { return Manager::Get()->PressButton(id->CreateUnmanagedValueID()); }

		/// <summary>Stops an activity in a device.</summary>
		/// <remarks>Since buttons are write-only values that do not report a state, no notification callbacks are sent.</remarks>
		/// <param name="id">The unique identifier of the integer value.</param>
		/// <returns>true if the activity was stopped.  Returns false if the value is not a ZWValueID::ValueType_Button. The type can be tested with a call to ZWValueID::GetType</returns>
		bool ReleaseButton(ZWValueId^ id) { return Manager::Get()->ReleaseButton(id->CreateUnmanagedValueID()); }

		/*@}*/

		//-----------------------------------------------------------------------------
		// Climate Control Schedules
		//-----------------------------------------------------------------------------
		/* Climate Control Schedules
		*  Methods for accessing schedule values.  All the methods require a ValueID, which will have been provided
		*  in the ValueAdded Notification callback when the the value was first discovered by OpenZWave.
		*  <p>The ValueType_Schedule is a specialized Value used to simplify access to the switch point schedule
		*  information held by a setback thermostat that supports the Climate Control Schedule command class.
		*  Each schedule contains up to nine switch points for a single day, consisting of a time in
		*  hours and minutes (24 hour clock) and a setback in tenths of a degree Celsius.  The setback value can
		*  range from -128 (-12.8C) to 120 (12.0C).  There are two special setback values - 121 is used to set
		*  Frost Protection mode, and 122 is used to set Energy Saving mode.
		*  <p>The switch point methods only modify OpenZWave's copy of the schedule information.  Once all changes
		*  have been made, they are sent to the device by calling SetSchedule.
		*/
		/*@{*/

		/// <summary>Get the number of switch points defined in a schedule.</summary>
		/// <param name="id">The unique identifier of the schedule value.</param>
		/// <returns>the number of switch points defined in this schedule.  Returns zero if the value is not a ValueID::ValueType_Schedule. The type can be tested with a call to ValueID::GetType.</returns>
		uint8 GetNumSwitchPoints(ZWValueId^ id) { return Manager::Get()->GetNumSwitchPoints(id->CreateUnmanagedValueID()); }

		/// <summary>Set a switch point in the schedule.</summary>
		/// <remarks>Inserts a new switch point into the schedule, unless a switch point already exists at the specified
		/// time in which case that switch point is updated with the new setback value instead.
		/// A maximum of nine switch points can be set in the schedule.</remarks>
		/// <param name="id">The unique identifier of the schedule value.</param>
		/// <param name="hours">The hours part of the time when the switch point will trigger.  The time is set using
		/// the 24-hour clock, so this value must be between 0 and 23.</param>
		/// <param name="minutes">The minutes part of the time when the switch point will trigger.  This value must be
		/// between 0 and 59.</param>
		/// <param name="setback">The setback in tenths of a degree Celsius.  The setback value can range from -128 (-12.8C)
		/// to 120 (12.0C).  There are two special setback values - 121 is used to set Frost Protection mode, and
		/// 122 is used to set Energy Saving mode.</param>
		/// <returns>true if successful.  Returns false if the value is not a ValueID::ValueType_Schedule. The type can be tested with a call to ValueID::GetType.</returns>
		/// <seealco cref="GetNumSwitchPoints" />
		/// <seealco cref="RemoveSwitchPoint" /> 
		/// <seealco cref="ClearSwitchPoints" />
		bool SetSwitchPoint(ZWValueId^ id, uint8 hours, uint8 minutes, byte setback) { return Manager::Get()->SetSwitchPoint(id->CreateUnmanagedValueID(), hours, minutes, setback); }

		/// <summary>Remove a switch point from the schedule.</summary>
		/// <remarks>Removes the switch point at the specified time from the schedule.</remarks>
		/// <param name="id">The unique identifier of the schedule value.</param>
		/// <param name="hours">The hours part of the time when the switch point will trigger.  The time is set using
		/// the 24-hour clock, so this value must be between 0 and 23.</param>
		/// <param name="minutes">The minutes part of the time when the switch point will trigger.  This value must be
		/// between 0 and 59.</param>
		/// <returns>true if successful.  Returns false if the value is not a ValueID::ValueType_Schedule or if there
		/// is not switch point with the specified time values. The type can be tested with a call to ValueID::GetType.</returns>
		/// <seealso cref="GetNumSwitchPoints" />
		/// <seealso cref="SetSwitchPoint" />
		/// <seealso cref="ClearSwitchPoints" />
		bool RemoveSwitchPoint(ZWValueId^ id, uint8 hours, uint8 minutes) { return Manager::Get()->RemoveSwitchPoint(id->CreateUnmanagedValueID(), hours, minutes); }

		/// <summary>Clears all switch points from the schedule.</summary>
		/// <param name="id">The unique identifier of the schedule value.</param>
		/// <seealso cref="GetNumSwitchPoints" />
		/// <seealso cref="SetSwitchPoint" />
		/// <seealso cref="RemoveSwitchPoint" />
		void ClearSwitchPoints(ZWValueId^ id) { Manager::Get()->ClearSwitchPoints(id->CreateUnmanagedValueID()); }

		/// <summary>Gets switch point data from the schedule.</summary>
		/// Retrieves the time and setback values from a switch point in the schedule.
        /// <param name="id">The unique identifier of the schedule value.</param>
        /// <param name="idx">The index of the switch point, between zero and one less than the value
		/// returned by <see cref="GetNumSwitchPoints" />.</param>
		/// <param name="o_hours">a pointer to a uint8 that will be filled with the hours part of the switch point data.</param>
		/// <param name="o_minutes">a pointer to a uint8 that will be filled with the minutes part of the switch point data.</param>
		/// <param name="o_setback">a pointer to an int8 that will be filled with the setback value.  This can range from -128
		/// (-12.8C)to 120 (12.0C).  There are two special setback values - 121 is used to set Frost Protection mode, and
		/// 122 is used to set Energy Saving mode.</param>
		/// <returns>true if successful.  Returns false if the value is not a ValueID::ValueType_Schedule. The type can be tested with a call to ValueID::GetType.</returns>
		/// <seealso cref="GetNumSwitchPoints" />
		bool GetSwitchPoint(ZWValueId^ id, uint8 idx,
#if __cplusplus_cli
		[Out] System::Byte %o_hours, [Out] System::Byte %o_minutes, [Out] System::SByte %o_setback);
#else
		byte *o_hours, byte *o_minutes, byte *o_setback);
#endif
		/*@}*/

		//-----------------------------------------------------------------------------
		// Configuration Parameters
		//-----------------------------------------------------------------------------
		/* Configuration Parameters
		*  Methods for accessing device configuration parameters.
		*  Configuration parameters are values that are managed by the Configuration command class.
		*  The values are device-specific and are not reported by the devices. Information on parameters
		*  is provided only in the device user manual.
		*  An ongoing task for the OpenZWave project is to create XML files describing the available
		*  parameters for every Z-Wave.  See the config folder in the project source code for examples.
		*/
		/*@{*/
	public:

		/// <summary>Set the value of a configurable parameter in a device.</summary>
		/// <remarks>Some devices have various parameters that can be configured to control the device behaviour.
		/// These are not reported by the device over the Z-Wave network, but can usually be found in
		/// the device's user manual.
		/// This method returns immediately, without waiting for confirmation from the device that the
		/// change has been made.</remarks>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to configure.</param>
		/// <param name="param">The index of the parameter.</param>
		/// <param name="value">The value to which the parameter should be set.</param>
		/// <returns>true if the a message setting the value was sent to the device.</returns>
		/// <seealso cref="RequestConfigParam" />

		bool SetConfigParam(uint32 homeId, uint8 nodeId, uint8 param, int32 value) { return Manager::Get()->SetConfigParam(homeId, nodeId, param, value); }

		/// <summary>Request the value of a configurable parameter from a device.</summary>
		/// <remarks><para>Some devices have various parameters that can be configured to control the device behaviour.
		/// These are not reported by the device over the Z-Wave network, but can usually be found in
		/// the device's user manual.</para><para>
		/// This method requests the value of a parameter from the device, and then returns immediately,
		/// without waiting for a response.  If the parameter index is valid for this device, and the
		/// device is awake, the value will eventually be reported via a ValueChanged notification callback.
		/// The ValueID reported in the callback will have an index set the same as _param and a command class
		/// set to the same value as returned by a call to Configuration::StaticGetCommandClassId.</para></remarks>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to configure.</param>
		/// <param name="param">The index of the parameter.</param>
		/// <seealso cref="SetConfigParam"/>
		/// <seealso cref="ValueID" />
		/// <seealso cref="Notification" />
		void RequestConfigParam(uint32 homeId, uint8 nodeId, uint8 param) { Manager::Get()->RequestConfigParam(homeId, nodeId, param); }

		/// <summary>Request the values of all known configurable parameters from a device.</summary>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node to configure.</param>
		/// <seealso cref="SetConfigParam" />
		/// <seealso cref="RequestConfigParam" />
		/// <seealso cref="ValueID" />
		/// <seealso cref="Notification" />
		void RequestAllConfigParams(uint32 homeId, uint8 nodeId) { Manager::Get()->RequestAllConfigParams(homeId, nodeId); }
		/*@}*/

		//-----------------------------------------------------------------------------
		// Groups (wrappers for the Node methods)
		//-----------------------------------------------------------------------------
		/** \name Groups
		*  Methods for accessing device association groups.
		*/
		/*@{*/
	public:
		
		/// <summary>Gets the number of association groups reported by this node.</summary>
		/// <remarks>In Z-Wave, groups are numbered starting from one.  For example, if a call to GetNumGroups returns 4, the _groupIdx
		/// value to use in calls to GetAssociations, AddAssociation and RemoveAssociation will be a number between 1 and 4.</remarks>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node whose groups we are interested in.</param>
		/// <returns>The number of groups.</returns>
		/// <seealso cref="GetAssociations" />
		/// <seealso cref="AddAssociation" />
		/// <seealso cref="RemoveAssociation" />
		uint8 GetNumGroups(uint32 homeId, uint8 nodeId) { return Manager::Get()->GetNumGroups(homeId, nodeId); }

		/// <summary>Gets the associations for a group.</summary>
		/// <remarks>Makes a copy of the list of associated nodes in the group, and returns it in an array of uint8's.
		/// The caller is responsible for freeing the array memory with a call to delete [].</remarks>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node whose associations we are interested in.</param>
		/// <param name="groupIdx">One-based index of the group (because Z-Wave product manuals use one-based group numbering).</param>
		/// <param name="o_associations">If the number of associations returned is greater than zero, o_associations will be set to point to an array containing the IDs of the associated nodes.</param>
		/// <returns>The number of nodes in the associations array.  If zero, the array will point to NULL, and does not need to be deleted.</returns>
		/// <seealso cref="GetNumGroups" />
		/// <seealso cref="AddAssociation" />
		/// <seealso cref="RemoveAssociation" />
		uint32 GetAssociations(
#if __cplusplus_cli
		uint32 const homeId, uint8 const nodeId, uint8 const groupIdx,
			[Out] cli::array<Byte>^ %o_associations);
#else
		uint32 homeId, uint8 nodeId, uint8 groupIdx, Platform::Array<byte>^ *o_associations);
#endif

		/// <summary>Gets the maximum number of associations for a group.</summary>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node whose associations we are interested in.</param>
		/// <param name="groupIdx">one-based index of the group (because Z-Wave product manuals use one-based group numbering).</param>
		/// <returns>The maximum number of nodes that can be associated into the group.</returns>
		/// <seealso cref="GetNumGroups" /> 
		/// <seealso cref="AddAssociation" />
		/// <seealso cref="RemoveAssociation" />
		uint8 GetMaxAssociations(uint32 homeId, uint8 nodeId, uint8 groupIdx) { return Manager::Get()->GetMaxAssociations(homeId, nodeId, groupIdx); }

	    /// <summary>Adds a node to an association group.</summary>
	    /// <remarks>Due to the possibility of a device being asleep, the command is assumed to suceeed, and the association data
	    /// held in this class is updated directly.  This will be reverted by a future Association message from the device
	    /// if the Z-Wave message actually failed to get through.  Notification callbacks will be sent in both cases.</remarks>
	    /// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
	    /// <param name="nodeId">The ID of the node whose associations are to be changed.</param>
	    /// <param name="groupIdx">One-based index of the group (because Z-Wave product manuals use one-based group numbering).</param>
	    /// <param name="targetNodeId">Identifier for the node that will be added to the association group.</param>
	    /// <seealso cref="GetNumGroups" />
	    /// <seealso cref="GetAssociations" />
	    /// <seealso cref="RemoveAssociation" />
		void AddAssociation(uint32 homeId, uint8 nodeId, uint8 groupIdx, uint8 targetNodeId) { return Manager::Get()->AddAssociation(homeId, nodeId, groupIdx, targetNodeId); }

		/// <summary>Removes a node from an association group.</summary>
		/// <remarks>Due to the possibility of a device being asleep, the command is assumed to suceeed, and the association data
		/// held in this class is updated directly.  This will be reverted by a future Association message from the device
		/// if the Z-Wave message actually failed to get through.   Notification callbacks will be sent in both cases.</remarks>
		/// <param name="homeId">The Home ID of the Z-Wave controller that manages the node.</param>
		/// <param name="nodeId">The ID of the node whose associations are to be changed.</param>
		/// <param name="groupIdx">One-based index of the group (because Z-Wave product manuals use one-based group numbering).</param>
		/// <param name="targetNodeId">Identifier for the node that will be removed from the association group.</param>
		/// <seealso cref="GetNumGroups" />
		/// <seealso cref="GetAssociations" />
		/// <seealso cref="AddAssociation" />
		void RemoveAssociation(uint32 homeId, uint8 nodeId, uint8 groupIdx, uint8 targetNodeId) { return Manager::Get()->RemoveAssociation(homeId, nodeId, groupIdx, targetNodeId); }
		/*@}*/

		//-----------------------------------------------------------------------------
		// Controller commands
		//-----------------------------------------------------------------------------
		/** \name Controller Commands
		*  Commands for Z-Wave network management using the PC Controller.
		*/
		/*@{*/
	public:

		/*@}*/

		//-----------------------------------------------------------------------------
		// Network commands
		//-----------------------------------------------------------------------------
		/** \name Network Commands
		*  Commands for Z-Wave network for testing, routing and other internal
		*  operations.
		*/
		/*@{*/
	public:
		
		/// <summary>Test network node.</summary>
		/// <remarks>Sends a series of messages to a network node for testing network reliability.</remarks>
		/// <param name="homeId">The Home ID of the Z-Wave controller to be reset.</param>
		/// <param name="count">This is the number of test messages to send.</param>
		/// <seealso cref="TestNetwork(uint,uint)" />
		void TestNetworkNode(uint32 homeId, uint8 nodeId, uint32 count) { Manager::Get()->TestNetworkNode(homeId, nodeId, count); }

		/// <summary>Test network.</summary>
		/// <remarks>Sends a series of messages to every node on the network for testing network reliability.</remarks>
		/// <param name="homeId">The Home ID of the Z-Wave controller to be reset.</param>
		/// <param name="count">This is the number of test messages to send.</param>
		/// <seealso cref="TestNetwork(uint,uint,uint)" />
		void TestNetwork(uint32 homeId, uint32 count) { Manager::Get()->TestNetwork(homeId, count); }

		/// <summary>Heal network node by requesting the node rediscover their neighbors.</summary>
		/// <remarks>Sends a ControllerCommand_RequestNodeNeighborUpdate to the node.</remarks>
		/// <param name="homeId">The Home ID of the Z-Wave network to be healed.</param>
		/// <param name="nodeId">The node to heal.</param>
		/// <param name="doRR">Whether to perform return routes initialization.</param>
		void HealNetworkNode(uint32 homeId, uint8 nodeId, bool doRR) { Manager::Get()->HealNetworkNode(homeId, nodeId, doRR); }

		/// <summary>Heal network by requesting node's rediscover their neighbors.</summary>
		/// <remarks>Sends a ControllerCommand_RequestNodeNeighborUpdate to every node.
		/// Can take a while on larger networks.</remarks>
		/// <param name="homeId">The Home ID of the Z-Wave network to be healed.</param>
		/// <param name="doRR">Whether to perform return routes initialization.</param>
		void HealNetwork(uint32 homeId, bool doRR) { Manager::Get()->HealNetwork(homeId, doRR); }

		/// <summary>Start the Inclusion Process to add a Node to the Network.</summary>
		/// <remarks><para>The Status of the Node Inclusion is communicated via Notifications. Specifically, you should
		/// monitor ControllerCommand Notifications.</para>
		/// <para>Results of the AddNode Command will be send as a Notification with the Notification type as
		/// Notification::Type_ControllerCommand</para></remarks>
		/// <param name="homeId">The Home ID of the Z-Wave network where the device should be added.</param>
		/// <param name="doSecurity">Whether to initialize the Network Key on the device if it supports the Security CC</param>
		/// <returns>if the Command was sent succcesfully to the Controller</returns>
		bool AddNode(uint32 homeId, bool doSecurity) { return Manager::Get()->AddNode(homeId, doSecurity); }

		/// <summary>Remove a Device from the Z-Wave Network</summary>
		/// <remarks><para>The Status of the Node Removal is communicated via Notifications. Specifically, you should
		/// monitor ControllerCommand Notifications.</para>
		/// <para>Results of the AddNode Command will be send as a Notification with the Notification type as
		/// Notification::Type_ControllerCommand</para></remarks>
		/// <param name="homeId">The HomeID of the Z-Wave network where you want to remove the device</param>
		/// <returns>if the Command was send succesfully to the Controller</returns>
		bool RemoveNode(uint32 homeId) { return Manager::Get()->RemoveNode(homeId); }

		/// <summary>Remove a Failed Device from the Z-Wave Network</summary>
		/// <remarks><para>This Command will remove a failed node from the network. The Node should be on the Controllers Failed
		/// Node List, otherwise this command will fail. You can use the HasNodeFailed function below to test if the Controller
		/// believes the Node has Failed.</para><para>
		/// The Status of the Node Removal is communicated via Notifications. Specifically, you should
		/// monitor ControllerCommand Notifications.
		/// </para><para>
		/// Results of the AddNode Command will be send as a Notification with the Notification type as
		/// Notification::Type_ControllerCommand
		/// </para></remarks>
		/// <param name="homeId">The HomeID of the Z-Wave network where you want to remove the device</param>
		/// <param name="nodeId">The NodeID of the Failed Node.</param>
		/// <returns>if the Command was send succesfully to the Controller</returns>
		bool RemoveFailedNode(uint32 homeId, uint8 nodeId) { return Manager::Get()->RemoveFailedNode(homeId, nodeId); }

		/// <summary>Check if the Controller Believes a Node has Failed.</summary>
		/// <remarks><para>This is different from the IsNodeFailed call in that we test the Controllers Failed Node List, whereas the IsNodeFailed is testing
		/// our list of Failed Nodes, which might be different.</para>
		/// <para>The Results will be communicated via Notifications. Specifically, you should monitor the ControllerCommand notifications</para>
		/// <para>Results of the AddNode Command will be send as a Notification with the Notification type as
		/// Notification::Type_ControllerCommand</para></remarks>
		/// <param name="homeId">The HomeID of the Z-Wave network where you want to test the device</param>
		/// <param name="nodeId">The NodeID of the Failed Node.</param>
		/// <returns>if the RemoveDevice Command was send succesfully to the Controller</returns>
		bool HasNodeFailed(uint32 homeId, uint8 nodeId) { return Manager::Get()->HasNodeFailed(homeId, nodeId); }

		/// <summary>Ask a Node to update its update its Return Route to the Controller</summary>
		/// <remarks><para>This command will ask a Node to update its Return Route to the Controller</para>
		/// <para>Results of the AddNode Command will be send as a Notification with the Notification type as
		/// Notification::Type_ControllerCommand</para></remarks>
		/// <param name="homeId">The HomeID of the Z-Wave network where you want to update the device</param>
		/// <param name="nodeId">The NodeID of the Node.</param>
		/// <returns>if the Command was send succesfully to the Controller</returns>
		bool AssignReturnRoute(uint32 homeId, uint8 nodeId) { return Manager::Get()->AssignReturnRoute(homeId, nodeId); }

		/// <summary>Ask a Node to update its Neighbor Tables</summary>
		/// <remarks><para>This command will ask a Node to update its Neighbor Tables.</para>
		/// <para>Results of the AddNode Command will be send as a Notification with the Notification type as
		/// Notification::Type_ControllerCommand</para></remarks>
		/// <param name="homeId">The HomeID of the Z-Wave network where you want to update the device</param>
		/// <param name="nodeId">The NodeID of the Node.</param>
		/// <returns>if the Command was send succesfully to the </returns>
		bool RequestNodeNeighborUpdate(uint32 homeId, uint8 nodeId) { return Manager::Get()->RequestNodeNeighborUpdate(homeId, nodeId); }

		/// <summary>Ask a Node to delete all Return Route.</summary>
		/// <remarks><para>This command will ask a Node to delete all its return routes, and will rediscover when needed.</para>
		/// <para>Results of the AddNode Command will be send as a Notification with the Notification type as
		/// Notification::Type_ControllerCommand</para></remarks>
		/// <param name="homeId">The HomeID of the Z-Wave network where you want to update the device</param>
		/// <param name="nodeId">The NodeID of the Node.</param>
		/// <returns>if the Command was send succesfully to the Controller</returns>
		bool DeleteAllReturnRoutes(uint32 homeId, uint8 nodeId) { return Manager::Get()->DeleteAllReturnRoutes(homeId, nodeId); }

		/// <summary>Send a NIF frame from the Controller to a Node.</summary>
		/// <remarks><para>This command send a NIF frame from the Controller to a Node</para>
		/// <para>Results of the AddNode Command will be send as a Notification with the Notification type as
		/// Notification::Type_ControllerCommand</para></remarks>
		/// <param name="homeId">The HomeID of the Z-Wave network</param>
		/// <param name="nodeId">The NodeID of the Node to recieve the NIF</param>
		/// <returns>if the sendNIF Command was send succesfully to the Controller</returns>
		bool SendNodeInformation(uint32 homeId, uint8 nodeId) { return Manager::Get()->SendNodeInformation(homeId, nodeId); }

		/// <summary>Create a new primary controller when old primary fails. Requires SUC.</summary>
		/// <remarks><para>This command Creates a new Primary Controller when the Old Primary has Failed. Requires a SUC on the network to function</para>
		/// <para>Results of the CreateNewPrimary Command will be send as a Notification with the Notification type as
		/// Notification::Type_ControllerCommand</para></remarks>
		/// <param name="homeId">The HomeID of the Z-Wave network</param>
		/// <returns>if the CreateNewPrimary Command was send succesfully to the Controller</returns>
		/// <seealso cref="CancelControllerCommand" />
		bool CreateNewPrimary(uint32 homeId) { return Manager::Get()->CreateNewPrimary(homeId); }

		/// <summary>Receive network configuration information from primary controller. Requires secondary.</summary>
		/// <remarks><para>This command prepares the controller to recieve Network Configuration from a Secondary Controller.</para>
		/// <para>Results of the ReceiveConfiguration Command will be send as a Notification with the Notification type as
		/// Notification::Type_ControllerCommand</para></remarks>
		/// <param name="homeId">The HomeID of the Z-Wave network</param>
		/// <returns>if the ReceiveConfiguration Command was send succesfully to the Controller</returns>
		/// <seealso cref="CancelControllerCommand" />
		bool ReceiveConfiguration(uint32 homeId) { return Manager::Get()->ReceiveConfiguration(homeId); }

		/// <summary>Replace a failed device with another.</summary>
		/// <remarks><para>If the node is not in the controller's failed nodes list, or the node responds, this command will fail.</para>
		/// <para>You can check if a Node is in the Controllers Failed node list by using the HasNodeFailed method</para>
		/// <para>Results of the ReplaceFailedNode Command will be send as a Notification with the Notification type as
		/// Notification::Type_ControllerCommand</para></remarks>
		/// <param name="homeId">The HomeID of the Z-Wave network</param>
		/// <param name="nodeId">the ID of the Failed Node</param>
		/// <returns>if the ReplaceFailedNode Command was send succesfully to the Controller</returns>
		/// <seealso cref="HasNodeFailed" />
		/// <seealso cref="CancelControllerCommand" />
		bool ReplaceFailedNode(uint32 homeId, uint8 nodeId) { return Manager::Get()->ReplaceFailedNode(homeId, nodeId); }

		/// <summary>Add a new controller to the network and make it the primary.</summary>
		/// <remarks><para>The existing primary will become a secondary controller.</para>
		/// <para>Results of the TransferPrimaryRole Command will be send as a Notification with the Notification type as
		/// Notification::Type_ControllerCommand</para></remarks>
		/// <param name="homeId">The HomeID of the Z-Wave network</param>
		/// <returns>if the TransferPrimaryRole Command was send succesfully to the Controller</returns>
		/// <see cref="CancelControllerCommand" />
		bool TransferPrimaryRole(uint32 homeId) { return Manager::Get()->TransferPrimaryRole(homeId); }

		/// <summary>Update the controller with network information from the SUC/SIS.</summary>
		/// <remarks><para>Results of the RequestNetworkUpdate Command will be send as a Notification with the Notification type asc
		/// Notification::Type_ControllerCommand</para></remarks>
		/// <param name="homeId">The HomeID of the Z-Wave network</param>
		/// <param name="nodeId">the ID of the Node</param>
		/// <returns>if the RequestNetworkUpdate Command was send succesfully to the Controller</returns>
		/// <seealso cref="CancelControllerCommand" />
		bool RequestNetworkUpdate(uint32 homeId, uint8 nodeId) { return Manager::Get()->RequestNetworkUpdate(homeId, nodeId); }

		/// <summary>Send information from primary to secondary</summary>
		/// <remarks><para>Results of the ReplicationSend Command will be send as a Notification with the Notification type as
		/// Notification::Type_ControllerCommand</para></remarks>
		/// <param name="homeId">The HomeID of the Z-Wave network</param>
		/// <param name="nodeId">the ID of the Node</param>
		/// <returns>if the ReplicationSend Command was send succesfully to the Controller</returns>
		/// <seealso cref="CancelControllerCommand" />
		bool ReplicationSend(uint32 homeId, uint8 nodeId) { return Manager::Get()->ReplicationSend(homeId, nodeId); }

		/// <summary>Create a handheld button id.</summary>
		/// <remarks><para>Only intended for Bridge Firmware Controllers.</para>
		/// <para>Results of the CreateButton Command will be send as a Notification with the Notification type as
		/// Notification::Type_ControllerCommand</para></remarks>
		/// <param name="homeId">The HomeID of the Z-Wave network</param>
		/// <param name="nodeId">the ID of the Virtual Node</param>
		/// <param name="buttonId">the ID of the Button to create</param>
		/// <returns>if the CreateButton Command was send succesfully to the Controller</returns>
		/// <seealso cref="CancelControllerCommand" />
		bool CreateButton(uint32 homeId, uint8 nodeId, uint8 buttonid) { return Manager::Get()->CreateButton(homeId, nodeId, buttonid); }

		/// <summary>Delete a handheld button id.</summary>
		/// <remarks><para>Only intended for Bridge Firmware Controllers.</para>
		/// <para>Results of the DeleteButton Command will be send as a Notification with the Notification type as
		/// Notification::Type_ControllerCommand</para></remarks>
		/// <param name="homeId">The HomeID of the Z-Wave network</param>
		/// <param name="nodeId">the ID of the Virtual Node</param>
		/// <param name="buttonId">the ID of the Button to delete</param>
		/// <returns>if the DeleteButton Command was send succesfully to the Controller</returns>
		/// <seealso cref="CancelControllerCommand" />
		bool DeleteButton(uint32 homeId, uint8 nodeId, uint8 buttonid) { return Manager::Get()->DeleteButton(homeId, nodeId, buttonid); }

		//-----------------------------------------------------------------------------
		// Controller commands
		//-----------------------------------------------------------------------------
		/** \name Controller Commands
		*  Commands for Z-Wave network management using the PC Controller.
		*/
		/*@{*/

	public:
		/// <summary>Hard Reset a PC Z-Wave Controller.</summary>
		/// <remarks>
		/// Resets a controller and erases its network configuration settings.
		/// The controller becomes a primary controller ready to add devices to a new network.
  		/// </remarks>
		/// <param name="homeId">The Home ID of the Z-Wave controller to be reset.</param>
		/// <seealso cref="SoftReset" />
		void ResetController(uint32 homeId) { Manager::Get()->ResetController(homeId); }

		/// <summary>Soft Reset a PC Z-Wave Controller.</summary>
		/// <remarks>Resets a controller without erasing its network configuration settings.</remarks>
		/// <param name="homeId">The Home ID of the Z-Wave controller to be reset.</param>
		/// <seealso cref="ResetController" />
		void SoftReset(uint32 homeId) { Manager::Get()->SoftReset(homeId); }

		/// <summary>Cancels any in-progress command running on a controller.</summary>
		/// <param name="homeId">The Home ID of the Z-Wave controller.</param>
		/// <returns>true if a command was running and was cancelled.</returns>
		bool CancelControllerCommand(uint32 homeId) { return Manager::Get()->CancelControllerCommand(homeId); }

#if __cplusplus_cli
	private:
		void  OnNotificationFromUnmanaged(Notification* _notification, void* _context);					// Forward notification to managed delegates hooked via Event addhandler 
	
		GCHandle										m_gchNotification;
		OnNotificationFromUnmanagedDelegate^			m_onNotification;
#else
	internal:
		static void OnNotificationFromUnmanaged(OpenZWave::Notification const * _notification, void * _context);
#endif

	private:
		std::string ConvertString(String^ value) {
#if __cplusplus_cli
			return msclr::interop::marshal_as<std::string>(value);
#else
			std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
			return convert.to_bytes(value->Data());
#endif
		}

		String^ ConvertString(std::string value) {
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
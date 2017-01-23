An Open-ZWave wrapper for use with .NET or UWP apps

### Usage
```csharp
//Initialize
ZWMOptions.Instance.Initialize(); //Configure default options
ZWMOptions.Instance.Lock();       //Options must be locked before using
ZWManager.Instance.Initialize();  //Start up the manager
ZWManager.Instance.OnNotification += OnNodeNotification; //Start listening for node events
//Hook up the serial port
var serialPortSelector = Windows.Devices.SerialCommunication.SerialDevice.GetDeviceSelector();
var devices = await DeviceInformation.FindAllAsync(serialPortSelector);
var serialPort = devices.First().Id; //Adjust to pick the right port for your usb stick
ZWManager.Instance.AddDriver(serialPort); //Add the serial port (you can have multiple!)# openzwave-dotnet-uwp

// The rest is in the Notification handler. Every time a node is found, changed, removed etc. 
// an event is reported here, including responses to commands you send. Nodes are identified
// by the HomeID (one per usb controller connected), and by the NodeID. You use these two values 
// to uniquely identify a node on your network, and can then oerform operations like changing 
// properties via the ZWManager instance.
```

### UWP Sample app:
![UWP Sample app](https://github.com/dotMorten/openzwave-dotnet-uwp/blob/master/Samples/UWP/OZWAppxScreenshot.png)

### Configuring the app
It's important that you deploy all the XML configuration files from `\open-zwave\config\**\*.*` into the /config/ folder of your app. The sample apps uses a `DeployConfigFiles.targets` file to do this by including this in your project:
```
  <Import Project="DeployConfigFiles.targets" />
```
Note, that if you use the nuget package this will automatically happen, as this .targets file is automatically included.

In UWP you also need to allow the usage of serial ports. To do this, you must add the following section to the Capabilities part of the `Package.appxmanifest`:
```xml
  <Capabilities>
    <DeviceCapability Name="serialcommunication">
      <Device Id="any">
        <Function Type="name:serialPort" />
      </Device>
    </DeviceCapability>
  </Capabilities>
```

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

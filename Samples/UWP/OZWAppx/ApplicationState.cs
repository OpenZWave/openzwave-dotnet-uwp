using OpenZWave;
using OpenZWave.NetworkManager;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
#if NETFX_CORE
using Windows.Devices.Enumeration;
#endif

namespace OZWAppx
{
    public class ApplicationState
    {
        static ApplicationState _state;
        public static ApplicationState Instance
        {
            get
            {
                return _state ?? (_state = new ApplicationState());
            }
        }

        Task initializeTask;
        private ApplicationState()
        {
        }
        public Task InitializeAsync()
        {
            initializeTask = Initialize();
            return initializeTask;
        }

        private Task Initialize()
        {
            return RefreshSerialPortsAsync();
        }

        public async Task RefreshSerialPortsAsync()
        {
            var activePorts = SerialPorts.Where(p => p.IsActive).Select(p => p.PortID).ToList();
            SerialPorts.Clear();
#if NETFX_CORE
            var serialPortSelector = Windows.Devices.SerialCommunication.SerialDevice.GetDeviceSelector();
            var devices = await DeviceInformation.FindAllAsync(serialPortSelector);
            foreach (var item in devices)
            {
                bool isActive = activePorts.Contains(item.Id);
                SerialPorts.Add(new SerialPortInfo(item.Id, item.Name, isActive));
                if (isActive)
                    activePorts.Remove(item.Id);
            }
#else //.NET
            foreach(var item in System.IO.Ports.SerialPort.GetPortNames())
            {
                bool isActive = activePorts.Contains(item);
                SerialPorts.Add(new SerialPortInfo(item, item, isActive));
                if (isActive)
                    activePorts.Remove(item);
            }
#endif
#if DEBUG
            // SerialPorts.Add(new SerialPortInfo("//COM-TEST/1", "Test Port 1"));
            // SerialPorts.Add(new SerialPortInfo("//COM-TEST/2", "Test Port 2"));
            // SerialPorts.Add(new SerialPortInfo("//COM-TEST/3", "Test Port 3"));
            // SerialPorts.Add(new SerialPortInfo("//COM-TEST/4", "Test Port 4"));
#endif

            // Remove controllers on ports that were active but lost
            foreach (var item in activePorts)
            {
                Watcher.Instance.RemoveController(item);
            }
        }

        public ObservableCollection<SerialPortInfo> SerialPorts { get; } = new ObservableCollection<SerialPortInfo>();

    }


    public sealed class SerialPortInfo
    {
        internal SerialPortInfo(string id, string name, bool isActive = false)
        {
            PortID = id;
            Name = name;
        }
        public string PortID { get; }
        public string Name { get; }

        private bool _isActive;

        public bool IsActive
        {
            get { return _isActive; }
            set
            {
                _isActive = value;
                if (value)
                {
                    Watcher.Instance.AddController(PortID, Name);
                }
                else
                {
                    Watcher.Instance.RemoveController(PortID);
                }
            }
        }
    }
}

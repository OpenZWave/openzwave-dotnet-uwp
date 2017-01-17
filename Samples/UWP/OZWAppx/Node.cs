using OpenZWave;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace OZWAppx
{
    /// <summary>
    /// Container for Nodes
    /// </summary>
    public class Node : INotifyPropertyChanged
    {
        private Byte m_id = 0;
        /// <summary>
        /// Gets or sets the identifier.
        /// </summary>
        /// <value>
        /// The identifier.
        /// </value>
        public Byte ID
        {
            get { return m_id; }
            set { m_id = value; OnPropertyChanged(); }
        }

        private bool m_isLoading = true;

        public bool IsLoading
        {
            get { return m_isLoading; }
            set { m_isLoading = value;  OnPropertyChanged(); }
        }

        public bool HasNodeFailed
        {
            get
            {
                return MainViewModel.Instance.Manager.HasNodeFailed(HomeID, ID);
            }
        }

        private UInt32 m_homeId = 0;
        /// <summary>
        /// Gets or sets the home identifier.
        /// </summary>
        /// <value>
        /// The home identifier.
        /// </value>
        public UInt32 HomeID
        {
            get { return m_homeId; }
            set { m_homeId = value; OnPropertyChanged(); }
        }

        private String m_name = "";
        /// <summary>
        /// Gets or sets the name.
        /// </summary>
        /// <value>
        /// The name.
        /// </value>
        public String Name
        {
            get { return m_name; }
            set
            {
                if (m_name != value)
                {
                    m_name = value;
                    OnPropertyChanged();
                    MainViewModel.Instance.Manager.SetNodeName(m_homeId, m_id, value);
                }
            }
        }

        private String m_location = "";
        /// <summary>
        /// Gets or sets the location.
        /// </summary>
        /// <value>
        /// The location.
        /// </value>
        public String Location
        {
            get { return m_location; }
            set
            {
                if (m_location != value)
                {
                    m_location = value;
                    OnPropertyChanged();
                    MainViewModel.Instance.Manager.SetNodeLocation(m_homeId, m_id, value);
                }
            }
        }

        private String m_label = "";
        /// <summary>
        /// Gets or sets the label.
        /// </summary>
        /// <value>
        /// The label.
        /// </value>
        public String Label
        {
            get { return m_label; }
            set { m_label = value; OnPropertyChanged(); }
        }

        private String m_manufacturer = "";
        /// <summary>
        /// Gets or sets the manufacturer.
        /// </summary>
        /// <value>
        /// The manufacturer.
        /// </value>
        public String Manufacturer
        {
            get { return m_manufacturer; }
            set { if (value != "Unknown: id=0000") { m_manufacturer = value; OnPropertyChanged(); } }
        }

        private String m_product = "";
        /// <summary>
        /// Gets or sets the product.
        /// </summary>
        /// <value>
        /// The product.
        /// </value>
        public String Product
        {
            get { return m_product; }
            set
            {
                if (value != "Unknown: type=0000, id=0000")
                {
                    m_product = value; OnPropertyChanged();
                }
            }
        }

        private ObservableCollection<ZWValueID> m_values = new ObservableCollection<ZWValueID>();

        public event PropertyChangedEventHandler PropertyChanged;

        private void OnPropertyChanged([CallerMemberName]string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        /// <summary>
        /// Gets the values.
        /// </summary>
        /// <value>
        /// The values.
        /// </value>
        public IList<ZWValueID> Values
        {
            get { return m_values; }
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="Node"/> class.
        /// </summary>
        public Node()
        {
        }

        /// <summary>
        /// Adds the value.
        /// </summary>
        /// <param name="valueID">The value identifier.</param>
        public void AddValue(ZWValueID valueID)
        {
            var id = m_values.Where(v => v.Genre == valueID.Genre && v.CommandClassId == valueID.CommandClassId && v.Id == valueID.Id).FirstOrDefault();
            if (id != null)
            {
                m_values[m_values.IndexOf(id)] = valueID;
            }
            else
            {
                m_values.Add(valueID);
            }
        }

        /// <summary>
        /// Removes the value.
        /// </summary>
        /// <param name="valueID">The value identifier.</param>
        public void RemoveValue(ZWValueID valueID)
        {
            m_values.Remove(valueID);
        }
    }
}

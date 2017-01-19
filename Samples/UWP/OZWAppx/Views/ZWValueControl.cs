using OpenZWave;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace OZWAppx.Views
{
    public class ZWValueControl : ContentControl
    {
        private ComboBox comboBox = new ComboBox();
        private CheckBox checkBox = new CheckBox() { HorizontalAlignment = HorizontalAlignment.Right };
        private TextBox textBox = new TextBox() { TextAlignment = TextAlignment.Right, BorderThickness = new Thickness(1) };
        private Grid g = new Grid();
        private TextBlock tbU = new TextBlock() { VerticalAlignment = VerticalAlignment.Center };
        private bool updating;

        public ZWValueControl()
        {
            HorizontalAlignment = HorizontalAlignment.Stretch;
            checkBox.Checked += CheckBox_Checked;
            checkBox.Unchecked += CheckBox_Checked;
            comboBox.SelectionChanged += ComboBox_SelectionChanged;
            textBox.TextChanged += TextBox_TextChanged;

            g = new Grid() { HorizontalAlignment = HorizontalAlignment.Stretch };
            g.ColumnDefinitions.Add(new ColumnDefinition());
            g.ColumnDefinitions.Add(new ColumnDefinition() { Width = GridLength.Auto });
            Grid.SetColumn(tbU, 1);
            g.Children.Add(textBox);
            g.Children.Add(tbU);
        }

        private void TextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (Value.Type == ZWValueType.String)
            {
                ZWManager.Instance.SetValue(Value, textBox.Text);
            }
            else if (Value.Type == ZWValueType.Decimal)
            {
                Decimal d;
                if (Decimal.TryParse(textBox.Text, out d))
                    ZWManager.Instance.SetValue(Value, textBox.Text);
                else
                    Rebuild();
            }
            else if (Value.Type == ZWValueType.Int)
            {
                int d;
                if (int.TryParse(textBox.Text, out d))
                    ZWManager.Instance.SetValue(Value, d);
                else
                    Rebuild();
            }
            else if (Value.Type == ZWValueType.Short)
            {
                short d;
                if (short.TryParse(textBox.Text, out d))
                    ZWManager.Instance.SetValue(Value, d);
                else
                    Rebuild();
            }
            else if (Value.Type == ZWValueType.Byte)
            {
                byte d;
                if (byte.TryParse(textBox.Text, out d))
                    ZWManager.Instance.SetValue(Value, d);
                else
                    Rebuild();
            }
            else
                Rebuild(); //TODO
        }

        private void ComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (updating)
                return;
            var newValue = e.AddedItems?.FirstOrDefault() as string;
            if(newValue != null)
                ZWManager.Instance.SetValueListSelection(Value, newValue);
        }

        private void CheckBox_Checked(object sender, RoutedEventArgs e)
        {
            if(!updating)
                ZWManager.Instance.SetValue(Value, checkBox.IsChecked.Value);
        }

        private void Rebuild()
        {
            if (Value == null)
            {
                Content = null;
            }
            else
            {
                updating = true;
                bool isReadOnly = ZWManager.Instance.IsValueReadOnly(Value);
                var value = GetStringValue(Value);
                if(Value.Type == ZWValueType.List && !isReadOnly)
                {
                    string[] values;
                    ZWManager.Instance.GetValueListItems(Value, out values);
                    comboBox.ItemsSource = values;
                    if(values != null)
                    {
                        comboBox.SelectedItem = (string)value;
                    }
                    Content = comboBox;
                    
                }
                else if(Value.Type == ZWValueType.Bool && !isReadOnly)
                {
                    checkBox.IsChecked = (bool)value;
                    Content = checkBox;
                }
                else
                {
                    var unit = ZWManager.Instance.GetValueUnits(Value);
                     //TextBlock tb = new TextBlock() { Text = stringValue };
                    if(isReadOnly)
                        Content = new TextBlock() { Text = $"{value} {unit}".Trim(), HorizontalAlignment = HorizontalAlignment.Right };
                    else
                    {
                        textBox.Text = value.ToString();
                        textBox.InputScope = new Windows.UI.Xaml.Input.InputScope();
                        if (Value.Type == ZWValueType.String)
                            textBox.InputScope.Names.Add(new Windows.UI.Xaml.Input.InputScopeName(Windows.UI.Xaml.Input.InputScopeNameValue.Default));
                        else
                            textBox.InputScope.Names.Add(new Windows.UI.Xaml.Input.InputScopeName(Windows.UI.Xaml.Input.InputScopeNameValue.Digits));
                        tbU.Text = unit;
                        Content = g;
                    }
                }
            }
            updating = false;
        }
        
        public OpenZWave.ZWValueID Value
        {
            get { return (OpenZWave.ZWValueID)GetValue(ValueProperty); }
            set { SetValue(ValueProperty, value); }
        }

        // Using a DependencyProperty as the backing store for Value.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty ValueProperty =
            DependencyProperty.Register("Value", typeof(OpenZWave.ZWValueID), typeof(ZWValueControl), new PropertyMetadata(null, ValuePropertyChanged));

        private static void ValuePropertyChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            (d as ZWValueControl).Rebuild();
        }


        private object GetStringValue(ZWValueID v)
        {
            var manager = ZWManager.Instance;
            switch (v.Type)
            {
                case ZWValueType.Bool:
                    bool r1;
                    manager.GetValueAsBool(v, out r1);
                    return r1;
                case ZWValueType.Byte:
                    byte r2;
                    manager.GetValueAsByte(v, out r2);
                    return r2;
                case ZWValueType.Decimal:
                    decimal r3;
                    string r3s;
                    manager.GetValueAsString(v, out r3s);
                    return r3s;
                //throw new NotImplementedException("Decimal");
                //m_manager.GetValueAsDecimal(v, out r3);
                //return r3.ToString();
                case ZWValueType.Int:
                    Int32 r4;
                    manager.GetValueAsInt(v, out r4);
                    return r4;
                case ZWValueType.List:
                    string value;
                    manager.GetValueListSelection(v, out value);
                    return value;
                case ZWValueType.Schedule:
                    return "Schedule";
                case ZWValueType.Short:
                    short r7;
                    manager.GetValueAsShort(v, out r7);
                    return r7;
                case ZWValueType.String:
                    string r8;
                    manager.GetValueAsString(v, out r8);
                    return r8;
                default:
                    return "";
            }
        }
    }
}

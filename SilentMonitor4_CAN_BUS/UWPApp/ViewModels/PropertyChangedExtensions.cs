using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace UWPApp.ViewModels
{
    public static class PropertyChangedExtensions
    {
        public static void Invoke(this PropertyChangedEventHandler propertyChanged, object sender, PropertyChangedEventArgs eventArgs)
        {
            if (propertyChanged != null)
            {
                propertyChanged(sender, eventArgs);
            }
        }
    }
}

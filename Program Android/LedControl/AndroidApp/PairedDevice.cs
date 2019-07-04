using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Android.App;
using Android.Content;
using Android.OS;
using Android.Runtime;
using Android.Views;
using Android.Widget;

namespace AndroidApp
{
    public class PairedDevice
    {
        public string Name { get; }
        public bool Connected { get; set; }

        public PairedDevice(string name)
        {
            Name = name;
        }
    }
}
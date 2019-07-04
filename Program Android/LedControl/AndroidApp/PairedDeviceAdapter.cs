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
    public class PairedDeviceAdapter : BaseAdapter<PairedDevice>
    {
        private Context CurrentContext;

        public PairedDeviceAdapter(Context context)
        {
            CurrentContext = context;
        }

        public override PairedDevice this[int position]
        {
            get
            {
                return BT.PairedDevices[position];
            }
        }

        public override int Count
        {
            get
            {
                return BT.PairedDevices.Count;
            }
        }

        public override long GetItemId(int position)
        {
            return position;
        }

        public override View GetView(int position, View convertView, ViewGroup parent)
        {
            View row = LayoutInflater.From(CurrentContext).Inflate(Resource.Layout.pairedDeviceAdapter, null, false);
            LinearLayout layout = row.FindViewById<LinearLayout>(Resource.Id.pairedDeviceLayout);
            layout.SetBackgroundColor(Android.Graphics.Color.White);
            layout.SetPadding(10, 0, 10, 0);
            TextView textView = row.FindViewById<TextView>(Resource.Id.pairedDeviceNameTxt);
            if(position == 0)
            {
                textView.SetTextColor(Android.Graphics.Color.ParseColor("#00bd0d"));
                textView.Gravity = GravityFlags.CenterHorizontal;
            }
            else
            {
                textView.SetTextColor(Android.Graphics.Color.ParseColor("#0095a1"));
            }
            textView.SetPadding(0, 15, 0, 15);
            PairedDevice device = BT.PairedDevices[position];

            textView.Text = device.Name;

            return row;
        }
    }
}
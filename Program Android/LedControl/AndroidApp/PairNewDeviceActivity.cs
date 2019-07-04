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
    [Activity(Label = "Paruj nowe urz¹dzenie")]
    public class PairNewDeviceActivity : BaseActivity
    {
        protected override void OnCreate(Bundle savedInstanceState)
        {
            base.OnCreate(savedInstanceState);
            SetContentView(Resource.Layout.AddNew);
            ActiveActivity = CurrentActivity.PAIRDEVICE;
        }

        protected override void OnRestart()
        {
            base.OnRestart();
            ActiveActivity = CurrentActivity.PAIRDEVICE;
        }
    }
}
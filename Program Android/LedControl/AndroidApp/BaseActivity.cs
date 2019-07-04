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
using Java.Util;

namespace AndroidApp
{
    public enum CurrentActivity { MAIN, PAIRDEVICE }
    public class BaseActivity : Activity
    {
        public CurrentActivity ActiveActivity = CurrentActivity.MAIN;
        public Timer ConnectionTimer;

        public BaseActivity()
        {
            
        }

        public void CreateConnectionTimer()
        {
            ConnectionTimer = new Timer();
            ConnectionTimer.Schedule(new TimerClass(this), 0, 50);
        }

        protected override void OnActivityResult(int requestCode, [GeneratedEnum] Result resultCode, Intent data)
        {
            base.OnActivityResult(requestCode, resultCode, data);
            if(requestCode == StaticValues.BluetoothEnable)
            {
                if(resultCode == Result.Ok)
                {
                    BT.SetPairedDevices(this);
                    if(ActiveActivity == CurrentActivity.MAIN)
                    {
                        MainActivity main = (MainActivity)this;
                        main.SetPairedDevicesAdapter();
                    }              
                }
            }
        }
    }

    public class TimerClass : TimerTask
    {
        private BaseActivity Context;
        public TimerClass(BaseActivity activity)
        {
            Context = activity;
            Toast.MakeText(Context, "Po³¹czono.", ToastLength.Short).Show();
        }
        public override void Run()
        {
            Context.RunOnUiThread(() => {
                MainActivity main = (MainActivity)Context;
                if (BT.IsConnected == false)
                {
                    if(BT.UserActionDisconnect == false)
                    {
                        Toast.MakeText(Context, "Roz³¹czono z urz¹dzeniem.", ToastLength.Short).Show();
                    }
                    main.ResetSelectedDevice();
                    Context.ConnectionTimer.Cancel();
                }
                else
                {
                    if(BT.BluetoothMessage != "")
                    {
                        main.SetSettings(BT.BluetoothMessage);
                        BT.BluetoothMessage = "";
                    }
                }
            });
        }
    }
}
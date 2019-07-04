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
using Android.Bluetooth;
using System.IO;
using Java.Util;
using System.Threading.Tasks;
using System.Threading;

namespace AndroidApp
{
    public class BT
    {
        private static BluetoothAdapter BtAdapter;
        public static List<PairedDevice> PairedDevices;

        private static BluetoothDevice Device;
        private static BluetoothSocket Socket;
        private static Stream OutputStream;
        private static Stream InputStream;
        private static UUID Uuid;

        public static bool IsConnected = false;
        public static bool UserActionDisconnect = true;

        public static string BluetoothMessage = "";

        public static void BluetoothInit(BaseActivity activity)
        {
            BtAdapter = BluetoothAdapter.DefaultAdapter;
            Uuid = UUID.FromString("00001101-0000-1000-8000-00805F9B34FB");

            if (BtAdapter != null)
            {
                if (BtAdapter.IsEnabled)
                {
                    SetPairedDevices(activity);
                }
                else
                {
                    TryEnableBluetooth(activity);
                }
            }
            else
            {
                Toast.MakeText(activity, "Brak adaptera bluetooth w telefonie.", ToastLength.Short).Show();
            }
        } 

        public static void SetPairedDevices(BaseActivity activity)
        {
            if(BtAdapter != null)
            {
                if (BtAdapter.IsEnabled)
                {
                    PairedDevices = new List<PairedDevice>();
                    PairedDevices.Add(new PairedDevice("WYBIERZ URZ¥DZENIE"));
                    if(BtAdapter.BondedDevices != null)
                    {
                        foreach(var o in BtAdapter.BondedDevices)
                        {
                            PairedDevices.Add(new PairedDevice(o.Name));
                        }
                        if(activity.ActiveActivity == CurrentActivity.MAIN)
                        {
                            MainActivity main = (MainActivity)activity;
                            main.SetPairedDevicesAdapter();
                        }
                    }
                    if(PairedDevices.Count < 1)
                    {
                        Toast.MakeText(activity, "Brak sparowanych urz¹dzeñ", ToastLength.Short).Show();
                    }
                }
            }
            else
            {
                Toast.MakeText(activity, "Brak adaptera bluetooth w telefonie.", ToastLength.Short).Show();
            }
        }

        private static void TryEnableBluetooth(Activity activity)
        {
            Intent i = new Intent(BluetoothAdapter.ActionRequestEnable);
            activity.StartActivityForResult(i, StaticValues.BluetoothEnable);
        }

        public static Task<bool> TryConnect(BaseActivity activity, int index)
        {
            if(index > 0)
            {
                Toast.MakeText(activity, "£¹czenie z urz¹dzeniem...", ToastLength.Short).Show();
            }
            return Task.Run(async () =>
            {
                CloseConnection(true);
                if (index > 0)
                {
                    foreach (var o in BtAdapter.BondedDevices)
                    {
                        if (o.Name == PairedDevices[index].Name)
                        {
                            Device = o;
                            break;
                        }
                    }
                    try
                    {
                        Socket = Device.CreateRfcommSocketToServiceRecord(Uuid);
                        Socket.Connect();
                        IsConnected = true;
                        OutputStream = Socket.OutputStream;
                        InputStream = Socket.InputStream;
                        //activity.CreateConnectionTimer();
                        Thread thread = new Thread(MessageThread);
                        thread.Start();
                        await BT.SendMessage("i");
                        return true;
                    }
                    catch
                    {
                        CloseConnection(true);
                        return false;
                    }
                }
                return false;
            });     
        }

        public static void CloseConnection(bool userActionDisconnect)
        {
            IsConnected = false;
            if(Socket != null)
            {
                Socket.Close();
            }
            if(OutputStream != null)
            {
                OutputStream.Close();
                OutputStream = null;
            }
            if(InputStream != null)
            {
                InputStream.Close();
                InputStream = null;
            }
            UserActionDisconnect = userActionDisconnect;
            Device = null;
        }

        public static Task SendMessage(string message)
        {
            return Task.Run(() =>
            {
                if(OutputStream != null)
                {
                    byte[] messageBytes = Encoding.UTF8.GetBytes(message);
                    OutputStream.Flush();
                    OutputStream.Write(messageBytes, 0, messageBytes.Length);
                }
            });
        } 

        private static void MessageThread()
        {
            byte[] buffer = new byte[1024];
            int bytes = 0;
            while (true)
            {
                try
                {
                    bytes = InputStream.Read(buffer, 0, buffer.Length);
                    BT.BluetoothMessage = System.Text.Encoding.UTF8.GetString(buffer);
                }
                catch
                {
                    CloseConnection(false);
                    break;
                }
            }
        }
    }
}
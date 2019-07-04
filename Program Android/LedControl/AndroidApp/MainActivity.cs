using System;
using Android.App;
using Android.Content;
using Android.Runtime;
using Android.Views;
using Android.Widget;
using Android.OS;
using Android.Graphics.Drawables;
using Android.Graphics;
using Android.Text;
using Android.Text.Style;

namespace AndroidApp
{
    [Activity(Label = "Led control", MainLauncher = true, Icon = "@drawable/logo")]
    public class MainActivity : BaseActivity
    {
        private Spinner PairedSpinner;
        private PairedDeviceAdapter PairedAdapter;
        private SeekBar PowerSeekBar;
        private TextView PowerTextView;
        private EditText TimeEditText;
        private Button SendBtn;
        private Button ResetBtn;

        protected override void OnCreate(Bundle bundle)
        {
            base.OnCreate(bundle);
            SetContentView(Resource.Layout.Main);
            ActiveActivity = CurrentActivity.MAIN;
            PairedSpinner = this.FindViewById<Spinner>(Resource.Id.pairedDevicesSpinner);
            PairedSpinner.ItemSelected += PairedSpinner_ItemSelected;
            PowerSeekBar = this.FindViewById<SeekBar>(Resource.Id.seekBar1);
            PowerSeekBar.ProgressChanged += PowerSeekBar_ProgressChanged;
            PowerSeekBar.Thumb.SetColorFilter(Color.ParseColor("#00bd0d"), PorterDuff.Mode.SrcIn);
            PowerTextView = this.FindViewById<TextView>(Resource.Id.powerTextView);
            TimeEditText = this.FindViewById<EditText>(Resource.Id.timeEditText);
            SendBtn = this.FindViewById<Button>(Resource.Id.sendBtn);
            SendBtn.Click += SendBtn_Click;
            ResetBtn = this.FindViewById<Button>(Resource.Id.resetBtn);
            ResetBtn.Click += ResetBtn_Click;
            BT.BluetoothInit(this);
        }

        public override bool OnCreateOptionsMenu(IMenu menu)
        {
            //MenuInflater.Inflate(Resource.Menu.topMenu, menu);
            return base.OnCreateOptionsMenu(menu);
        }

        protected override void OnRestart()
        {
            base.OnRestart();
            ActiveActivity = CurrentActivity.MAIN;
            SetPairedDevicesAdapter();
        }

        public override bool OnOptionsItemSelected(IMenuItem item)
        {
            if (BT.IsConnected)
            {
                BT.CloseConnection(true);
            }
            if(item.ItemId == Resource.Id.menu_addDevice)
            {
                Intent intent = new Intent(this, typeof(PairNewDeviceActivity));
                StartActivity(intent);
            }
            return base.OnOptionsItemSelected(item);
        }

        public void SetPairedDevicesAdapter()
        {
            if(PairedAdapter == null)
            {
                PairedAdapter = new PairedDeviceAdapter(this);
                PairedSpinner.Adapter = PairedAdapter;
            }
            else
            {
                PairedAdapter.NotifyDataSetChanged();
            }
        }

        public void ResetSelectedDevice()
        {
            PairedSpinner.SetSelection(0);
        }

        public void SetSettings(string settings)
        {
            string[] values = settings.Split(';');
            TimeEditText.Text = values[0];
            PowerSeekBar.Progress = Convert.ToInt32(values[1]);
            PowerTextView.Text = values[1];
        }

        private async void PairedSpinner_ItemSelected(object sender, AdapterView.ItemSelectedEventArgs e)
        {
            PairedSpinner.Enabled = false;
            bool result = await BT.TryConnect(this, e.Position);
            if (e.Position > 0)
            {
                if (result == true)
                {
                    CreateConnectionTimer();
                }
                else
                {
                    Toast.MakeText(this, "Błąd podczas łączenia z urządzeniem", ToastLength.Short).Show();
                }
            }
            PairedSpinner.Enabled = true;
        }

        private void PowerSeekBar_ProgressChanged(object sender, SeekBar.ProgressChangedEventArgs e)
        {
            PowerTextView.Text = e.Progress.ToString();
        }

        private async void ResetBtn_Click(object sender, EventArgs e)
        {
            if(BT.IsConnected == true)
            {
                await BT.SendMessage("r");
            }
            else
            {
                Toast.MakeText(this, "Brak połączenia z urządzeniem.", ToastLength.Short).Show();
            }
        }

        private async void SendBtn_Click(object sender, EventArgs e)
        {
            if (BT.IsConnected == true)
            {
                int time = -1;
                try
                {
                    time = Convert.ToInt32(TimeEditText.Text);
                }
                catch
                {

                }
                if(time > -1 && time < 601)
                {
                    string messageValue = "t" + time.ToString() + "p" + PowerSeekBar.Progress.ToString();
                    await BT.SendMessage(messageValue);
                }
                else
                {
                    Toast.MakeText(this, "Wartość CZAS musi być w przedziale 0 - 600s", ToastLength.Short).Show();
                }
            }
            else
            {
                Toast.MakeText(this, "Brak połączenia z urządzeniem.", ToastLength.Short).Show();
            }
        }
    }
}


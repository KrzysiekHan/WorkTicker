package md530df750621508882e387b3020b704535;


public class PairNewDeviceActivity
	extends md530df750621508882e387b3020b704535.BaseActivity
	implements
		mono.android.IGCUserPeer
{
/** @hide */
	public static final String __md_methods;
	static {
		__md_methods = 
			"n_onCreate:(Landroid/os/Bundle;)V:GetOnCreate_Landroid_os_Bundle_Handler\n" +
			"n_onRestart:()V:GetOnRestartHandler\n" +
			"";
		mono.android.Runtime.register ("AndroidApp.PairNewDeviceActivity, AndroidApp, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null", PairNewDeviceActivity.class, __md_methods);
	}


	public PairNewDeviceActivity () throws java.lang.Throwable
	{
		super ();
		if (getClass () == PairNewDeviceActivity.class)
			mono.android.TypeManager.Activate ("AndroidApp.PairNewDeviceActivity, AndroidApp, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null", "", this, new java.lang.Object[] {  });
	}


	public void onCreate (android.os.Bundle p0)
	{
		n_onCreate (p0);
	}

	private native void n_onCreate (android.os.Bundle p0);


	public void onRestart ()
	{
		n_onRestart ();
	}

	private native void n_onRestart ();

	private java.util.ArrayList refList;
	public void monodroidAddReference (java.lang.Object obj)
	{
		if (refList == null)
			refList = new java.util.ArrayList ();
		refList.add (obj);
	}

	public void monodroidClearReferences ()
	{
		if (refList != null)
			refList.clear ();
	}
}

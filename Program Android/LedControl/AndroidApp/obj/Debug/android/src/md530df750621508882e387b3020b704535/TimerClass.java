package md530df750621508882e387b3020b704535;


public class TimerClass
	extends java.util.TimerTask
	implements
		mono.android.IGCUserPeer
{
/** @hide */
	public static final String __md_methods;
	static {
		__md_methods = 
			"n_run:()V:GetRunHandler\n" +
			"";
		mono.android.Runtime.register ("AndroidApp.TimerClass, AndroidApp, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null", TimerClass.class, __md_methods);
	}


	public TimerClass () throws java.lang.Throwable
	{
		super ();
		if (getClass () == TimerClass.class)
			mono.android.TypeManager.Activate ("AndroidApp.TimerClass, AndroidApp, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null", "", this, new java.lang.Object[] {  });
	}

	public TimerClass (md530df750621508882e387b3020b704535.BaseActivity p0) throws java.lang.Throwable
	{
		super ();
		if (getClass () == TimerClass.class)
			mono.android.TypeManager.Activate ("AndroidApp.TimerClass, AndroidApp, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null", "AndroidApp.BaseActivity, AndroidApp, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null", this, new java.lang.Object[] { p0 });
	}


	public void run ()
	{
		n_run ();
	}

	private native void n_run ();

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

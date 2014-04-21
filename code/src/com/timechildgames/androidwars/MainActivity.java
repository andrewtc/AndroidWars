package com.timechildgames.androidwars;

import android.app.NativeActivity;
import android.content.Context;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Bundle;
import android.util.Log;
import android.view.inputmethod.InputMethodManager;

public class MainActivity extends NativeActivity
{
	protected static final String TAG = "AndroidWars";
	
	
	@Override
	public synchronized void onCreate( Bundle savedInstanceState )
	{
		super.onCreate( savedInstanceState );

		// Create the online game client.
		onlineGameClient = new OnlineGameClient( this );
		Log.d( TAG, "Created Java OnlineGameClient instance!" );
	}
	
	
	public synchronized OnlineGameClient getOnlineGameClient()
	{
		return onlineGameClient;
	}
	
	
	public boolean networkIsAvailable()
	{
		ConnectivityManager connectivityManager = (ConnectivityManager) getSystemService( Context.CONNECTIVITY_SERVICE );
		NetworkInfo networkInfo = connectivityManager.getActiveNetworkInfo();
		
		// Return true if this device can connect to the Internet and has a wireless connection.
		return ( networkInfo != null && networkInfo.isConnected() );
	}
	
	
	public void toggleKeyboard( boolean visible )
	{
		// Get the input manager.
		InputMethodManager manager = (InputMethodManager) getSystemService( INPUT_METHOD_SERVICE );
		
		if( visible )
		{
			// If the keyboard should be visible, show it.
			manager.toggleSoftInput( InputMethodManager.SHOW_FORCED, 0 );
		}
		else
		{
			// Otherwise, hide the keyboard.
			manager.toggleSoftInput( 0, 0 );
		}
	}
	
	
	protected OnlineGameClient onlineGameClient;
}

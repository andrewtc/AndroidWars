package com.timechildgames.androidwars;

import android.app.NativeActivity;
import android.content.Context;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Bundle;

public class MainActivity extends NativeActivity
{
	protected static final String TAG = "AndroidWars";
	
	
	@Override
	public void onCreate( Bundle savedInstanceState )
	{
		super.onCreate( savedInstanceState );
		
		// Create online game service.
		onlineGameService = new OnlineGameService( this );
		onlineGameService.init();
	}
	
	
	public OnlineGameService getOnlineGameService()
	{
		return onlineGameService;
	}
	
	
	public boolean networkIsAvailable()
	{
		ConnectivityManager connectivityManager = (ConnectivityManager) getSystemService( Context.CONNECTIVITY_SERVICE );
		NetworkInfo networkInfo = connectivityManager.getActiveNetworkInfo();
		
		return ( networkInfo != null && networkInfo.isConnected() );
	}
	
	
	protected OnlineGameService onlineGameService;
}

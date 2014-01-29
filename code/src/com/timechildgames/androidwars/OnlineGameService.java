package com.timechildgames.androidwars;

import java.lang.String;
import java.util.Deque;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;

import org.json.JSONException;
import org.json.JSONObject;

import android.util.Log;

import com.parse.*;


public class OnlineGameService
{
	protected static final String APPLICATION_ID = "PLhLDmLfRx1RB84ha7SUCryCkH5bkND4VxM1OFlq";
	protected static final String CLIENT_KEY     = "RxIDaLzwmU5IEMznpX39BauYUsxmf8HnBIJIPtwa";
	
	
	public OnlineGameService( MainActivity activity )
	{
		this.activity = activity;
		isInitialized = false;
		lastRequestID = -1;
		
		// Initialize the list of requests.
		requestResults = new LinkedList< OnlineRequestResult >();
	}
	
	
	public void init()
	{
		if( !isInitialized )
		{
			// Initialize Parse.
			Parse.initialize( activity, APPLICATION_ID, CLIENT_KEY );
			
			isInitialized = true;
		}
	}
	
	
	public int startRequest( String functionName, String parametersJSON )
	{
		// Choose a new request ID for this request.
		final int requestID = reserveNextRequestID();
		
		try
		{
			// Unpack the parameters.
			JSONObject parametersObject = new JSONObject( parametersJSON );
			
			// Reformat the parameters in a HashMap.
			HashMap< String, Object > parameters = convertToHashMap( parametersObject );
		
			ParseCloud.callFunctionInBackground( functionName, parameters, new FunctionCallback< String >()
			{
				public void done( String result, ParseException exception )
				{
					// Make sure no error occurred.
					if( exception == null )
					{
						// Save the result of the cloud function call.
						pushResult( requestID, result );
					}
					else
					{
						// If a Parse error occurred, report the error.
						pushError( requestID, exception.toString() );
					}
				}
			});
		}
		catch( JSONException exception )
		{
			// If a JSON conversion error occurred, report the error.
			pushError( requestID, exception.toString() );
		}
		
		return requestID;
	}
	
	
	public OnlineRequestResult fetchRequestResult()
	{
		OnlineRequestResult result = null;
		
		if( !requestResults.isEmpty() )
		{
			result = requestResults.pop();
		}
		
		return result;
	}
	
	
	protected int reserveNextRequestID()
	{
		++lastRequestID;
		return lastRequestID;
	}
	
	
	protected HashMap< String, Object > convertToHashMap( JSONObject object ) throws JSONException
	{
		HashMap< String, Object > map = new HashMap< String, Object >();
		Iterator< String > keys = object.keys();
		
		while( keys.hasNext() )
		{
			String key = keys.next();
			map.put( key, object.getString( key ) );
		}
		
		return map;
	}
	
	
	protected void pushResult( int requestID, String result )
	{
		requestResults.addLast( new OnlineRequestResult( requestID, result, false, "" ) );
	}
	
	
	protected void pushError( int requestID, String errorMessage )
	{
		Log.d( activity.TAG, errorMessage );
		requestResults.addLast( new OnlineRequestResult( requestID, "", true, errorMessage ) );
	}
	
	
	protected MainActivity activity;
	protected boolean isInitialized;
	protected int lastRequestID;
	protected Deque< OnlineRequestResult > requestResults;
}

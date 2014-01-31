package com.timechildgames.androidwars;

import java.io.UnsupportedEncodingException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.Deque;
import java.util.LinkedList;

import org.apache.http.Header;
import org.apache.http.entity.StringEntity;
import org.apache.http.message.BasicHeader;

import android.util.Log;
import com.loopj.android.http.*;


/**
 * Manages sending requests to and receiving responses from the AndroidWarsOnline service.
 */
public class OnlineGameClient
{
	public static final String UNKNOWN_ERROR_MESSAGE = "An unknown error occurred.";
	
	public static final String PARSE_REST_URL = "https://www.parse.com/1/";
	public static final String PARSE_FUNCTION_PREFIX = "functions/";
	
	public static final String ANDROIDWARS_APPLICATION_ID = "PLhLDmLfRx1RB84ha7SUCryCkH5bkND4VxM1OFlq";
	public static final String ANDROIDWARS_REST_API_KEY   = "ly951AFZEjjdf8O5ZIjOo4qCRKFqdt0jvoWzuQMs";
	
	private static AsyncHttpClient client = new AsyncHttpClient();
	
	
	public OnlineGameClient( MainActivity activity )
	{
		this.activity = activity;
		this.responses = new LinkedList< OnlineRequestResponse >();
	}
	
	
	public long callCloudFunction( String functionName, String params )
	{
		// Reserve the next request ID.
		final long requestID = reserveRequestID();
		
		try
		{
			// Format the request URL.
			URL url = formatFunctionURL( functionName );
			
			// Build necessary Parse headers for the request.
			Header[] headers = 
			{
				new BasicHeader( "X-Parse-Application-ID", ANDROIDWARS_APPLICATION_ID ),
				new BasicHeader( "X-Parse-REST-API-Key", ANDROIDWARS_REST_API_KEY )
			};
			
			// Format POST data.
			StringEntity data = new StringEntity( params );
			
			// Initiate the request.
			client.post( activity, url.toString(), headers, data, "application/json", new TextHttpResponseHandler()
			{
				@Override
				public void onSuccess( int statusCode, Header[] headers, String response )
				{
					synchronized( responses )
					{
						// Queue up the successful response.
						responses.addLast( OnlineRequestResponse.createSuccessfulResponse( requestID, statusCode, response ) );
					}
				}
				
				@Override
				public void onFailure( int statusCode, Header[] headers, String response, Throwable exception )
				{
					String errorMessage = UNKNOWN_ERROR_MESSAGE;
					
					if( exception != null )
					{
						// If an exception occurred, return the exception message.
						errorMessage = exception.toString();
					}
					else if( response.length() > 0 )
					{
						// Otherwise, return the response as the error message.
						errorMessage = response;
					}

					synchronized( responses )
					{
						// Queue up the error response.
						responses.addLast( OnlineRequestResponse.createErrorResponse( requestID, statusCode, errorMessage ) );
					}
				}
			});
		}
		catch( MalformedURLException exception )
		{
			// Print the exception.
			Log.d( MainActivity.TAG, exception.toString() );
		}
		catch( UnsupportedEncodingException exception )
		{
			// Print the exception.
			Log.d( MainActivity.TAG, exception.toString() );
		}
		
		// Return the ID of the request.
		return requestID;
	}
	
	
	public static URL formatFunctionURL( String functionName ) throws MalformedURLException
	{
		return new URL( PARSE_REST_URL + PARSE_FUNCTION_PREFIX + functionName );
	}
	
	
	public OnlineRequestResponse fetchNextResponse()
	{
		OnlineRequestResponse response = null;
		
		synchronized( responses )
		{
			if( !responses.isEmpty() )
			{
				// If there is a response on the responses list, pop the next response and return it.
				response = responses.pop();
			}
		}
		
		return response;
	}
	
	
	private long reserveRequestID()
	{
		// Return the next available requestID.
		return nextRequestID++;
	}
	
	
	private MainActivity activity;
	private long nextRequestID = 0;
	private Deque< OnlineRequestResponse > responses;
}

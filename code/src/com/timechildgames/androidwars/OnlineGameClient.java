package com.timechildgames.androidwars;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
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
	
	private MainActivity activity;
	private long nextRequestID = 0;
	private Deque< OnlineRequestResponse > responses;
	
	
	public OnlineGameClient( MainActivity activity )
	{
		this.activity = activity;
		this.responses = new LinkedList< OnlineRequestResponse >();
	}
	
	
	public static RequestParams createRequestParams()
	{
		return new RequestParams();
	}
	
	
	private Header[] createRequestHeaders( String sessionToken )
	{
		// Build necessary Parse headers for the request.
		ArrayList< Header > headers = new ArrayList< Header >();
		
		headers.add( new BasicHeader( "X-Parse-Application-ID", ANDROIDWARS_APPLICATION_ID ) );
		headers.add( new BasicHeader( "X-Parse-REST-API-Key", ANDROIDWARS_REST_API_KEY ) );
		
		if( !sessionToken.isEmpty() )
		{
			// Add the user session header if the request is authenticated.
			headers.add( new BasicHeader( "X-Parse-Session-Token", sessionToken ) );
		}
		
		return headers.toArray( new Header[] { } );
	}
	
	
	private String headersToString( Header[] headers )
	{
		String headersString = "";
		
		for( Header header : headers )
		{
			headersString += header.toString() + "; ";
		}
		
		return headersString;
	}
	
	
	/**
	 * Sends an HTTP GET request to the AndroidWarsOnline service.
	 * @param resource The path (i.e. URL fragment) of the resource to access.
	 * @param parameters An object containing the parameters to send with the request.
	 * @return
	 */
	public long sendGetRequest( String resource, RequestParams parameters, String sessionToken )
	{
		// Reserve the next request ID.
		final long requestID = reserveRequestID();
		
		try
		{
			// Format the request URL.
			URL url = formatRequestURL( resource );
			
			// Build necessary headers for the request.
			Header[] headers = createRequestHeaders( sessionToken );
			
			// Create a new response handler for the request callbacks.
			ResponseHandler responseHandler = new ResponseHandler( requestID );

			// Initiate the request.
			Log.d( MainActivity.TAG, "Sending GET request to \"" + url.toString() + "\" with parameters \"" + parameters + "\" and " + headers.length + " headers (\"" + headersToString( headers ) + "\")" );
			client.get( activity, url.toString(), headers, parameters, responseHandler );
		}
		catch( Exception exception )
		{
			// Print the exception.
			Log.d( MainActivity.TAG, exception.toString() );
		}
		
		// Return the ID of the request.
		return requestID;
	}
	
	
	/**
	 * Sends an HTTP POST request to the AndroidWarsOnline service.
	 * @param resource The path (i.e. URL fragment) of the resource to access.
	 * @param data The JSON data to send with the request.
	 * @return The request ID of the request that was initiated.
	 */
	public long sendPostRequest( String resource, String data, String sessionToken )
	{
		// Reserve the next request ID.
		final long requestID = reserveRequestID();
		
		try
		{
			// Format the request URL.
			URL url = formatRequestURL( resource );
			
			// Build necessary headers for the request.
			Header[] headers = createRequestHeaders( sessionToken );
			
			// Format POST data.
			StringEntity dataEntity = new StringEntity( data );
			
			// Create a new response handler for the request callbacks.
			ResponseHandler responseHandler = new ResponseHandler( requestID );

			// Initiate the request.
			Log.d( MainActivity.TAG, "Sending POST request to \"" + url.toString() + "\" with data \"" + data + "\" and " + headers.length + " headers (\"" + headersToString( headers ) + "\")" );
			client.post( activity, url.toString(), headers, dataEntity, "application/json", responseHandler );
		}
		catch( Exception exception )
		{
			// Print the exception.
			Log.d( MainActivity.TAG, exception.toString() );
		}
		
		// Return the ID of the request.
		return requestID;
	}
	
	
	class ResponseHandler extends TextHttpResponseHandler
	{
		private long requestID;
		
		
		public ResponseHandler( long requestID )
		{
			this.requestID = requestID;
		}
		
		
		@Override
		public void onSuccess( int statusCode, Header[] headers, String response )
		{
			Log.d( MainActivity.TAG, "Received successful response for request " + requestID + ": \"" + response + "\"" );
			synchronized( responses )
			{
				// Queue up the successful response.
				responses.addLast( OnlineRequestResponse.createSuccessfulResponse( requestID, statusCode, response ) );
			}
		}
		
		@Override
		public void onFailure( int statusCode, Header[] headers, String response, Throwable exception )
		{
			Log.d( MainActivity.TAG, "Received failed response for request " + requestID + ": \"" + response + "\"" );
			String errorMessage = UNKNOWN_ERROR_MESSAGE;
			
			if( response.length() > 0 )
			{
				// Otherwise, return the response as the error message.
				errorMessage = response;
			}
			else if( exception != null )
			{
				// If an exception occurred, return the exception message.
				errorMessage = exception.toString();
			}

			synchronized( responses )
			{
				// Queue up the error response.
				responses.addLast( OnlineRequestResponse.createErrorResponse( requestID, statusCode, errorMessage ) );
			}
		}
	}
	
	
	public static URL formatRequestURL( String resource ) throws MalformedURLException
	{
		return new URL( PARSE_REST_URL + resource );
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
}

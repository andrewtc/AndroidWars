package com.timechildgames.androidwars;

public class OnlineRequestResponse
{
	public static final int STATUS_CODE_SUCCESSFUL = 200;
	
	
	public static OnlineRequestResponse createSuccessfulResponse( long requestID, int statusCode, String result )
	{
		return new OnlineRequestResponse( requestID, statusCode, result );
	}
	
	
	public static OnlineRequestResponse createErrorResponse( long requestID, int statusCode, String errorMessage )
	{
		return new OnlineRequestResponse( requestID, statusCode, errorMessage );
	}
	
	
	public OnlineRequestResponse( long requestID, int statusCode, String result )
	{
		this.requestID = requestID;
		this.statusCode = statusCode;
		this.result = result;
	}
	
	
	public long getRequestID()
	{
		return requestID;
	}
	
	
	public int getStatusCode()
	{
		return statusCode;
	}
	
	
	public String getResult()
	{
		return result;
	}
	
	
	public boolean isError()
	{
		return ( statusCode != STATUS_CODE_SUCCESSFUL );
	}
	
	
	private long requestID;
	private int statusCode;
	private String result;
}

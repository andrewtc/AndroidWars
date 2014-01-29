package com.timechildgames.androidwars;

import java.lang.String;


class OnlineRequestResult
{
	public OnlineRequestResult( int requestID, String result, boolean hasError, String errorMessage )
	{
		this.requestID = requestID;
		this.result = result;
		this.hasError = hasError;
		this.errorMessage = "";
	}
	
	
	public int getRequestID()
	{
		return requestID;
	}
	
	
	public String getResult()
	{
		return result;
	}
	
	
	public boolean hasError()
	{
		return hasError;
	}
	
	
	public String getErrorMessage()
	{
		return errorMessage;
	}
	
	
	protected int requestID;
	protected String result;
	protected boolean hasError;
	protected String errorMessage;
}

// Use Parse.Cloud.define to define as many cloud functions as you want.
// For example:
Parse.Cloud.define( "hello", function( request, response )
{
    var name = request.params[ "name" ];
    
    if( typeof name == 'undefined' || name.length == 0 )
    {
        name = "World";
    }

    response.success( "Hello, " + name + "!" );
});


Parse.Cloud.define( "requestMatchmakingGame", function( request, response )
{
    response.success( "Hello, World!" );
});
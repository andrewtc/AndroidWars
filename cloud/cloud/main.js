var Player = Parse.Object.extend( "Player" );
var GameRequest = Parse.Object.extend( "GameRequest" );


function queryPlayerForUser( user, options )
{
    // Look up the Player for this User.
    var query = new Parse.Query( Player ).equalTo( "user", user );
    query.first( options );
}


Parse.Cloud.define( "hello", function( request, response )
{
    var name = request.params[ "name" ];
    
    if( typeof name == 'undefined' || name.length == 0 )
    {
        response.error( "No 'name' parameter was provided." );
    }
    else
    {
        response.success( "Hello, " + name + "!" );
    }
});


Parse.Cloud.define( "requestMatchmakingGame", function( request, response )
{
    if( request.user )
    {
        // Look up the Player for this User.
        queryPlayerForUser( request.user,
        {
            success: function( player )
            {
                // If the user is logged in, create a new matchmaking game request.
                var gameRequest = new GameRequest();
                gameRequest.set( "player", player );
                
                gameRequest.save( null,
                {
                    success: function( gameRequest )
                    {
                        response.success( "Created new game request for " + player.get( "name" ) + "." );
                    },
                    error: function( gameRequest, error )
                    {
                        response.error( "Could not create GameRequest: " + error.description );
                    }
                });
            },
            error: function( error )
            {
                // If no Player was found, error.
                response.error( "No Player found for User \"" + request.user.getUsername() + "\"!" );
            }
        });
    }
    else
    {
        // If the user isn't logged in, return an error.
        response.error( "You must be logged in to access this function." );
    }
});
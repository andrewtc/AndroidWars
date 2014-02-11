var Matchmaking =
{
    MIN_PLAYERS: 2,
    MAX_PLAYERS: 4
}


var Player = Parse.Object.extend( "Player" );
var Game = Parse.Object.extend( "Game" );
var GamePlayer = Parse.Object.extend( "GamePlayer" );
var GameRequest = Parse.Object.extend( "GameRequest" );


// ========== COMMON FUNCTIONALITY ===========

/**
 * Errors if the request does not have a current User. Otherwise, calls the success
 * callback with the current user as the first parameter.
 */
function assertCurrentUser( request, response, onSuccess )
{
    if( response )
    {
        if( request )
        {
            if( request.user )
            {
                if( onSuccess )
                {
                    // Call the success callback.
                    onSuccess( request.user );
                }
            }
            else
            {
                response.error( "You must be logged in to call this function!" );
            }
        }
        else
        {
            response.error( "No request supplied to assertUser() function!" );
        }
    }
    else
    {
        console.log( "No response supplied to assertUser() function!" );
    }
}


/**
 * Errors if the request does not have a current User or if the current User does not
 * have an associated Player record. Otherwise, calls the success callback with the
 * current User in the first parameter and the current Player in the second.
 */
function assertCurrentPlayer( request, response, onSuccess )
{
    assertCurrentUser( request, response, function( user )
    {
        // Look up the Player for this User.
        var findPlayer = new Parse.Query( Player )
            .equalTo( "user", user );
            
        findPlayer.first(
        {
            success: function( player )
            {
                if( player )
                {
                    if( onSuccess )
                    {
                        // Call the success callback.
                        onSuccess( user, player );
                    }
                }
                else
                {
                    response.error( "Could not find Player for User \"" + user.getUsername() + "\"!" );
                }
            },
            error: function( error )
            {
                response.error( "Error looking up current Player for User \"" + user.getUsername() + "\": " + error );
            }
        });
    });
}


// ========== CLOUD FUNCTIONS ==========

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
    assertCurrentPlayer( request, response, function( user, player )
    {
        // Check to see if there is already a GameRequest for this Player.
        var findExistingRequest = new Parse.Query( GameRequest )
            .equalTo( "player", player );
        
        findExistingRequest.first(
        {
            success: function( existingRequest )
            {
                if( !existingRequest )
                {
                    // Create a new matchmaking game request.
                    var gameRequest = new GameRequest();
                    gameRequest.set( "player", player );
                    
                    gameRequest.save( null,
                    {
                        success: function( gameRequest )
                        {
                            response.success( "Created new game request for Player \"" + player.get( "name" ) + "\"." );
                        },
                        error: function( gameRequest, error )
                        {
                            response.error( "Could not create GameRequest: " + error );
                        }
                    });
                }
                else
                {
                    response.error( "There is already a pending GameRequest for Player \"" + player.get( "name" ) + "\"." );
                }
            },
            error: function( error )
            {
                response.error( "Error finding existing GameRequest for Player \"" + player.get( "name" ) + "\": " + error );
            }
        });
    });
});


Parse.Cloud.define( "getCurrentGameList", function( request, response )
{
    assertCurrentPlayer( request, response, function( user, player )
    {
        // Retrieve the list of games for the current player.
        var findGamesForPlayer = new Parse.Query( GamePlayer )
            .equalTo( "player", player )
            .include( "game" );
            
        findGamesForPlayer.find(
        {
            success: function( list )
            {
                var games = [];
                
                for( var i = 0; i < list.length; i++ )
                {
                    // Build the list of game data to send back to the client.
                    var game = list[ i ].get( "game" );
                    /*games.push(
                    {
                        id:   game.id,
                        name: game.name
                    });*/
                    games.push( game );
                }
                
                // Return the list of current games for the current Player.
                response.success( games );
            },
            error: function( error )
            {
                response.error( "Error retrieving current game list for player \"" + player.get( "name" ) + "\": " + error.message );
            }
        });
    });
});


// ========== BACKGROUND JOBS ==========

Parse.Cloud.job( "createMatchmakingGame", function( request, status )
{
    // Query the list of all GameRequests.
    var findGamePlayers = new Parse.Query( GameRequest )
        .ascending( "createdAt" )
        .limit( Matchmaking.MIN_PLAYERS )
        .include( "player" );
        
    findGamePlayers.find(
    {
        success: function( gameRequests )
        {
            var playerCount = gameRequests.length;
        
            if( playerCount >= Matchmaking.MIN_PLAYERS )
            {
                // If there are enough game requests, create a new game.
                var game = new Game();
                var gameName = "";
                var objectsToSave = [ game ];
                
                for( var i = 0; i < playerCount; i++ )
                {
                    var gameRequest = gameRequests[ i ];
                    var player = gameRequest.get( "player" );
                    
                    // Build game name.
                    gameName += ( i > 0 ? " vs. " : "" ) + player.get( "name" );
                
                    // Create a new record for each Player in the Game.
                    var gamePlayer = new GamePlayer();
                    
                    gamePlayer.set( "game", game );
                    gamePlayer.set( "player", player );
                    
                    objectsToSave.push( gamePlayer );
                }
                
                // Set the name of the Game.
                game.set( "name", gameName );
                
                Parse.Object.saveAll( objectsToSave,
                {
                    success: function( list )
                    {
                        // Destroy the GameRequests used to create the Game.
                        Parse.Object.destroyAll( gameRequests,
                        {
                            success: function( list )
                            {
                                status.success( "Created Game \"" + gameName + "\" with " + playerCount + " players." );
                            },
                            error: function( error )
                            {
                                status.error( "Error destroying GameRequests: " + error );
                            }
                        });
                    },
                    error: function( error )
                    {
                        status.error( "Could not create Game from GameRequests: " + error );
                    }
                });
            }
            else
            {
                status.success( "Not enough GameRequests to create a Game. (Found " + playerCount + ", needed " + Matchmaking.MIN_PLAYERS + ".)" );
            }
        },
        error: function( error )
        {
            status.error( "Could not find GameRequests: " + error );
        }
    });
});

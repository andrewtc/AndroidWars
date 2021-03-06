var Matchmaking =
{
    MIN_PLAYERS: 2,
    MAX_PLAYERS: 4
}


var Player = Parse.Object.extend( "Player" );
var Game = Parse.Object.extend( "Game" );
var GamePlayer = Parse.Object.extend( "GamePlayer" );
var GameRequest = Parse.Object.extend( "GameRequest" );
var Turn = Parse.Object.extend( "Turn" );


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
                    //console.log( JSON.stringify( game ) );
                    games.push(
                    {
                        id:   game.id,
                        name: game.get( "name" )
                    });
                    //games.push( game );
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


Parse.Cloud.define( "getGame", function( request, response )
{
    assertCurrentPlayer( request, response, function( user, player )
    {
        // Get the ID from the request.
        var gameID = request.params[ "id" ];
        
        if( gameID )
        {
            // Convert the Game ID to a pointer.
            var gamePointer = new Game();
            gamePointer.id = gameID;
        
            // Get the requested game by its ID.
            var findGame = new Parse.Query( GamePlayer )
                .equalTo( "player", player )
                .equalTo( "game", gamePointer )
                .include( "game" );
                
            findGame.first(
            {
                success: function( gamePlayer )
                {
                    if( gamePlayer )
                    {
                        // Return the game.
                        var game = gamePlayer.get( "game" );
                        
                        // Get the Map.
                        var map = game.get( "map" );
                        map.fetch(
                        {
                            success: function( map )
                            {
                                // Get current turn.
                                var findCurrentTurn = new Parse.Query( Turn )
                                    .equalTo( "game", game )
                                    .descending( "number" );
                            
                                findCurrentTurn.first(
                                {
                                    success: function( currentTurn )
                                    {
                                        // Create the game data to return.
                                        var gameData = 
                                        {
                                            id:      game.id,
                                            name:    game.get( "name" ),
                                            map:     map.get( "name" ),
                                            turn:    0
                                        };
                                        
                                        if( currentTurn )
                                        {
                                            // If a current Turn record exists, get its number.
                                            gameData.turn = currentTurn.get( "number" );
                                            
                                            // Return the current turn state as part of the data.
                                            gameData.currentState = currentTurn.get( "currentState" );
                                        }
                                        
                                        // Return the game data.
                                        response.success( gameData );
                                    },
                                    error: function( error )
                                    {
                                        response.error( "Error retrieving current turn for game \"" + gameID + "\": " + error.message );
                                    }
                                });
                            },
                            error: function( error )
                            {
                                response.error( "Invalid Map for Game!" );
                            }
                        });
                    }
                    else
                    {
                        response.error( "You do not have access to this Game. playerID=" + player.id + ", gameID=" + gameID );
                    }
                },
                error: function( error )
                {
                    response.error( "Error retrieving game \"" + gameID + "\": " + error.message );
                }
            });
        }
        else
        {
            response.error( "No game ID supplied!" );
        }
    });
});


Parse.Cloud.define( "postTurn", function( request, response )
{
    assertCurrentPlayer( request, response, function( user, player )
    {
        // Get the ID from the request.
        var gameID = request.params[ "id" ];
        
        if( gameID )
        {
            // Convert the Game ID to a pointer.
            var gamePointer = new Game();
            gamePointer.id = gameID;
        
            // Get the requested game by its ID.
            var findGame = new Parse.Query( GamePlayer )
                .equalTo( "player", player )
                .equalTo( "game", gamePointer )
                .include( "game" );
                
            findGame.first(
            {
                success: function( gamePlayer )
                {
                    if( gamePlayer )
                    {
                        // Get the game.
                        var game = gamePlayer.get( "game" );
                        
                        // Get the game state.
                        var gameState = request.params;
                        
                        if( gameState )
                        {
                            // Get current turn.
                            var findCurrentTurn = new Parse.Query( Turn )
                                .equalTo( "game", game )
                                .descending( "number" );
                        
                            findCurrentTurn.first(
                            {
                                success: function( currentTurn )
                                {
                                    var turnNumber = 0;
                                    
                                    if( currentTurn )
                                    {
                                        // If a current Turn record exists, get its number.
                                        turnNumber = currentTurn.get( "number" );
                                    }
                                    
                                    // Increment the turn counter.
                                    turnNumber++;
                            
                                    // Create a new Turn.
                                    var turn = new Turn();
                                    
                                    turn.set( "game", game );
                                    turn.set( "gamePlayer", gamePlayer );
                                    turn.set( "number", turnNumber );
                                    turn.set( "currentState", gameState );
                                    
                                    turn.save(
                                    {
                                        success: function( turn )
                                        {
                                            // Success!
                                            response.success( "Created Turn " + turnNumber + "! playerID=" + player.id + ", gameID=" + gameID + ", turnID=" + turn.id );
                                        },
                                        error: function( turn, error )
                                        {
                                            response.error( "Error saving Turn: " + error.message );
                                        }
                                    });
                                },
                                error: function( error )
                                {
                                    response.error( "Error retrieving current turn for game \"" + gameID + "\": " + error.message );
                                }
                            });
                        }
                        else
                        {
                            response.error( "No game state supplied for turn!. playerID=" + player.id + ", gameID=" + gameID );
                        }
                    }
                    else
                    {
                        response.error( "You do not have access to this Game. playerID=" + player.id + ", gameID=" + gameID );
                    }
                },
                error: function( error )
                {
                    response.error( "Error posting turn for game \"" + gameID + "\": " + error.message );
                }
            });
        }
        else
        {
            response.error( "No game ID supplied!" );
        }
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

This is a quickly put together document intended for people who
would like to run one or more dedicated Quadra servers.

# Command line options #

Here's a list of command line options related to the running of a Quadra dedicated server:

  * `-dedicated`

> Tells quadra to run as a dedicated server. By default, this will create a free-for-all server that will only terminate when the task is killed.

  * `-admin <x>`

> Set the remote administration password to x. Remote administration is disabled unless this option is specified. The usage of the remote administration feature is explained in the next section.

> Note to Linux server administrators: if you use this option as is, all users on your system will be able to see the password by issuing the "ps" command. A good workaround is to place the -admin option (or a "/setpasswd" command) in a file and then start Quadra with the "-exec 

&lt;file&gt;

" option.

  * `-public`

> Make the game public, so that it shows up in QSnoop (or within the game when the user clicks "TCP/IP Internet") all over the internet. Without this option, clients must know the internet address of the server to connect.

  * `-port <x>`

> Listen to port x instead of what is in the config.  When running multiple servers, a different port must be specified for each.

  * `-endfrag <x>`

> The game will end and declare a winner when a team reaches x frags. The server will then auto-restart a new game with the same parameters.

  * `-endtime <x>`

> The game will end and declare a winner after x minutes. The server will then auto-restart a new game with the same parameters.

  * `-endpoints <x>`

> The game will end and declare a winner when a team scores x points. The server will then auto-restart a new game with the same parameters.

  * `-endlines <x>`

> The game will end and declare a winner when a team clears x lines. The server will then auto-restart a new game with the same parameters.

  * `-once`

> Prevent server from restarting after the game's end condition has been reached. The server will automatically shut down instead.

  * `-name <x>`

> Set game name to x.

  * `-ffa`

> Run the game in free-for-all mode (default).

  * `-survivor`

> Run the game in survivor mode.

  * `-peace`

> Run the game in peace mode (no attacks).

  * `-blind <x>`

> Run the game in blind mode. When attacked by their opponents, the players will have some blind blocks to place. The blocks will stay invisible for some x time (default is 30, which is about 5 seconds).

  * `-fullblind <x>`

> Another type of blind mode where the whole canvas turns invisible instead of only a few blocks. x determines the length of time of the invisibility and defaults to 12 (2 seconds) per attack "line".

  * `-hotpotato`

> Run the game in hot potato mode.

  * `-attack`, `-attackclean`, `-attack2`, `-attack2clean <type> [strength]`

> Change default attack types. Available types are lines, none, blind and fullblind. The last two may be followed by an optionnal strength parameter to specify the length of time of the blindness.

> The options `-attack2` and `-attack2clean` apply to the team holding the hot potato in that game mode.

  * `-nolevelup`

> Disable levelup (default).

  * `-levelup`

> Enable levelup.

  * `-level <x>`

> Start players on specified level.

  * `-combomin <x>`

> Set the minimum combo required to send attack lines.

  * `-nohandicap`

> Prevent players to set a handicap when joining the game.

  * `-record <x>`

> Record the game(s) played on the server. If the server is set to auto-restart (default), the recorded filenames will be x0000.rec, x0001.rec and so on. If auto-restart is disabled (using option -once), a single x.rec file will be created.

> Warning: any existing file of the same name will be overwritten.

  * `-exec <x>`

> Read file x and consider each line as a command line option or a remote adminitration command to be issued right after the game's creation. Lines starting with '-' are added to the command line and lines starting with '/' are executed as remote admin commands (with full administrative rights). Anything else is ignored.

# Remote administration #

A Quadra server (dedicated or not) can be remotely administered using Quadra itself or a standard telnet application as the client. When using Quadra, administrative commands can be issued using the game's chat interface.

When using telnet, a connection to the server must first be established like this:

```
telnet <address> <port>
```

where address is the internet address of the Quadra server and port is the port where the server is listening (usually 3456). Once the connection is open, the client must first type two slashes (//) to distinguish itself from a normal Quadra client (since they connect to the same port). That connection will then be considered by the server as a chat client so it is possible to use it to chat or send admin commands.

The commands are as follows:

  * `/help`

> Print a summary of available commands.

  * `/admin <password>`

> Turn on administrator mode. The password must exactly match the one specified with the command line option -admin when the server was started. The only commands available to clients that haven't yet provided the correct password are /admin, /score and /bye.

  * `/list`

> List the IP addresses and port of all currently active connections, with the name(s) of the player(s) on that connection.

  * `/drop <player|address:port>`

> Drop a player or the connection(s) coming from a specific IP address. Specifying only an address without the port number will drop all connections coming from that address.

  * `/acceptplayers <0|1>`

> Allow (1) or disallow (0) new players to join the game.

  * `/acceptconnects <0|1>`

> Allow (1) or disallow (0) new client computers to join the game.

  * `/endgame`

> End current game. If the server is set to auto-restart (default), a new game will be started. If the command line option -once was specified, the server will shut down.

  * `/quit`

> End current game and shut down the server.

  * `/score`

> Print a scoreboard of the current game.

  * `/bye`

> Terminate session.
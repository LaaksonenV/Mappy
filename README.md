# Mappy
Campaign tool primarily for games such as Mordheim as ran in my LFGS.
First versions will be just a tool for GM, later maybe evolving to a proper game platform

## How does it do de do
Add players to game, edit information as needed. All edits are done in their own respective dialogwindows'.
Edit moves from players by hand or copy-paste as single string, the edit window will attempt to split the text appropriately.
Start turn to move the players accordingly.
If players end up meeting and have to fight, edit the result(s) of the fight(s).
After all fights are resolved, end turn to complete remaining moves. If some moves cannot be resolved, manually change location of "offending" players, and end turn again.

## Rules
Each player can move up to three cells in a hexagonal map. If they move only up to one cell, they may set up a camp.
Players are moved on map one cell at a time in an order determined by their initiative, or by random if needed.
If two or more players end up in same cell, their movement stops, andd battle ensues.
Winning players may continue their moves, losing player will retreat one or more steps back jumping over other players. These moves may not end in the same location with other players.

## Now and sometimes later
First version will be just a simple player list with army states and moves' dialogs.
Its main raison d'ètre is for helping move players on the map, not to keep track of the whole game. This may come later.
Later I will add a proper map with interactive cells for more intuitive controls.
At some point the rules will be more or less editable, for now it's quickly made for a running game.

### Some plans

#### Minor stuff maybe possibly soonish or laterish
- Removing players (Currently possible by removing them from saved 'game.txt')
- Check if moves are legal (no jumping)
- Enhanced fighting dialog (eg. edit each players result in the same fight)
- Information dialogs for fights and other "interrupting" changes in game
- Anything else small that comes to mind or suggested

#### Map for version 2, some time in the near future
- Picture and grid, manually overlaid
- Icons and colours for players
- Locational extra information
- Unpassable gridlines, editable as single line between cells or by all lines of a cell
- Player and fight information reachable through the list and map

#### Planned stuff not deemed too neccessary at the moment
Do tell if you disagree with neccessity, otherwise I'll get to these when I have the time not wasteable for anything else.
- Saving settings such as window size and location
- Saving and loading "as", ie. multiple games (You can always rename 'game.txt' manually)

#### Possible stuff not really even in planning as of yet
- Keeping track of the whole game
- Network integration for inputting moves, through Discord or separate host and client applications or...

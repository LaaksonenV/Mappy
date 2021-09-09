# Mappy
Campaign tool primarily for games such as Mordheim as ran in my LFGS.
First versions will be just a tool for GM, later maybe evolving to a proper game platform

## How does it do de do
Add players to game, with a starting cell.
Write each players route as they come in.
End turn, and players will "move".
If players end up meeting, wait for retreat routes.
After all retreats are given, start new round, wait for routes.

## Rules
Each player can move up to three cells in a hexagonal map. If they move only up to one cell, they may set up a camp.
Players are moved on map one cell at a time in an order determined by their initiative.
If two or more players end up in same cell, their movement stops, andd battle ensues.
Player(s) losing the battle must retreat, avoiding other players.

## Now and sometimes later
First version will be just a simple player list with army states and moves' dialogs.
Later I will add a proper map with interactive cells for more intuitive controls.
At some point the rules will be more or less editable, for now it's quickly made for a running game.

### Version plan (just digital text, not carved in stone)
0.1 Initial working version
0.2 Check if moves are legal (no jumping, basically rudimentary map matrix)
0.3 Map
0.4 Improved map, with impassable cell walls
...
1.0 Proper working version


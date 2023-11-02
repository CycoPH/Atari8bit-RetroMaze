# Retro Maze #
by [Peter Hinz 2023 aka RetroCoder](https://github.com/CycoPH)

## Game idea ##
The basic idea for this game was a coding challenge: to draw a 3D looking maze using just ASCII characters.  With some extensive font modifications the result is *RetroMaze*.
Play through 15 ever more complex mazes and find enough items to unlock the exit. You can find chests with useful items, ghosts that can be traded with or simply attacked for their loot, and vortex jumps that take you to new locations.

Each level is unique never to be repeated.

## Interaction ##
On the start screen use the joystick up/down to select your starting level, or left/right to select the difficulty.
Press *FIRE BUTTON* to start the game.

In the maze you use the joystick to move. Forward/Backward will move you in that direction. Left/Right will turn your view into the selected direction. Hit and hold the *FIRE BUTTON* to show the map and action menu. Release the *FIRE BUTTON* to activate the selected action.

### Map interactions ###
Press and hold *FIRE BUTTON*, the view will change to show a top-down map of the walked maze in the middle of the screen, and a menu with actions at the bottom.

The map shows your current location and facing direction. It also shows all the exposed parts of the maze. The game employs a fog of war technique, so you looking down a long corridor will expose a small number of maze positions.  If you have not looked at it, it will not show on the map.

The action menu at the bottom of the map screen will list the actions available to you.  The list of actions depends on where you are in the map. At a minimum you will have the *BACK* and *POUCH* actions available to you.

Note that you enter the map/action view by pressing *AND* holding the fire button. The first highlighted action is always *BACK*. When you release the fire button the selected action is performed. In the default case releasing the fire button on the *BACK* action will return you to the 3D maze view. So hit fire to show the map, release to return the the 3D view.

Select a new action by pressing *UP* or *DOWN* on the joystick. The selection will change with each joystick push/pull. Release the *FIRE BUTTON* to activate the action.

#### Pouch actions ####
Select the pouch action and release the fire button to open the pouch. The pouch will show you how many *BOMBS*, *MAPS* and *VORTEX* actions you have available. Use the joystick left-right to select an action. Press fire to activate the action.
Use joystick up-down to get out of the pouch menu and select something else.

##### Vortex jump #####
When you activate the *VORTEX* from the pouch you jump directly to the exit location.
Unless you are at the exit location already, then you would jump to the middle of the maze.
This is very useful to check what you need to collect to finish the level.

##### Map reveal #####
Selecting the *MAP* from the pouch will reveal the whole map. Every power pill, all chests and ghosts are revealed.
Don't waste them, they become very useful in the larger levels.

##### Place a Bomb #####
Selecting the *BOMB* will switch back into the 3D maze view and a bomb will explode.
The effect of the bomb will break a random number (1-4) of walls in the direction you are facing, so make sure that you select the correct direction before you place the bomb. Great for making shortcuts in the maze.
The explosion will also affect any locked ghost, chests, exit. Try it and find out for yourself.

#### Chest actions ####
Walk up to a chest and hit the fire button.  If the chest is locked you will first have to unlock it before you can open it.
Hit the fire button to unlock, note you need enough energy, orbs, keys, etc to unlock the chest. Once unlocked you can take the three types of items inside.

#### Vortex actions ####
You can find vortex jumps in the maze.  These will randomly place you at another location.
Hit fire and select *WARP* to jump to a new location.

#### Exit actions ####
The level exit door is locked with two kinds of items. You need enough of each type to unlock the exit.

#### Ghosts actions ####
No maze game would be complete without ghosts. These are friendly but they do try to trick you when trading with them. Each ghost has specific trading options, and not all options are always to your advantage. The idea is that you use the ghosts to trade for items that you will need to open all the chests and unlock the exit door.

You can use the *TRADE* action to interact with a ghost. Each type of ghost has special trade options that depend on what you have in your inventory. Once the *TRADE* action is activated, the GIVE/TAKE menu is shown. Joystick left/right cycles through the available trade options. Press *FIRE* to action a trade. Joystick UP/DOWN will exit the trade menu.

*ATTACKING* a ghost is a quick and dirty technique to get hold of loot. It will cost you some of your inventory items to attack them, or try a bomb, but it will have consequences. Once attacked and looted the ghost will disappear and will not be available for trading.

#### Switch actions ####
The maze designers left behind some switches. Activating it will transform some of the maze. You may find that using them will make your travels much faster.

# Have fun and don't get lost #

# SSCoop
System Shock Remake Coop Prototype

WIP

Goal is to make use of the leftover standard UE network code in an attempt to get multiplayer working in this game.

How to test:

Compile the dll, rename it to umpdc.dll and place it inside the Win64 folder, then run the main exe at the root folder of the game.

Run the game (first instance will be the listen server) and head to the first level.

To connect, you must run another instance and inject Universal Unreal Unlocker 4, open the console and type "open ip" (ip is the listen server ip address)

or

Use ultimate asi loader, rename both .dll to .asi, create a plugins folder and place both of them inside it.

# Introduction #

This page displays the command line options available in the most recent release of KeeperFX.

Note that the [Launcher App](LauncherApplication.md) can be used to choose command line options - this way you won't have to type them by youself, but check the ones you want in a list.

# Details #

`-nointro`
> The intro sequence won't play at startup.

`-nocd`
> The game music will play from OGG files in the '/music' subfolder instead of the CD Sound tracks. You can get the required OGG files from <a href='http://www.gog.com/game/dungeon_keeper'>GOG release of DK</a>, or just download them from <a href='https://keeperklan.com/downloads.php?do=file&id=112'>here</a>.

`-1player`
> Allows playing multiplayer maps in skirmish mode.
> This is normally ON, so the option has no effect.

`-nosound` or `-s`
> Disables all the sounds.

`-fps <num>`
> Changes the game speed; default _num_ is 20.

`-usersfont`
> Disable the AWE32/64 SoundFonts (.SBK files).

`-alex`
> Used to show the 'JLW' easter egg. And not only that.

`-level <num>`
> Brings you directly to level number _num_.
> After the level is finished, quits the game.
> Note that level number must be 1..65534.

`-human <num>`
> Changes human player to _num_. This option will
> work properly only in skirmish mode. Single player
> levels must be specially designed for this option
> to work. Also, the selected player must have
> heart on the map.

`-q`
> Works like '-level 1'.

`-columnconvert`
> It converts the columns data. Watch out with
> this one or you may be unable to play the game again.

`-lightconvert`
> Same thing, but for lights.

`-vidriver <driver>`
> Set the video driver to be used by SDL library.
> Valid options on Windows host are 'directx' and 'windib'.
> Use this if your system is broken and most games do not work on it. Only available in KeeperFX versions 0.4.5 and before.

`-vidsmooth`
> Smoothes the 3D view using 1-pixel bilinear blur.
> This consumes more CPU, and the effect is merely visible,
> so blurring is disabled by default.

`-ppropoly <mode>`
> Allows to control "pentium pro polygon rendering". Default
> value is 0, which means the game will detect if the CPU is
> modern enough and enable advanced rendering based on that.
> For processors below Pentium Pro (today such CPU would be
> ancient) the game disables advanced polygon lightning
> computing. Setting _mode_ to 1 will make sure the advanced
> rendering is always enabled, and setting it to 2 disables
> Pentium Pro polygons regardless of the CPU.

`-altinput`
> Uses alternate mouse input method. This allows the mouse cursor to go outside of the game window. Ingame mouse sensitivity setting will be ignored. May be helpful if original method isn't working right
> (ie. mouse stops).

`-packetsave <filename>`
> Writes a packet file (replay file) when playing.
> After using this option, you must start a new level
> (or use '-level' parameter). Saved replay will work
> properly as long as you won't change any of the game
> files. Even a minor change in map or configuration
> may make the replay invalid.

`-packetload <filename>`
> Loads a previously created packet file. Starts the
> level for which packet file was created, and continues
> the gameplay. You may exit this mode by pressing
> Alt+X, or take over the control by pressing ALT+T.
> Note that this option is experimental, and packet files
> may sometimes not work as intended.

`-sessions <hostname1:port;hostname2:port;etc>`
> Adds sessions to the TCP/IP session list. _hostname_
> is either a DNS name or an IPv4 address (the latter ought
> to be far more common).

`-compuchat <mode>`
> Makes the Computer Player announce all his actions. Useful for testers and mapmakers. Valid options are 'scarce' for announcing only the major decisions and 'frequent' which will announce every task the computer player takes.
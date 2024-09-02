# NES Puzzle Maker rom sources

This is the source for a puzzle NES game you can customize to make your own games! 

The main site is: https://puzzle.nes.science

You can do most things with the game there, without ever touching this source code!

It is loosely based off of [nes-starter-kit](https://nes-starter-kit.nes.science), 
however it makes many adjustments. 

It uses the following tools: 
* [create-nes-game](https://create-nes-game.nes.science)
* [neslib](https://shiru.untergrund.net/code.shtml)
* [Famitracker](http://famitracker.com/)
* [Early 80's Arcade Pixel Art](https://opengameart.org/content/early-80s-arcade-pixel-art-dungeonsslimes-walls-power-ups-etc)
* [Multiple songs by Wolfgang on OpenGameArt](https://opengameart.org/users/wolfgang) (Note: Music is silence if built from source)

## Building

This game uses [create-nes-game](https://create-nes-game.nes.science) for game building. Download and 
optionally install that.

Next, copy the code for this repository to your pc somehow, either using git or the "Download Zip" feature under
the code button at the top-right of the github ui. From that directory, run the following command to set up the 
repository (you only have to do this once per pc): 

```
create-nes-game download-dependencies
```

After that, you can build at any time with `create-nes-game build`. You can also use `create-nes-game run` to run
the game in mesen.

## Using custom rom data

So, you made a game with `puzzle.nes.science` and now you wanna tweak it. Awesome! The majority of game data is
stored in `source/assembly/library/patchable_data.asm` You can generate a new version of this file from the tool.

Go to [the tool](https://puzzle.nes.science) and load the game you want to export. 

Now go to the homepage of the tool. On the right side you'll see a section called "Developer tools". Under it
there should be an option: `Download patachable_data.asm`. Click that, then replace the file in this repository.
That's most of it!

If you're using custom music, put the `.bin` file you used in the tool in as `sound/music.bin`.

If you're using custom tiles, put the `.chr` file you used in the tool as `graphics/tiles_mod.chr`.

If you're using custom sprites, put the `.chr` file you used in the tool as `graphics/sprites_mod.chr`.

If you're using a custom font, put the `.chr` file you used in the tool as `graphics/ascii_mod.chr`.

If you're using custom sound effects, put the `.nsf` file you used in the tool as `sound/sfx.nsf`

That's about it!

## Smaller rom version

A version of the rom that was compatible with the nesdev compo was available through rom version 5-3, and
editor version 1.4.4. It was removed to allow adding more features, though you can certainly make a smaller rom
with some careful hacking at the rom's content.

## License and copyright

The tool is copyrighted to Indie GameWeb, as of 2024. The rom sources are available under an MIT license, however
art from the online tool may have other licenses that need to be followed. The source code cannot help you with that, so
please be aware of required licenses for any art and music you may use! (The sound effects were created by me, and are
available under the same MIT license.)

# NES Puzzle Maker rom sources

This is the source for a puzzle NES game you can customize to make your own games! 

The main site is: https://puzzle.nes.science

You can do most things with the game there, without ever touching this source code!

It is loosely based off of [nes-starter-kit](https://cppchriscpp.github.io/nes-starter-kit), 
however it makes many adjustments. 

It uses the following tools: 
* [create-nes-game](https://cppchriscpp.github.io/create-nes-game)
* [neslib](https://shiru.untergrund.net/code.shtml)
* [Famitracker](http://famitracker.com/)
* [Early 80's Arcade Pixel Art](https://opengameart.org/content/early-80s-arcade-pixel-art-dungeonsslimes-walls-power-ups-etc)
* [Multiple songs by Wolfgang on OpenGameArt](https://opengameart.org/users/wolfgang) (Note: Music is silence if built from source)

## Building

This game uses [create-nes-game](https://cppchriscpp.github.io/create-nes-game) for game building. Download and 
optionally install that, then run the following command to set up the repository (you only have to do this once
per pc): 

```
create-nes-game download-dependencies
```

After that, you can build at any time with `create-nes-game build`. You can also use `create-nes-game run` to run
the game in mesen.

## Using custom rom data

So, you made a game with `puzzle.nes.science` and now you wanna tweak it. Awesome! The majority of game data is
stored in `source/assembly/library/patchable_data.asm` You can generate a new version of this file from the tool.

Go to [the tool](https://puzzle.nes.science) and load the game you want to export. 

Now go to the root page of the tool. On the right side you'll see a section called "Developer tools". Under it
there should be an option: `Download patachable_data.asm`. Click that, then replace the file in this repository.
That's most of it!

If you're using custom music, put the `.bin` file you used in the tool in as `sound/music.bin`.

If you're using custom graphics, put the `.chr` file you used in the tool as `graphics/tiles_mod.chr`.

That's about it!

## Smaller rom version

A version of the rom that should be compatible with the nesdev compo is now available as well. It reduces the rom
size from 128kb down to 64kb. Some features in the UI will also be restricted or disabled. Enable this feature in 
the UI on the "Other Settings" page. This will make the editor work with the smaller rom.

If you want to compile it yourself, you won't be able to do this as simply as you can above. When you run 
`create-nes-game build` it will work, but build a 128kb rom. That's not quite what you want! 

You can make it work though - in the `config` folder, there are two files: `ca65.cfg` and `ca65-compressed.cfg`.
Rename `ca65.cfg` to something else, then rename `ca65-compressed.cfg` to `ca65.cfg`. Now you can run 
`create-nes-game` again and it should build the 64kb version. Have fun!

## License and copyright

The tool is copyrighted to Christopher Parker, as of 2022. The rom sources are available under an MIT license, however
art from the online tool may have other licenses that need to be followed. The source code cannot help you with that, so
please be aware of required licenses for any art and music you may use! (The sound effects were created by me, and are
available under the same MIT license.)
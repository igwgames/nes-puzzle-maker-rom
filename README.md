# NES Puzzle Maker rom sources

This is the source for a puzzle NES game you can customize to make your own games! 

The main site is: https://puzzle.nes.science

You can do most things with the game there, without ever touching this source code!

It is loosely based off of [nes-starter-kit](https://cppchriscpp.github.io/nes-starter-kit), 
however it makes many adjustments. 

It uses the following tools: 
* [neslib](https://shiru.untergrund.net/code.shtml)
* [Famitracker](http://famitracker.com/)
* [Early 80's Arcade Pixel Art](https://opengameart.org/content/early-80s-arcade-pixel-art-dungeonsslimes-walls-power-ups-etc)
* [Multiple songs by Wolfgang on OpenGameArt](https://opengameart.org/users/wolfgang) (Note: Music is silence if built from source)

## Building

The building instructions are basically identical to those of nes-starter-kit - follow those! 

[nes-starter-kit instructions](https://cppchriscpp.github.io/nes-starter-kit//guide/section_1/setting_up_your_tools.html)

Now, there is one thing that is different. Delete the `cc65` folder in `tools`. Instead, download the latest version
from [here](https://sourceforge.net/projects/cc65/files/cc65-snapshot-win32.zip/download) and put that in `tools` instead.

## Using custom rom data

So, you made a game with `puzzle.nes.science` and now you wanna tweak it. Awesome! The best way to do that right now is
with an IPS patch. The tool gives you a (kinda secret) way to do this.

Go to [the tool](https://puzzle.nes.science) and load the game you want to export. Now, open the developer tools. The
easiest way to do this is to right-click any element and choose "Inspect" or "Inspect Element". Within the tools, find
the "Console" tab, and paste the following in: 

```javascript
await getIpsData()
```

This will print out a url you can click and save. Rename this to `puzzle.ips` and drop it in the `rom` folder of this
project, next to `puzzle.nes`, which you should have built. 

Now if you open your rom, your emulator should hopefully automatically patch the rom with your data, just like in the
tool! You can also use tools like [Lunar's IPS patcher](http://www.romhacking.net/utilities/240/) to build it into
the rom, if you want to distribute it. 

Editor's note: Yes, this isn't super duper ideal, however forcing the data into an assembly or C file takes quite a bit
of work and refactoring. If this is something you really want, open an issue asking for it. It's not impossible, just a
lot of work. It may also slow down future releases of the tool, if I can't find a smart way to do it. 

## License and copyright

The tool is copyrighted to Christopher Parker, as of 2022. The rom sources are available under an MIT license, however
art from the online tool may have other licenses that need to be followed. The source code cannot help you with that, so
please be aware of required licenses for any art and music you may use! (The sound effects were created by me, and are
available under the same MIT license.)
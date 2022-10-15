# This wraps create-nes-game for purposes of building multiple roms. Right now I separate out the general user 
# rom and the one for people aiming for the nesdev compo. I strip out some features for nesdev compo users, though
# I hope to eliminate the need for this at some point. 

# If you just want to build a game and don't care about the smaller rom, uncheck the option in the ui and use
# `create-nes-game build` without even thinking about this.
VERSION=v5-2

build: build-full build-compressed

build-full: 
	create-nes-game build

build-compressed:
	create-nes-game --linker-config-file config/ca65-compressed.cfg --assembler-options "-D SMALL_ROM" --output-file "rom/puzzle-compressed.nes" build


clean: 
	create-nes-game clean

s3_upload:
	make build
	mc cp ./rom/puzzle.nes  s3/cpprograms-nes-games-https/retro-puzzle-maker-$(VERSION).nes && mc policy public s3/cpprograms-nes-games-https/retro-puzzle-maker-$(VERSION).nes
	mc cp ./rom/puzzle-compressed.nes  s3/cpprograms-nes-games-https/retro-puzzle-maker-$(VERSION).nes && mc policy public s3/cpprograms-nes-games-https/retro-puzzle-maker-compressed-$(VERSION).nes
	git push origin :rom-$(VERSION) || echo "No remote tag to delete"
	git tag -d rom-$(VERSION) || echo "No local tag to delete"
	git tag rom-$(VERSION)
	git push
	git push --tags 
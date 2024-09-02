# This wraps create-nes-game for purposes of releasing to s3. 
# You can probably ignore it
VERSION=v6-1

build: build-full

build-full: 
	create-nes-game build

clean: 
	create-nes-game clean

s3_upload:
	make build
	git push origin :rom-$(VERSION) || echo "No remote tag to delete"
	git tag -d rom-$(VERSION) || echo "No local tag to delete"
	git tag rom-$(VERSION)
	git push
	git push --tags 
	mc cp ./rom/puzzle.nes  s3/cpprograms-nes-games-https/retro-puzzle-maker-$(VERSION).nes && mc policy public s3/cpprograms-nes-games-https/retro-puzzle-maker-$(VERSION).nes
	curl -k -L -o puzz.zip -O https://github.com/igwgames/nes-puzzle-maker-rom/archive/refs/tags/rom-${VERSION}.zip
	mc cp ./puzz.zip s3/cpprograms-nes-games-https/retro-puzzle-maker-${VERSION}.zip && mc policy public s3/cpprograms-nes-games-https/retro-puzzle-maker-${VERSION}.zip
	rm puzz.zip

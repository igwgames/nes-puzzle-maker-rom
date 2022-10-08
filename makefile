# You don't want this. I left one little command here for release purposes. create-nes-game handles 
# everything you care about.
VERSION=v5-1

s3_upload:

	mc cp ./rom/puzzle.nes  s3/cpprograms-nes-games-https/retro-puzzle-maker-v5-1.nes && mc policy public s3/cpprograms-nes-games-https/retro-puzzle-maker-$(VERSION).nes
	git push :rom-$(VERSION) || echo "No remote tag to delete"
	git tag -d rom-$(VERSION) || echo "No local tag to delete"
	git tag rom-$(VERSION)
	git push
	git push --tags 
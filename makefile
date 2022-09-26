# You don't want this. I left one little command here for release purposes. create-nes-game handles 
# everything you care about.

s3_upload:
	mc cp ./rom/puzzle.nes  s3/cpprograms-nes-games-https/retro-puzzle-maker-v5-1.nes && mc policy public s3/cpprograms-nes-games-https/retro-puzzle-maker-v5-1.nes
# Fairly-Simple Minecraft Editor
A simple minecraft bedrock edition save file format decoder (work in progress)

Current Progress:
- draw and export chunk structure, example from my testworld:

![chunk structure image](worldData/chunks.png)

- export parsed world/player data in:

[worldData/level.json](worldData/level.json) <- level.dat<br>
[worldData/player.json](worldData/player.json) <- ~local_player<br>
[worldData/plain.txt](worldData/plain.txt) <- FSmcedit.exe stdoutput

- export chunk data in

[worldData/chunk_x_y.json](worldData/chunk_20_1.json)
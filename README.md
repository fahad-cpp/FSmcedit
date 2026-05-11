# Fairly-Simple Minecraft Editor
A simple minecraft bedrock edition save file format decoder (work in progress)

Current Progress:
- draw and export chunk structure, example from my testworld:

<img src="worldData/chunks.png" width="256">

- export parsed world data in:

[worldData/level.json](worldData/level.json) <- level.dat<br>
[worldData/plain.txt](worldData/plain.txt) <- FSmcedit.exe stdoutput

- export chunk data in:

[worldData/chunks.json](worldData/chunks.json) (dumped without indent to save space) (large file)

- export players' data on server in:

[worldData/players.json](worldData/players.json)

- export entity data in:

[worldData/entities.json](worldData/entities.json)


## Usage

At least one option should be specified
```bash
FSmcedit --world <world_folder> --struct <mcstructure_file>
```

passing `--struct` and a .mcstructure file will give you material list for that structure.

and passing `--world` and world folder path will parse world folder and output parsed data in worldData
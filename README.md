# Due-Recompense is a text-based computer roleplaying game.

  Features:
1. Your choices matter!
2. Original soundtrack (first time making music, so it may not be good)
3. ASCII artworks for specific objects/entities (first time making ASCII art, so it may not be good)
4. Dialogues and turn-based boss battles

# How to compile 
OS: Windows 10  
MinGW gcc version: 9.2.0  
Dependency: SDL2, SDL2_mixer  
Tools: 7-zip  

Procedure:
  1. Clone repository to your computer.
  2. Go to https://www.libsdl.org/download-2.0.php (SDL2 download link).
  3. Download the latest version for your OS in Development Libraries. Don't close yet.
  4. Go to https://www.libsdl.org/projects/SDL_mixer/ (SDL2_mixer download link).
  5. Download the latest version for your OS in Development Libraries. Don't close yet.
  6. Navigate to MinGW\include\ (the include path).
  7. Open both archives for Development Libraries files. You should see an SDL2-2.x.x\ folder and an SDL2_mixer-2.x.x\ folder. Expand these folders.
  8. Navigate to i686-w64-mingw32 folder (for MinGW32 version) or x86_64-w64-mingw32 folder (for MinGW64 version). Then, navigate to the include\ folder.
  9. Extract the both SDL2\ folders to the MinGW\include\ folder. SDL_mixer.h should be in the SDL2\ folder after this.
  10. From the download links above, download the Runtime Binaries version that matches the MinGW version you extracted. For example, if you extracted from i686-w64-mingw32,     download the 32-bit version. 
  11. Extract all the .dll files from both archives to the directory that contains main.c.
  12. Compile the program with the line gcc main.c -o Due_Recompense.exe -LC:\MinGW\include\lib -lSDL2 -lSDL2_Mixer -lmingw32
Note: C:\MinGW\include\lib should be replaced with the directory where libSDL2.a and libSDL2_mixer.a are located.
  13. Run the program by double clicking Due_Recompense.exe or typing Due_Recompense in the terminal from the directory.

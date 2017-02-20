**Please read this before reporting any issues with the code, as it is quite likely to solve your issues!**

# About

Code for the book [Game Hacking: Developing Autonomous Bots for Online Games](http://www.nostarch.com/gamehacking).

Each project in this repository corresponds to a chapter or section in the book. The projects are referenced in the relevant pages.

# Purpose
The exact purpose of each binary will be explained in the book. Some binaries are meant to be scanned and debugged, others simply exist as proof-of-concepts for lessons, and a few actually illustrate how certain algorithms look with different inputs.

# Compiling
All of the example code should compile out-of-the-box on Visual Studio 2010, and any newer version. The exception is the two practice binaries for Chapter 1, which rely on the `Allegro 5.0.10` library to be installed with the `bin`, `lib`, and `include` directories properly set up in your Visual Studio paths for additional include directories and additional linker directories.

If you can't compile yourself, all of the binaries are in the `/bin/` directory. Debug binaries are in the `/bin/DEBUG_BUILDS/` directory with included debug databases. Specifically, there is zero need to compile the binaries for the labs, and these are the ones likely to give you trouble (because of missing Allegro libs). The remaining code should compile fine, as you can poke it and recompile as much as you'd like.

# Usage
You may need to grab [this redistributable package from Microsoft](https://www.microsoft.com/en-us/download/details.aspx?id=5555) to run the binaries if you don't have Visual Studio. If you get errors about loading fonts or map, move [arial.ttf](https://github.com/GameHackingBook/GameHackingExamples/blob/master/arial.ttf) and [game.map](https://github.com/GameHackingBook/GameHackingExamples/blob/master/game.map) into the same directory as the binary and try again.

If you for same reason want to use the debug binaries, you will need the debug redistributable installed. This comes with Visual Studio 2010, and you can also find the stand-alone DLLs online. If this give you trouble, I'd recommend either sticking to the release binaries, or compiling on your local version of Visual studio given the compile steps above.

# GameHackingExamples

Example code for the book [Game Hacking: Developing Autonomous Bots for Online Games](http://www.nostarch.com/gamehacking).

Each project in this repository corresponds to a chapter or section in the book. The projects are referenced in the relevant pages.


# Compiling
All of the example code should compile out-of-the-box on Visual Studio 2010, and any newer version. The exception is the two practice binaries for Chapter 1, which rely on the `Allegro 5.0.10` library to be installed with the `bin`, `lib`, and `include` directories properly set up in your Visual Studio paths for additional include directories and additional linker directories.

If you can't compile yourself, all of the binaries are in the `/bin/` directory. Debug binaries are in the `/bin/DEBUG_BUILDS/` directory with included debug databases. You may need to grab [this redistributable package from Microsoft](https://www.microsoft.com/en-us/download/details.aspx?id=5555) to run the binaries if you don't have Visual Studio.

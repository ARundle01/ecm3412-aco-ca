# ReadMe

## Compilation

Compilation was done using a VScode Build Task following the structure: `g++ -O3 -fdiagnostics-color=always -std=c++11 -g ${fileDirname}/**.cpp -o ${fileDirname}\\${fileBasenameNoExtension}.exe`.

## Running the executable

To run the executable, one should use a command line shell as such: `binPacking.exe {PROBLEM NUMBER} {NUMBER OF ANTS} {EVAPORATION RATE}`.

For example, running BPP1 with 100 ant paths and an evaporation rate of 0.5, I would do the following: `binPacking.exe 1 100 0.5`.

## Results files

`BPP1 results.txt` and `BPP2 results.txt` contain results from my own runs of the algorithm, as found in the report.

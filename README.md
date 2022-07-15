# Chip-8
A chip-8 interpreter/emulator built in C++ using SDL2 for graphics. 

# Build Instructions (Windows and MinGW)

1. Put SDL2 inside "devs" folder in project root directory. 
2. mkdir build
3. cd build
4. cmake -G "MinGW Makefiles" ..
5. mingw32-make
6. ./main.exe \<ROM> \<Scale> \<Delay in hz>

# Notes
The version of SDL2 used is 2.0.22. 

I have only tested on windows with GCC/G++ MinGW but it should work with other compilers and toolkits.  


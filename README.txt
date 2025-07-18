DataAugmenter - Utility that allows for painless image data augmentation
Emmanuel Butsana, 05 June 2025

Flags:

-i : Input directory containing image files
-o : Output directory to write image files to

Usage:

1. Install dependecies (common C++ and OpenCV)
2. Compile source (g++ image.cpp main.cpp manipulations.cpp multithread.cpp pipeline.cpp `pkg-config --cflags --libs opencv4` -o dataAugmenter)
3. Run code (./dataAugmenter -i ./input/path -o ./output/path). Path can be entered as relative or absolute path


Potential improvements:

--> Add flag to allow users to change the # of workers
--> Add flag to allow users to determine augmentation factor (fixed at 5)
--> Error checking for image file type
--> GPU acceleration


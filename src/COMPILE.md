g++ src/client.cpp src/icon.o -lmingw32 -lSDL2 -llibSDL2_image -llibSDL2_mixer -llibSDL2_net -llibSDL2_ttf -lglew32 -lglew32s -lopengl32 -o "3D Engine".exe

windres src/icon.rc -o src/icon.o    

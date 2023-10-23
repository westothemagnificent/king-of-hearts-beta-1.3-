g++ -static -v -Iinclude/SDL_TTF -Iinclude/SDL2 -Llib/SDL_TTF -Llib/SDL2/x64 -o build/game src/game.cpp  -l:SDL2main.lib -l:SDL2.lib -l:libSDL2_ttf.dll.a -lmingw32
copy lib\SDL2\x64\SDL2.dll build\
copy lib\SDL_TTF\SDL2_ttf.dll build\
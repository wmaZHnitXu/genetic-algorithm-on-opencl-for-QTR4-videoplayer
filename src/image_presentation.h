#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <png.h>

SDL_Surface *createSurfaceFromTexture(SDL_Renderer *renderer, SDL_Texture *texture, int width, int height);

void displayImage(const char* imagePath);
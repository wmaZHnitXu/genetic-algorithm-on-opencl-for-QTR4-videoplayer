#include <image_presentation.h>
#include <dmatrix.h>
#include <math.h>

SDL_Surface *createSurfaceFromTexture(SDL_Renderer *renderer, SDL_Texture *texture, int width, int height) {
    SDL_Surface *surface = NULL;
    SDL_Texture *target = NULL;

    // Save the current render target (so we can restore it later)
    SDL_Texture *oldTarget = SDL_GetRenderTarget(renderer);

    // Create a temporary texture we can use as a render target
    target = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, width, height);

    if (!target) {
        printf("Failed to create temporary texture\n");
        return NULL;
    }

    // Set the new temporary texture as the render target
    if (SDL_SetRenderTarget(renderer, target) < 0) {
        printf("Failed to set temporary texture as render target\n");
        SDL_DestroyTexture(target);
        return NULL;
    }

    // Clear the render target
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);

    // Copy the source texture to the render target
    if (SDL_RenderCopy(renderer, texture, NULL, NULL) < 0) {
        printf("Failed to copy texture\n");
        SDL_DestroyTexture(target);
        return NULL;
    }

      // Create surface from the contents of the current render target (the temporary texture)
    surface = SDL_CreateRGBSurface(0,width,height ,32,
                                   #if(SDL_BYTEORDER==SDL_BIG_ENDIAN)
                                   0xFF000000,
                                   0x00FF0000,
                                   0x0000FF00,
                                   0x000000FF
                                   #else
                                   0x000000FF,
                                   0x0000FF00,
                                   0x00FF0000,
                                   0xFF000000
                                    #endif
                                  );

    if(surface ==NULL){
        printf("Failed to create surface \n");
        return NULL;
    }

    SDL_RenderReadPixels(
      renderer,NULL,surface->format->format,surface->pixels,surface->pitch);


    // Restore old Render Target
    SDL_SetRenderTarget(renderer , oldTarget);


    return surface;

}

void displayImage(const char* imagePath) {
    // Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        return;
    }

   // Initialize PNG loading with SDL_image library
   int imgFlags = IMG_INIT_PNG;
   if (!(IMG_Init(imgFlags) & imgFlags)) {
       fprintf(stderr, "Could not initialize SDL_image: %s\n", IMG_GetError());
       return;
   }

   // Create window
   SDL_Window *window = SDL_CreateWindow("PNG Image Display",
                                         SDL_WINDOWPOS_CENTERED,
                                         SDL_WINDOWPOS_CENTERED,
                                         1024,
                                         1024,
                                         0);

   if (!window) {
       fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
       return;
   }

   // Create renderer
   SDL_Renderer *renderer = SDL_CreateRenderer(window,-1 , SDL_RENDERER_ACCELERATED);

    if (!renderer){
        SDL_DestroyWindow(window);
        fprintf(stderr, "Sdl_createRender failed ;% s \n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_Texture *texture= IMG_LoadTexture(renderer,imagePath);
    if(!texture){
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        printf("failed to load texture :% s \n ",  IMG_GetError());
    }

    int w = 1024;
    int h = 1024;
    SDL_Surface *surface = createSurfaceFromTexture(renderer, texture, w, h);
    // Assuming 'surface' is your SDL_Surface*

    Uint32 *pixels = (Uint32 *)surface->pixels;
    Uint32 color = SDL_MapRGBA(surface->format, 255, 0, 0, 255); // Red color

    for(int y = 0; y < surface->h / 2; y++) {
        for(int x = 0; x < surface->w / 2; x++) {
            int index = (y * surface->w) + x;
            pixels[index] = color;
        }
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer , texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    SDL_Delay(5000); //Wait before closing

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);


    return;

}

void displayMatrix(DMatrix* matrix) {
    // Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        return;
    }

   // Initialize PNG loading with SDL_image library
   int imgFlags = IMG_INIT_PNG;
   if (!(IMG_Init(imgFlags) & imgFlags)) {
       fprintf(stderr, "Could not initialize SDL_image: %s\n", IMG_GetError());
       return;
   }

   // Create window
   SDL_Window *window = SDL_CreateWindow("PNG Image Display",
                                         SDL_WINDOWPOS_CENTERED,
                                         SDL_WINDOWPOS_CENTERED,
                                         1024,
                                         1024,
                                         0);

   if (!window) {
       fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
       return;
   }

   // Create renderer
   SDL_Renderer *renderer = SDL_CreateRenderer(window,-1 , SDL_RENDERER_ACCELERATED);

    if (!renderer){
        SDL_DestroyWindow(window);
        fprintf(stderr, "Sdl_createRender failed ;% s \n", SDL_GetError());
        exit(EXIT_FAILURE);
    }


    SDL_Texture *texture = NULL;
    int w = matrix->cols;
    int h = matrix->rows;
    SDL_Surface *surface = SDL_CreateRGBSurface(0,w,h,32,0,0,0,0);


    Uint32 *pixels = (Uint32 *)surface->pixels;

    for(int y = 0; y < surface->h; y++) {
        for(int x = 0; x < surface->w; x++) {
            Uint32 color = matrix->data[y][x];
            int index = (y * surface->w) + x;
            pixels[index] = color;
        }
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer , texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    SDL_Delay(5000); //Wait before closing

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);


    return;
}
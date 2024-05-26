#include <image_presentation.h>
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

    SDL_Delay(15000); //Wait before closing

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
   SDL_Window *window = SDL_CreateWindow("Matrix Display",
                                         SDL_WINDOWPOS_CENTERED,
                                         SDL_WINDOWPOS_CENTERED,
                                         512,
                                         512,
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
            Uint32 color = matrix->data[y * w + x];
            int index = (y * surface->w) + x;
            pixels[index] = color;
        }
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer , texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    SDL_Delay(15000); //Wait before closing

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);


    return;
}

DMatrix* createMatrixFromPng(char* path) {
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        fprintf(stderr, "Could not open file %s for reading\n", path);
        return allocDMatrix(16, 16);
    }

    // Create and initialize the png_struct
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        fclose(fp);
        fprintf(stderr, "Could not allocate read struct\n");
        return allocDMatrix(16, 16);
    }

    // Create and initialize the png_info struct
    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_read_struct(&png, (png_infopp)NULL, (png_infopp)NULL);
        fclose(fp);
        fprintf(stderr, "Could not allocate info struct\n");
        return allocDMatrix(16, 16);
    }

    // Set error handling
    if (setjmp(png_jmpbuf(png))) {
        png_destroy_read_struct(&png, &info, (png_infopp)NULL);
        fclose(fp);
        fprintf(stderr, "Error during png creation\n");
        return allocDMatrix(16, 16);
    }

    png_init_io(png, fp);
    png_read_info(png, info);

    // Get image info
    int width = png_get_image_width(png, info);
    int height = png_get_image_height(png, info);
    png_byte color_type = png_get_color_type(png, info);
    png_byte bit_depth = png_get_bit_depth(png, info);

    // Read any color_type into 8bit depth, RGBA format.
    // See http://www.libpng.org/pub/png/libpng-manual.txt
    if (bit_depth == 16)
        png_set_strip_16(png);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);

    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);

    if (png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);

    // These color_type don't have an alpha channel then fill it with 0xff.
    if (color_type == PNG_COLOR_TYPE_RGB ||
        color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    png_read_update_info(png, info);

    // Allocate memory to read the image data
    png_bytep *row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
    for (int y = 0; y < height; y++) {
        row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
    }

    png_read_image(png, row_pointers);

    DMatrix* matrix = allocDMatrix(width, height);

    // Process image data
    float r = 0;
    float g = 0;
    float b = 0;
    int c = 0;
    for (int y = 0; y < height; y++) {
        png_bytep row = row_pointers[y];
        for (int x = 0; x < width; x++) {
            png_bytep px = &(row[x * 4]);
            // Do something with px (which is the pixel at (x, y))
            int value = 0xFF000000;
            value += px[0] * 0x00010000;
            value += px[1] * 0x00000100;
            value += px[2] * 0x00000001;
            setDMatrixElem(matrix, y, x, value);
            r += (float)px[0];
            g += (float)px[1];
            b += (float)px[2];
            c++;            
        }
    }
    printf("Avg color: RGBA(%f, %f, %f)\n", r, g, b);

    // Cleanup
    for (int y = 0; y < height; y++) {
        free(row_pointers[y]);
    }
    free(row_pointers);

    fclose(fp);
    png_destroy_read_struct(&png, &info, NULL);
    return matrix;
}
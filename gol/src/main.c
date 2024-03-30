#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <stdbool.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define ALIVE 0xFFFFFF
#define DEAD 0x000000
#define TEXT_COLOR {150, 0, 0}
#define TEXT_SIZE 24

void render_text(
    SDL_Renderer *renderer,
    int x,
    int y,
    const char *text,
    TTF_Font *font,
    SDL_Rect *rect,
    SDL_Color *color
){
    SDL_Surface *surface;
    SDL_Texture *texture;

    surface = TTF_RenderText_Solid(font, text, *color);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    rect->x = x;
    rect->y = y;
    rect->w = surface->w;
    rect->h = surface->h;
    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, NULL, rect);
    SDL_DestroyTexture(texture);
}

int main( int argc, char* argv[] )
{
    srand((unsigned)time(NULL));

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture[2];


    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Conway's Game of Life",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WINDOW_WIDTH,WINDOW_HEIGHT,0);
    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    for (int i = 0; i < 2; ++i) {
        texture[i] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                    SDL_TEXTUREACCESS_STATIC, WINDOW_WIDTH,
                                    WINDOW_HEIGHT);
    }

    TTF_Init();

    TTF_Font *font = TTF_OpenFont("", TEXT_SIZE);
    if (font == NULL) {
        fprintf(stderr, "error: font not found\n");
        
    }
    char gen_text[100], cell_text[100], brush_text[50];
    SDL_Color textColor = TEXT_COLOR;
    SDL_Rect textRect;


    Uint32 *cells[2];

    for (int i = 0; i < 2; ++i) {
        cells[i] = (Uint32*)malloc(sizeof(Uint32) * WINDOW_WIDTH * WINDOW_HEIGHT);
    }
    // int gosper_glider_gun[9][36] = {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
    //                                 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0},
    //                                 {0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
    //                                 {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
    //                                 {1,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    //                                 {1,1,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1,1,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0},
    //                                 {0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
    //                                 {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    //                                 {0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};

    // int gun_x_offset = (WINDOW_WIDTH - 36) / 2;
    // int gun_y_offset = (WINDOW_HEIGHT - 9) / 2;
    // for (int y = 0; y < 9; ++y) {
    //     for (int x = 0; x < 36; ++x) {   
    //     if (gosper_glider_gun[y][x] == 1) {
    //         cells[0][(y + gun_y_offset) * WINDOW_WIDTH + (x + gun_x_offset)] = ALIVE;
    //     }
    //     }
    // }

    SDL_Event event;
    int mouseX = 0, mouseY = 0; // Current mouse position
    bool isLeftButtonPressed = false;
    bool isRightButtonPressed = false;
    bool isSpacePressed = false;
    int quit = 0;
    int current_buffer = 0;
    int generationNumber = 0;
    int brushSize = 0;

    while(!quit){
        while (SDL_PollEvent(&event)){
            switch(event.type){
            case SDL_QUIT:
                quit = 1;
                break;
            case SDL_MOUSEMOTION:
                    mouseX = event.motion.x;
                    mouseY = event.motion.y;
                    break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    isLeftButtonPressed = true;
                }
                if(event.button.button == SDL_BUTTON_RIGHT){
                    isRightButtonPressed = true;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    isLeftButtonPressed = false;
                }
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    isRightButtonPressed = false;
                }
                break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym){
                    case SDLK_ESCAPE:
                        SDL_DestroyWindow(window);
                        SDL_Quit();
                        return 0;
                        break;
                    case SDLK_SPACE:
                        isSpacePressed = !isSpacePressed;
                        break;
                }
            case SDL_KEYUP:
                    
                    break;
            case SDL_MOUSEWHEEL:
                if(event.wheel.y > 0){
                    if(brushSize < 100){
                        brushSize+=2;
                    }
                }
                else if(event.wheel.y < 0){
                    if(brushSize > 2){
                        brushSize-=2;
                    }
                }
            }
        }
        
        int cellNumber = 0;
        if (isLeftButtonPressed) {
            // cells[current_buffer][mouseY * WINDOW_WIDTH + mouseX] = 0xFFFFFF;
            for (int i = mouseX-brushSize; i <= mouseX+brushSize; i++) {
                for(int j = mouseY-brushSize; j <= mouseY+brushSize; j++){
                    if (i < 0 || i >= WINDOW_WIDTH || j < 0 || j >= WINDOW_HEIGHT) {
                        continue;
                    }
                    cells[current_buffer][j * WINDOW_WIDTH + i] = ALIVE;
                }
                
            }
        }
        else if(isRightButtonPressed){
            for (int i = mouseX-brushSize; i <= mouseX+brushSize; i++) {
                for(int j = mouseY-brushSize; j <= mouseY+brushSize; j++){
                    if (i < 0 || i >= WINDOW_WIDTH || j < 0 || j >= WINDOW_HEIGHT) {
                        continue;
                    }
                    cells[current_buffer][j * WINDOW_WIDTH + i] = DEAD;
                }
                
            }
        }
        
        if(!isSpacePressed){
            for (int y = 0; y < WINDOW_HEIGHT; ++y) {
                for (int x = 0; x < WINDOW_WIDTH; ++x) {
                    int alive_neighbors = 0;

                    for (int i = x - 1; i <= x + 1; ++i) {
                        for (int j = y - 1; j <= y + 1; ++j) {
                            if (i < 0 || i >= WINDOW_WIDTH || j < 0 || j >= WINDOW_HEIGHT || (i == x && j == y)) {
                                continue;
                            }
                            if(cells[current_buffer][j * WINDOW_WIDTH + i] == ALIVE){
                                alive_neighbors++;
                            }
                        }
                    }
                    int index = y * WINDOW_WIDTH + x;
                    if (cells[current_buffer][index] == ALIVE) {
                        cells[1 - current_buffer][index] = (alive_neighbors == 2 || alive_neighbors == 3) ? ALIVE : DEAD;
                        cellNumber++;
                    } 
                    else if((cells[current_buffer][index] == DEAD)) {
                        cells[1 - current_buffer][index] = alive_neighbors == 3 ? ALIVE : DEAD;
                    }

                }
            }
            generationNumber++;
        }
        
        
        
        SDL_UpdateTexture(texture[1 - current_buffer], NULL, cells[1 - current_buffer],WINDOW_WIDTH * sizeof(Uint32));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture[current_buffer], NULL, NULL);

        snprintf(cell_text, 100, "Alive cells: %u", cellNumber);
        snprintf(gen_text,100, "Generation: %u", generationNumber);
        snprintf(brush_text,50, "Brush size: %u", brushSize);


        render_text(renderer, 0, 0, gen_text, font, &textRect, &textColor);
        render_text(renderer, 0, textRect.y + textRect.h, cell_text, font, &textRect, &textColor);
        render_text(renderer, 0, textRect.y + textRect.h, brush_text, font, &textRect, &textColor);


        SDL_RenderPresent(renderer);
        current_buffer = 1 - current_buffer;


    }
    TTF_CloseFont(font);
    for (int i = 0; i < 2; ++i) {
        free(cells[i]);
        SDL_DestroyTexture(texture[i]);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}






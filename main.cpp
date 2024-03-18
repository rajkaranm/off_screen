#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <ctime> 
#include <cstdlib>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Global window and renderer
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
bool quit = false;

// global texture
SDL_Texture* bg = nullptr;

//  Player
SDL_Texture* player = nullptr;
SDL_Rect player_rect = {.x = 0, .y = 455, .w = 60, .h = 60};

//  Monster
SDL_Texture* monster = nullptr;
SDL_Rect monster_rect = {.x = SCREEN_WIDTH - 60, .y = 455, .w = 60, .h = 60};
int monster_vel = 10;

SDL_Texture* coin = nullptr;
SDL_Rect coin_rect = {.x =  1, .y = 1, .w = 60, .h = 60};

TTF_Font* font = NULL;
SDL_Texture* fontTexture;
SDL_Rect font_rect = {.x =  300, .y = 200, .w = 200, .h = 100};

bool pause = false;
int score = 0;

bool init()
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "SDL could not initialize! SDL ERROR: " << SDL_GetError() << std::endl;
        return false;
    }

    IMG_Init(IMG_INIT_PNG);
    if (TTF_Init() == -1)
    {
        std::cout << "SDL could not initialize Text! SDL ERROR: " << SDL_GetError() << std::endl;
        return false;
    }



    // Create Window
    window = SDL_CreateWindow(
        "Hide", 
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if (window == nullptr)
    {
        std::cout << "SDL could not create window! SDL ERROR: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create a renderer for the window
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        std::cout << "SDL could not create renderer! SDL ERROR: " << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}

void handleEvents()
{
    // handle event
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT) 
            quit = true;
        else if (event.type == SDL_KEYDOWN) 
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_LEFT:
                if (pause == false)
                    player_rect.x -= 10;
                break;
            
            case SDLK_RIGHT:
                if (pause == false)
                    player_rect.x += 10;
                break;
            case SDLK_RETURN:
                if (pause == true)
                {
                    // Start game again
                    pause = false;
                    score = 0;
                    monster_rect.x = SCREEN_WIDTH - 60;
                    player_rect.x = 0;

                }
            
            default:
                break;
            }
        }
    }
}

SDL_Texture* loadTexture(const char* filename)
{
    SDL_Surface* surface = IMG_Load(filename);
    if (!surface)
    {
        std::cout<< "Surface could not be loaded: " << SDL_GetError() << std::endl;
    }
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return tex;
}

void loadTextTexture(std::string text)
{
    SDL_Color color;
    color.r = 255;
    color.b = 255;
    color.g = 255;
    color.a = 255;
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
    fontTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
}

void loadMedia()
{
    font = TTF_OpenFont("assets/lazy.ttf", 28);

    bg = loadTexture("assets/bg.png");
    if (bg == NULL)
        std::cout << "Failed to load bg" << SDL_GetError() << std::endl;
    player = loadTexture("assets/player.png");
    if (player == NULL)
        std::cout << "Failed to load player" << SDL_GetError() << std::endl;
    monster = loadTexture("assets/monster.png");
    if (monster == NULL)
        std::cout<< "Failed to load monster" << SDL_GetError() <<std::endl;

    coin = loadTexture("assets/coin.png");
    if (coin == NULL)
        std::cout<< "Failed to load coin" << SDL_GetError() <<std::endl;
}

void update()
{
    if (pause == false)
    {
        // Update monster
        if (monster_rect.x < 1)
            monster_vel = 5;
        else if (monster_rect.x > SCREEN_WIDTH - 61)
            monster_vel = -5;

        monster_rect.x += monster_vel;

        // update player



        // Collision detection between player and monster
        if ((player_rect.x < monster_rect.x + monster_rect.w) && 
            (player_rect.x + player_rect.w > monster_rect.x) &&
            (player_rect.y < monster_rect.y + monster_rect.h) &&
            (player_rect.y + player_rect.h > player_rect.y))
        {
            pause = true;
        }

        // update player
        if (player_rect.x < - 60)
            player_rect.x = SCREEN_WIDTH + 60;
        else if (player_rect.x > SCREEN_WIDTH + 61)
            player_rect.x = - 60;

        // Update Coin 
        if (coin_rect.y < 450)
        {
            coin_rect.y += 10;
        }
        // collision checking between coin and player
        if ((player_rect.x < coin_rect.x + coin_rect.w) && 
            (player_rect.x + player_rect.w > coin_rect.x) &&
            (player_rect.y < coin_rect.y + coin_rect.h) &&
            (player_rect.y + player_rect.h > player_rect.y))
        {
            score += 1;
            coin_rect.x = ((rand() % SCREEN_WIDTH) + 1);
            coin_rect.y = 1;
            
        }


    }

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);
}

void render()
{
    // Update screen
    SDL_RenderCopy(renderer, bg, NULL, NULL);
    SDL_RenderCopy(renderer, player, NULL, &player_rect);
    SDL_RenderCopy(renderer, coin, NULL, &coin_rect);
    SDL_RenderCopy(renderer, monster, NULL, &monster_rect);
    if (pause == true)
    {
        std::string str = "Score: ";
        str += std::to_string(score);
        loadTextTexture(str);
        // SDL_RenderCopy(renderer, fontTexture, NULL, &monster_rect);
        SDL_RenderCopy(renderer, fontTexture, NULL, &font_rect);
    }

    SDL_RenderPresent(renderer);
    // SDL_Delay(100);
}

void clean()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main()
{
    srand(time(0));
    coin_rect.x = ((rand() % SCREEN_WIDTH) + 1);
    std::cout<<coin_rect.x<<std::endl;

    // Initialize
    init();

    // load texture
    loadMedia();

    const int FPS = 60;
    const int desiredDelta = 1000 / FPS;


    // Event loop
    while (!quit)
    {
        int startLoop = SDL_GetTicks();
        handleEvents();
        update();
        render();
        int delta = SDL_GetTicks() - startLoop;
        if (delta < desiredDelta)
            SDL_Delay(desiredDelta - delta);
    }
    // clear resources
    clean();
    
    return 0;
}
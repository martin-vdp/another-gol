#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define WINDOW_SIZE 1000
#define CELL_SIZE 20


// DISCLAIMER: THIS IS BAD CODE NOT ENOUGH OPTIMIZED DO NOT PAY ATTENTION

static void render_cell(SDL_Renderer* renderer, uint8_t grid[50][50])
{
	SDL_Rect cell_rect;
	cell_rect.w = CELL_SIZE;
	cell_rect.h = CELL_SIZE;	

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

	for(unsigned int j=0;j<50;j++)
	{
		for(unsigned int y=0;y<50;y++)
		{
			if(grid[j][y] == 1)
			{
				cell_rect.x = j*20;
				cell_rect.y = y*20;
				SDL_RenderFillRect(renderer, &cell_rect);
			}
		}
	}
}

static void render_grid(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
	for(unsigned int i=1;i<WINDOW_SIZE/CELL_SIZE;i++)
	{
		SDL_RenderDrawLine(renderer, i*CELL_SIZE, 0, i*CELL_SIZE, WINDOW_SIZE);
		SDL_RenderDrawLine(renderer, 0, i*CELL_SIZE, WINDOW_SIZE, i*CELL_SIZE);
	}
}

static void click_cell(uint8_t grid[50][50], unsigned int x, unsigned int y)
{
	grid[x/20][y/20] = grid[x/20][y/20] == 1 ? 0 : 1;
}

static uint8_t count_neighbors(uint8_t grid[50][50], unsigned int x, unsigned int y)
{
	uint8_t sum = 0;
	for(int8_t i = -1;i<2;i++)
		for(int8_t j = -1;j<2;j++)
			sum += grid[x+i][y+j];
	return sum - grid[x][y];
}

static void compute_grid(uint8_t grid[50][50])
{
	uint8_t old_grid[50][50];
	memcpy(old_grid, grid, 50*50);

	uint8_t neighbors = 0;

	for(unsigned int j=0;j<50;j++)
	{
		for(unsigned int y=0;y<50;y++)
		{
			if(j == 0 || y == 0 || j == 49 || y == 49) continue;		
			
			neighbors = count_neighbors(old_grid, j, y);
			
			if(old_grid[j][y] == 0 && neighbors == 3)
				grid[j][y] = 1;
			else if(old_grid[j][y] == 1 && (neighbors < 2 || neighbors > 3))
				grid[j][y] = 0;
			else    grid[j][y] = old_grid[j][y]; 
		}
	}	

}

int main(void)
{
	// TODO:
	// - making it generic like "./a.out 10 20" to have a 10 * 20 grid (easy just change the vars you use to calculate the stuff my ass)

	if(SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		fprintf(stderr, "SDL_Init: %s", SDL_GetError());
		return -1;
	}
	

	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
	{
		fprintf(stderr, "Couldn't init IMG: %s", IMG_GetError());
		return -3;
	}

	SDL_Window* window = SDL_CreateWindow("EPIC BALD BOEY'S GAME OF LIFE", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_SIZE, WINDOW_SIZE, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	
	
	SDL_Surface* background_pic_surface = IMG_Load("head.png"); 
	if(!background_pic_surface)
	{
		fprintf(stderr, "Couldn't load head pic: %s\n", IMG_GetError());
		return -1;
	}
	SDL_Texture* background_pic_texture = SDL_CreateTextureFromSurface(renderer, background_pic_surface);
	
	uint8_t cell_grid[50][50];
	memset(cell_grid, 0, sizeof(uint8_t) * (50*50));

	SDL_Event e;

	uint8_t running = 1;
	while(running)
	{
		SDL_Delay(150);
		while(SDL_PollEvent(&e))
		{
			switch(e.type)
			{
				case SDL_KEYDOWN:
					switch(e.key.keysym.sym)
					{
						case SDLK_c:
							memset(cell_grid, 0, sizeof(uint8_t) * (50*50));
							break;
						
						case SDLK_f:
							compute_grid(cell_grid);
							break;

						case SDLK_ESCAPE:
							exit(0);
							break;
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
						click_cell(cell_grid, e.button.x, e.button.y);
						break;
				case SDL_QUIT:
					exit(0);
					break;
			}		
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		//ALL THE RENDERING STUFF
		SDL_RenderCopy(renderer, background_pic_texture, NULL, NULL);		
		render_grid(renderer);

		render_cell(renderer, cell_grid); 

		SDL_RenderPresent(renderer);


	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);


	IMG_Quit();
	SDL_Quit();


	return 0;
}

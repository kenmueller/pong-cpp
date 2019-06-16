#include "Game.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define PADDLE_HEIGHT 100
#define THICKNESS 15

Game::Game()
	: mWindow(nullptr)
	, mIsRunning(true)
	, mPaddlePos(Vector2{10, WINDOW_HEIGHT / 2.0f})
	, mBallPos(Vector2{WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f})
	, mBallVel(Vector2{-200, 235})
	, mTicksCount(0)
	, mPaddleDir(0) {}

bool Game::Initialize() {
	if (SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}
	mWindow = SDL_CreateWindow("Pong", 100, 100, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	if (!mWindow) {
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}
	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!mRenderer) {
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}
	return true;
}

void Game::RunLoop() {
	while (mIsRunning) {
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput() {
	SDL_Event event;
	while (SDL_PollEvent(&event))
		switch (event.type) {
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	mPaddleDir = 0;
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_ESCAPE])
		mIsRunning = false;
	if (state[SDL_SCANCODE_W])
		mPaddleDir--;
	if (state[SDL_SCANCODE_S])
		mPaddleDir++;
}

void Game::UpdateGame() {
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16.667f));
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	if (deltaTime > 0.05)
		deltaTime = 0.05;
	mTicksCount = SDL_GetTicks();
	if (mPaddleDir) {
		mPaddlePos.y += mPaddleDir * 300 * deltaTime;
		if (mPaddlePos.y < (PADDLE_HEIGHT / 2.0f + THICKNESS))
			mPaddlePos.y = PADDLE_HEIGHT / 2.0f + THICKNESS;
		else if (mPaddlePos.y > (WINDOW_HEIGHT - PADDLE_HEIGHT / 2.0f - THICKNESS))
			mPaddlePos.y = WINDOW_HEIGHT - PADDLE_HEIGHT / 2.0f - THICKNESS;
	}
}

void Game::GenerateOutput() {
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 255, 255);
	SDL_RenderClear(mRenderer);
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
	SDL_Rect wall{
		0,
		0,
		WINDOW_WIDTH,
		THICKNESS
	};
	SDL_RenderFillRect(mRenderer, &wall);
	wall.y = WINDOW_HEIGHT - THICKNESS;
	SDL_RenderFillRect(mRenderer, &wall);
	wall.x = WINDOW_WIDTH - THICKNESS;
	wall.y = 0;
	wall.w = THICKNESS;
	wall.h = WINDOW_WIDTH;
	SDL_RenderFillRect(mRenderer, &wall);
	SDL_Rect paddle{
		static_cast<int>(mPaddlePos.x),
		static_cast<int>(mPaddlePos.y - PADDLE_HEIGHT / 2.0f),
		THICKNESS,
		PADDLE_HEIGHT
	};
	SDL_RenderFillRect(mRenderer, &paddle);
	SDL_Rect ball{
		static_cast<int>(mBallPos.x - THICKNESS / 2.0f),
		static_cast<int>(mBallPos.y - THICKNESS / 2.0f),
		THICKNESS,
		THICKNESS
	};
	SDL_RenderFillRect(mRenderer, &ball);
	SDL_RenderPresent(mRenderer);
}

void Game::ShutDown() {}

int main(int argc, char **argv) {
	Game game;
	bool success = game.Initialize();
	if (success)
		game.RunLoop();
	game.ShutDown();
}

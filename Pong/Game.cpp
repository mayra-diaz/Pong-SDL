#include "Game.h"



Game::Game() {
	_pWindow = nullptr;
	_pRenderer = nullptr;
	_bIsRunning = true;
	_nTickCount = 0;
}


Game::~Game() {
}

// Initialize the game.
bool Game::Initialize() {
    // Initialize SDL.
    int sdlResult = SDL_Init(SDL_INIT_VIDEO);
    if (sdlResult != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }
    
    // Create the SDL window.
    _pWindow = SDL_CreateWindow("Pong", 100, 100, room_width, room_height, 0);
    if (_pWindow == nullptr) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }
    
    // Create the SDL renderer for all drawing.
    _pRenderer = SDL_CreateRenderer(_pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (_pRenderer == nullptr) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return false;
    }
    
    // Game specific initialization
    _paddleLeftDir = 0;
    _paddleRightDir = 0;
    _paddleHeight = 100;
    _paddleLeftPos = {10, room_height/2};
    _paddleRightPos = {room_width-(10+_THICKNESS), room_height/2};
    _ballPos = {room_width/2, room_height/2};
    _ballVelocity = {-240, 280};

	return true;
}

// Run the overall game loop.
void Game::RunLoop() {
	// Keep running the game until it is over.
	while (_bIsRunning) {
		_processInput();
		_updateGame();
		_generateOutput();
	}
}

// Shutdown the game.
void Game::Shutdown() {
	// Cleanup SDL.
	SDL_DestroyRenderer(_pRenderer);
	SDL_DestroyWindow(_pWindow);
	SDL_Quit();
}

void Game::_processInput() {
	// Poll for events in the window.
	// Needed to keep the window open.
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				_bIsRunning = false;
				break;
		}
	}

	// Get the entire state of the keyboard in an unsigned integer array.
	const Uint8* state = SDL_GetKeyboardState(NULL);

	// Check state of particular keys.
	if (state[SDL_SCANCODE_ESCAPE]) {
		_bIsRunning = false;
	}
    
    // Check for W/S keys to move left paddle
    _paddleLeftDir = 0;
    if (state[SDL_SCANCODE_W]){
        _paddleLeftDir += -1;
    }
    if (state[SDL_SCANCODE_S]){
        _paddleLeftDir += 1;
    }
    // Check for O/K keys to move left paddle
    _paddleRightDir = 0;
    if (state[SDL_SCANCODE_O]){
        _paddleRightDir += -1;
    }
    if (state[SDL_SCANCODE_K]){
        _paddleRightDir += 1;
    }
}

void Game::_updateGame() {
	// Wait until 16ms have elapsed.
	//while (!SDL_TICKS_PASSED(SDL_GetTicks(), _nTickCount + 16));

	// Delta time is the difference in ticks from the last frame
	// to this frame, converted to seconds.
	float deltaTime = (SDL_GetTicks() - _nTickCount) / 1000.f;

	_nTickCount = SDL_GetTicks();
    
    // Update paddle position based on direction
    //_updatePaddlePosition(&_paddleLeftPos, _paddleLeftDir);
    //_updatePaddlePosition(&_paddleRightPos, _paddleRightDir);
    
    if (_paddleLeftPos.y != 0){
        _paddleLeftPos.y += _paddleLeftDir * 300 * deltaTime;
        if (_paddleLeftPos.y <= _THICKNESS) {
            _paddleLeftPos.y = _THICKNESS;
        } else if (_paddleLeftPos.y >= room_height-(_paddleHeight+_THICKNESS)) {
            _paddleLeftPos.y = room_height-(_paddleHeight+_THICKNESS);
        }
    }
    
    if (_paddleRightPos.y != 0){
        _paddleRightPos.y += _paddleRightDir * 300 * deltaTime;
        if (_paddleRightPos.y <= _THICKNESS) {
            _paddleRightPos.y = _THICKNESS;
        } else if (_paddleRightPos.y >= room_height-(_paddleHeight+_THICKNESS)) {
            _paddleRightPos.y = room_height-(_paddleHeight+_THICKNESS);
        }
    }
    
    // Update ball position based on its velocity
    _ballPos.x += _ballVelocity.x * deltaTime;
    _ballPos.y += _ballVelocity.y * deltaTime;
    int spr_width = 2*_THICKNESS;
    
    // Check for ball vertical collision
    if (_ballPos.y >= room_height-spr_width && _ballVelocity.y > 0) {
        _ballVelocity.y *= -1;
    } else if (_ballPos.y<=spr_width && _ballVelocity.y < 0) {
        _ballVelocity.y *= -1;
    }
    
    // Check for ball collision with paddles
    bool diffLeft  = _paddleLeftPos.y <= _ballPos.y and _ballPos.y <= _paddleLeftPos.y+_paddleHeight;
    bool diffRight = _paddleRightPos.y <= _ballPos.y and _ballPos.y <= _paddleRightPos.y+_paddleHeight;
    
    if (diffLeft && _ballPos.x<=_paddleLeftPos.x+_THICKNESS && _ballPos.y>=_THICKNESS && _ballVelocity.x<0){
        _ballVelocity.x *= -1;
    } else if (diffRight && _ballPos.x>=_paddleRightPos.x-_THICKNESS && _ballPos.y<=room_width-_THICKNESS && _ballVelocity.x>0){
        _ballVelocity.x *= -1;
    }
    
    // Check for ball outside room
    if (_ballPos.x < _paddleLeftPos.x+_THICKNESS){
        rightScore++;
        // Change ball position
    } else if (_ballPos.x > _paddleRightPos.x){
        leftScore++;
    }
    
}

void Game::_generateOutput() {
	// Set background to blue.
	SDL_SetRenderDrawColor(_pRenderer, 1, 17, 97, 255);
    
    // Clear the back buffer to be ready to draw.
    SDL_RenderClear(_pRenderer);
    
    if (leftScore==3){
        SDL_Rect paddleLeft{_paddleLeftPos.x, _paddleLeftPos.y, _THICKNESS, _paddleHeight};
        SDL_RenderFillRect(_pRenderer, &paddleLeft);
    } else if (rightScore==3){
        SDL_Rect paddleRight{_paddleRightPos.x, _paddleRightPos.y, _THICKNESS, _paddleHeight};
        SDL_RenderFillRect(_pRenderer, &paddleRight);
    } else {
        // Draw top white wall
        SDL_SetRenderDrawColor(_pRenderer, 255, 255, 255, 255);
        SDL_Rect wall{0,0, room_width, _THICKNESS};
        SDL_RenderFillRect(_pRenderer, &wall);
        
        // Draw bottom white wall
        wall.y = room_height - _THICKNESS;
        SDL_RenderFillRect(_pRenderer, &wall);
        
        // Draw left white paddle
        SDL_Rect paddleLeft{_paddleLeftPos.x, _paddleLeftPos.y, _THICKNESS, _paddleHeight};
        SDL_RenderFillRect(_pRenderer, &paddleLeft);
        
        // Draw right white paddle
        SDL_Rect paddleRight{_paddleRightPos.x, _paddleRightPos.y, _THICKNESS, _paddleHeight};
        SDL_RenderFillRect(_pRenderer, &paddleRight);
        
        // Draw pong ball
        SDL_Rect ball{_ballPos.x, _ballPos.y, _THICKNESS, _THICKNESS};
        SDL_RenderFillRect(_pRenderer, &ball);
    }

	// Swap the front and back buffers.
	SDL_RenderPresent(_pRenderer);
}

void Game::_updatePaddlePosition(Game::Vector2 *paddlePos, int paddleDir) {
    float deltaTime = (SDL_GetTicks() - _nTickCount) / 1000.f;
    if (paddlePos->y != 0){
        paddlePos->y += paddleDir * 300 * deltaTime;
        if (paddlePos->y <= 2*_THICKNESS) {
            paddlePos->y = 2*_THICKNESS;
        } else if (paddlePos->y >= room_height-(_paddleHeight+2*_THICKNESS)) {
            paddlePos->y = room_height-(_paddleHeight+2*_THICKNESS);
        }
    }
}


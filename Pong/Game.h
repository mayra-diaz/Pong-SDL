#pragma once

#include "SDL2/SDL.h"

// Generic Game class.
class Game
{
public:
	struct Vector2 {
		int x;
		int y;
	};

	Game();
	~Game();

	// Initialize the game.
	bool Initialize();

	// Run the game loop until the game is over.
	void RunLoop();

	// Shutdown the game.
	void Shutdown();

private:
    
    // SDL window and renderer.
    SDL_Window* _pWindow;
    SDL_Renderer* _pRenderer;

    // Is the game still running.
    bool _bIsRunning;

    // Number of ticks since the start of the game.
    Uint32 _nTickCount;
    
    // Constants
    int _THICKNESS = 15;
    int room_height = 768;
    int room_width = 1024;
    
    // Score
    int leftScore = 0;
    int rightScore = 0;
    
    // Objects constants
    int _paddleHeight;
    int _paddleLeftDir;
    int _paddleRightDir;
    Vector2 _paddleLeftPos;
    Vector2 _paddleRightPos;
    Vector2 _ballPos;
    Vector2 _ballVelocity;
    
	void _processInput();

	void _updateGame();

	void _generateOutput();
    
    void _updatePaddlePosition(Vector2* paddlePos, int paddleDir);

};


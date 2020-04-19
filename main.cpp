#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include <SDL_mixer.h>

#include "Entity.h"
#include "Map.h"
#include "Util.h"	
#include "Scene.h"
#include "Menu.h" 
#include "Level1.h"
#include "Level2.h" 
#include "Level3.h"

/*
Joyce Huang
April 19th, 2020
CS-UY 3113 Intro to Game Programming
Project 5: Platfromer!
*/


SDL_Window * displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

Scene* currentScene;
Scene* sceneList[4];

bool startGame = false; 
int playerLives = 3; 

Mix_Music* music;
Mix_Chunk* jump;
Mix_Chunk* gameEnd;
Mix_Chunk* success;
Mix_Chunk* enemyDeath;

GLuint fontTextureID; 

void SwitchToScene(Scene* scene) {
	currentScene = scene;
	currentScene->Initialize();
}

void Initialize() {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	displayWindow = SDL_CreateWindow("Project 5: Platformer!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif

	glViewport(0, 0, 640, 480);

	program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	music = Mix_LoadMUS("atmosphere.mp3");
	Mix_PlayMusic(music, -1);
	Mix_VolumeMusic(MIX_MAX_VOLUME / 4);

	jump = Mix_LoadWAV("jump.wav");
	gameEnd = Mix_LoadWAV("gameover.wav");
	success = Mix_LoadWAV("success.wav");
	enemyDeath = Mix_LoadWAV("enemy_died.wav");

	Mix_VolumeChunk(jump, MIX_MAX_VOLUME / 4);
	Mix_VolumeChunk(gameEnd, MIX_MAX_VOLUME / 4);
	Mix_VolumeChunk(success, MIX_MAX_VOLUME / 4);
	Mix_VolumeChunk(enemyDeath, MIX_MAX_VOLUME / 4);

	viewMatrix = glm::mat4(1.0f);
	modelMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

	program.SetProjectionMatrix(projectionMatrix);
	program.SetViewMatrix(viewMatrix);

	glUseProgram(program.programID);

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	fontTextureID = Util::LoadTexture("font1.png");

	sceneList[0] = new Menu(); 
	sceneList[1] = new Level1();
	sceneList[2] = new Level2();
	sceneList[3] = new Level3();
	SwitchToScene(sceneList[0]);

}

void ProcessInput() {

	currentScene->state.player->movement = glm::vec3(0);

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
		case SDL_WINDOWEVENT_CLOSE:
			gameIsRunning = false;
			break;

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_LEFT:
				// Move the player left
				break;

			case SDLK_RIGHT:
				// Move the player right
				break;

			case SDLK_SPACE:
				if (currentScene->state.player->collidedBottom) {
					currentScene->state.player->jump = true;
					Mix_PlayChannel(-1, jump, 0);
				}
				break;
			break; // SDL_KEYDOWN
			} // switch (event.key.keysym.sym) {
		}  // switch (event.type)
	} // while loop

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	if (keys[SDL_SCANCODE_LEFT]) {
		currentScene->state.player->movement.x = -1.0f;
		currentScene->state.player->animIndices = currentScene->state.player->animLeft;
	}
	else if (keys[SDL_SCANCODE_RIGHT]) {
		currentScene->state.player->movement.x = 1.0f;
		currentScene->state.player->animIndices = currentScene->state.player->animRight;
	}

	if (keys[SDL_SCANCODE_RETURN] && (currentScene == sceneList[0])) {
		startGame = true; 
	} 

	if (glm::length(currentScene->state.player->movement) > 1.0f) {
		currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
	}

}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;
void Update() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float deltaTime = ticks - lastTicks;
	lastTicks = ticks;

	deltaTime += accumulator;
	if (deltaTime < FIXED_TIMESTEP) {
		accumulator = deltaTime;
		return;
	}

	while (deltaTime >= FIXED_TIMESTEP) {
		currentScene->Update(FIXED_TIMESTEP);
		deltaTime -= FIXED_TIMESTEP;
	}

	accumulator = deltaTime;

	viewMatrix = glm::mat4(1.0f);
	if (currentScene->state.player->position.x > 5) {  // scroll screen 
		viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 3.75, 0));
	}
	else { // do not scroll screen 
		viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
	}

	// check for when player collides with enemy (dead) and check for when enemy dies 

	if (currentScene->state.player->lastCollided == ENEMY) {
		if (currentScene->state.player->collidedLeft || currentScene->state.player->collidedRight ||
			currentScene->state.player->collidedTop) {

			playerLives -= 1;
			Mix_PlayChannel(-1, gameEnd, 0);
			currentScene->state.player->position = glm::vec3(1, 0, 0);
		}
		else if (currentScene->state.player->collidedBottom) {
			// enemy is no longer active 
			(currentScene->state.player->lastCollidedEntity)->isActive = false;
			currentScene->enemiesAlive -= 1;
			Mix_PlayChannel(-1, enemyDeath, 0);
			if (currentScene == sceneList[3] && currentScene->enemiesAlive == 0) {
				Mix_PlayChannel(-1, success, 0);
			}
		}
		currentScene->state.player->lastCollided = NONE;
	}

	currentScene->state.player->lives = playerLives;

	if (playerLives == 0) {
		currentScene->gameOver = true;
		currentScene->winner = false;
		currentScene->state.player->isActive = false;
	}
	
}

void Render() {
	glClear(GL_COLOR_BUFFER_BIT);

	program.SetViewMatrix(viewMatrix);

	currentScene->Render(&program);

	std::string text; 
	glm::vec3 textPosition = glm::vec3(0, 0, 0); 

	if (currentScene->gameOver) {

		if (currentScene->winner) {
			text = "Congratulations! You Win!";
		}
		else {
			text = "Game Over! You Lose!";
		}

		if (currentScene->state.player->position.x > 5) // scrolled screen 
			textPosition = glm::vec3((currentScene->state.player->position.x) - 3, -2, 0);

		else textPosition = glm::vec3(3, -2, 0);

		Util::DrawText(&program, fontTextureID, text, 0.5f, -0.25f, textPosition); 
	}

	SDL_GL_SwapWindow(displayWindow);
}


void Shutdown() {

	Mix_FreeChunk(jump);
	Mix_FreeChunk(gameEnd);
	Mix_FreeChunk(success);
	Mix_FreeChunk(enemyDeath);
	Mix_FreeMusic(music);

	SDL_Quit();
}

int main(int argc, char* argv[]) {
	Initialize();

	while (gameIsRunning) {
		ProcessInput();
		Update();

		if (currentScene->state.nextScene >= 0) SwitchToScene(sceneList[currentScene->state.nextScene]);
		else if (startGame) {
			startGame = false; 
			SwitchToScene(sceneList[1]);
		}

		Render();
	}

	Shutdown();
	return 0;
}
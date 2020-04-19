#include "Level3.h"
#include <SDL_mixer.h>

#define LEVEL3_ENEMY_COUNT 3
#define LEVEL3_ITEM_COUNT 2 

#define LEVEL3_WIDTH 22
#define LEVEL3_HEIGHT 8

unsigned int level3_data[] =
{
	 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 3, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1,
	 3, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1, 1, 1, 2, 2, 2,
	 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
};

void Level3::Initialize() {

	state.nextScene = -1;

	GLuint mapTextureID = Util::LoadTexture("tileset.png");
	state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 4, 1);

	// Move over all of the player and enemy code from initialization.

	// Initialize Game Objects

	// Initialize Player
	state.player = new Entity();
	state.player->entityType = PLAYER;
	state.player->position = glm::vec3(1, 0, 0);
	state.player->movement = glm::vec3(0);
	state.player->acceleration = glm::vec3(0, -9.81f, 0);
	state.player->speed = 2.0f;
	state.player->lives = 3;
	state.player->textureID = Util::LoadTexture("george_0.png");

	state.player->animRight = new int[4]{ 3, 7, 11, 15 };
	state.player->animLeft = new int[4]{ 1, 5, 9, 13 };
	state.player->animUp = new int[4]{ 2, 6, 10, 14 };
	state.player->animDown = new int[4]{ 0, 4, 8, 12 };

	state.player->animIndices = state.player->animRight;
	state.player->animFrames = 4;
	state.player->animIndex = 0;
	state.player->animTime = 0;
	state.player->animCols = 4;
	state.player->animRows = 4;

	state.player->height = 0.8f;
	state.player->width = 0.8f;

	state.player->jumpPower = 6.0f;

	// enemies 
	state.enemies = new Entity[LEVEL3_ENEMY_COUNT];
	GLuint birdTextureID = Util::LoadTexture("FatBird_Idle.png");

	for (int i = 0; i < LEVEL3_ENEMY_COUNT; ++i) {

		state.enemies[i].entityType = ENEMY;
		state.enemies[i].textureID = birdTextureID;
		
		state.enemies[i].speed = 1.0f;
		state.enemies[i].aiType = UPANDDOWN;

		state.enemies[i].animIndices = new int[8]{ 0, 1, 2, 3, 4, 5, 6, 7 };
		state.enemies[i].animIndex = 0;
		state.enemies[i].animFrames = 8;
		state.enemies[i].animTime = 0;
		state.enemies[i].animCols = 8;
		state.enemies[i].animRows = 1;
	}

	state.enemies[0].position = glm::vec3(5, -1.5f, 0);
	state.enemies[1].position = glm::vec3(11, -5, 0);
	state.enemies[2].position = glm::vec3(17, -1.5f, 0);

	enemiesAlive = LEVEL3_ENEMY_COUNT;

	// items 
	state.items = new Entity[LEVEL3_ITEM_COUNT];
	GLuint arrowTextureID = Util::LoadTexture("right_arrow.png");

	for (int i = 0; i < LEVEL3_ITEM_COUNT; ++i) {
		state.items[i].textureID = arrowTextureID;
		state.items[i].entityType = ITEM;
	}
	state.items[0].position = glm::vec3(8, -4, 0);
	state.items[1].position = glm::vec3(14, -4, 0);

}

void Level3::Update(float deltaTime) {
	state.player->Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);

	for (int i = 0; i < LEVEL3_ENEMY_COUNT; ++i) {
		state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);
	}

	for (int i = 0; i < LEVEL3_ITEM_COUNT; ++i) {
		state.items[i].Update(deltaTime, state.player, state.items, LEVEL3_ITEM_COUNT, state.map); 
	}

	if (enemiesAlive == 0) {
		gameOver = true; 
		winner = true; 
		// state.nextScene = 0;
	}

}
void Level3::Render(ShaderProgram* program) {
	state.map->Render(program);

	GLuint fontTextureID = Util::LoadTexture("font1.png");

	glm::vec3 levelPosition = glm::vec3(0, 0, 0);
	glm::vec3 livesPosition = glm::vec3(0, 0, 0);

	if (state.player->position.x > 5) {  // if the screen is scrolling 
		livesPosition = glm::vec3(state.player->position.x + 3, -0.5f, 0);
		levelPosition = glm::vec3(state.player->position.x - 4.6f, -0.5f, 0);
	}
	else { // if screen not scrolling 
		livesPosition = glm::vec3(8, -0.5f, 0);
		levelPosition = glm::vec3(1, -0.5f, 0);
	}

	Util::DrawText(program, fontTextureID, "Lives: " + std::to_string(state.player->lives), 0.45f, -0.25f, livesPosition);
	Util::DrawText(program, fontTextureID, "Level 3", 0.45f, -0.25f, levelPosition);

	for (int i = 0; i < LEVEL3_ITEM_COUNT; ++i) {
		state.items[i].Render(program); 
	}

	state.player->Render(program);

	for (int i = 0; i < LEVEL3_ENEMY_COUNT; ++i) {
		state.enemies[i].Render(program);
	}

}

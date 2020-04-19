#include "Menu.h"

#define ENEMY_COUNT 1

#define MENU_WIDTH 14
#define MENU_HEIGHT 8

unsigned int menu_data[] =
{
	 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

void Menu::Initialize() {

	state.nextScene = -1;

	GLuint mapTextureID = Util::LoadTexture("tileset.png");
	state.map = new Map(MENU_WIDTH, MENU_HEIGHT, menu_data, mapTextureID, 1.0f, 4, 1);

	// Move over all of the player and enemy code from initialization.

		// Initialize Game Objects

	// Initialize Player
	state.player = new Entity();
	state.player->entityType = PLAYER;
	state.player->position = glm::vec3(7, 0, 0);
	state.player->movement = glm::vec3(0);
	state.player->acceleration = glm::vec3(0, -9.81f, 0);
	state.player->speed = 2.0f;
	state.player->lives = 3; 
	state.player->textureID = Util::LoadTexture("george_0.png");

	state.player->animRight = new int[4]{ 3, 7, 11, 15 };
	state.player->animLeft = new int[4]{ 1, 5, 9, 13 };
	state.player->animUp = new int[4]{ 2, 6, 10, 14 };
	state.player->animDown = new int[4]{ 0, 4, 8, 12 };

	state.player->animIndices = state.player->animDown;
	state.player->animFrames = 4;
	state.player->animIndex = 0;
	state.player->animTime = 0;
	state.player->animCols = 4;
	state.player->animRows = 4;

	state.player->height = 0.8f;
	state.player->width = 0.8f;

	state.player->jumpPower = 6.0f;

	// enemies 
	state.enemies = new Entity[ENEMY_COUNT];
	GLuint enemyTextureID = Util::LoadTexture("ctg.png");

	state.enemies[0].entityType = ENEMY;
	state.enemies[0].textureID = enemyTextureID;
	state.enemies[0].position = glm::vec3(4, -2.25, 0);
	state.enemies[0].speed = 1;
	state.enemies[0].aiType = WAITANDGO;
	state.enemies[0].aiState = IDLE;
	state.enemies[0].isActive = false;

}
void Menu::Update(float deltaTime) {
	state.player->Update(deltaTime, state.player, state.enemies, ENEMY_COUNT, state.map);
}
void Menu::Render(ShaderProgram* program) {

	state.map->Render(program);

	GLuint fontTextureID = Util::LoadTexture("font1.png");
	Util::DrawText(program, fontTextureID, "The Adventures of George!", 0.5f, -0.25f, glm::vec3(4, -2, 0));
	Util::DrawText(program, fontTextureID, "Press Enter to start!", 0.45f, -0.25f, glm::vec3(5.0f, -6.5f, 0));

	state.player->Render(program);
}

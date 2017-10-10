#include <Arduino.h>
#include <Arduboy2.h>

#define randBetween(min, max) ((rand() % (max - min)) + min)

Arduboy2 arduboy;

const unsigned int SCREEN_WIDTH = 128;
const unsigned int SCREEN_HEIGHT = 64;
const unsigned int PLAYER_SPEED = 2;
const unsigned int MAX_STARS = 20;
const unsigned int MAX_BULLETS = 5;
const unsigned int MAX_ENEMIES = 6;
const unsigned int BULLET_WIDTH = 3;
const unsigned int BULLET_SPEED = 3;
const unsigned int ENEMY_SPEED = 1;

typedef struct {
    int x;
    int y;
    int size;
    int speed;
} Star;

typedef struct {
    unsigned int x, y, width, height;
} Player;

typedef struct {
    int x;
    int y;
    int dirY;
    int isAlive;
    int width;
    int height;
} Enemy;

typedef struct {
    int x;
    int y;
    int startX;
    int isActive;
} Bullet;

const unsigned char PROGMEM playerSprite[] = {
    0x3f, 0x3e, 0x3c, 0x34, 0x34, 0x3c, 0x38, 0x30,
};

const unsigned char PROGMEM enemySprite[] = {
    0xe0, 0x3f, 0xf4, 0x3c, 0x3c, 0xf4, 0x3f, 0xe0,
};

Star stars[MAX_STARS];
Player player;
Bullet bullets[MAX_BULLETS];
Enemy enemies[MAX_ENEMIES];

void setup() {

    arduboy.boot();
    arduboy.setFrameRate(60);

    // Starfield background init

    for (int i = 0; i < MAX_STARS; i++) {
        stars[i].x = randBetween(1, SCREEN_WIDTH);
        stars[i].y = randBetween(1, SCREEN_HEIGHT);
        stars[i].speed = randBetween(1, 4);
        stars[i].size = 1;
    }

    // Player init

    player.y = SCREEN_HEIGHT * .5 - player.height * .5;
    player.width = 8;
    player.height = 6;

}

void drawStarfield() {

    for (int i = 0; i < MAX_STARS; i++) {

        stars[i].x -= stars[i].speed;

        arduboy.fillRect(stars[i].x, stars[i].y, stars[i].size, stars[i].size);

        if (stars[i].x - stars[i].size < 0) {
            stars[i].x = SCREEN_WIDTH;
            stars[i].y = randBetween(1, SCREEN_HEIGHT);
        }

    }

}

void drawPlayer() {
    arduboy.drawBitmap(player.x, player.y, playerSprite, 8, 8, WHITE);
}

void drawBullets() {

    for (int i = 0; i < MAX_BULLETS; i++) {

        if (bullets[i].isActive) {
            arduboy.fillRect(bullets[i].x, bullets[i].y, BULLET_WIDTH, 1);
        }

    }

}

void drawEnemies() {

    for (int i = 0; i < MAX_ENEMIES; i++) {

        if (enemies[i].isAlive) {
            arduboy.drawBitmap(enemies[i].x, enemies[i].y, enemySprite, 8, 8, WHITE);
        }

    }

}

void fireBullet() {

    for (int i = 0; i < MAX_BULLETS; i++) {

        if (!bullets[i].isActive) {
            bullets[i].isActive = 1;
            bullets[i].x = player.x + player.width - BULLET_SPEED;
            bullets[i].startX = bullets[i].x;
            bullets[i].y = player.y + player.height - 1;
            break;
        }

    }

}

void handleInputs() {

    if (arduboy.pressed(RIGHT_BUTTON) && player.x + player.width < SCREEN_WIDTH) {
        player.x += PLAYER_SPEED;
    }

    if (arduboy.pressed(LEFT_BUTTON) && player.x > 0) {
        player.x -= PLAYER_SPEED;
    }

    if (arduboy.pressed(UP_BUTTON) && player.y > 0) {
        player.y -= PLAYER_SPEED;
    }

    if (arduboy.pressed(DOWN_BUTTON) && player.y + player.height < SCREEN_HEIGHT) {
        player.y += PLAYER_SPEED;
    }

    if (arduboy.justPressed(B_BUTTON)) {
        fireBullet();
    }

}

void updateBullets() {

    for (int i = 0; i < MAX_BULLETS; i++) {

        if (bullets[i].isActive) {

            if (bullets[i].x > SCREEN_WIDTH) {
                bullets[i].isActive = 0;
                continue;
            }

            bullets[i].x += BULLET_SPEED;

        }

    }

}

void updateEnemies() {

    for (int i = 0; i < MAX_ENEMIES; i++) {

        if (enemies[i].isAlive) {

            if (enemies[i].x < -enemies[i].width) {
                enemies[i].isAlive = 0;
                continue;
            }

            if (enemies[i].y < 0 || enemies[i].y > SCREEN_HEIGHT - enemies[i].height) {
                enemies[i].dirY = -enemies[i].dirY;
            }

            enemies[i].x -= ENEMY_SPEED;
            enemies[i].y += enemies[i].dirY * ENEMY_SPEED;

        } else {
            enemies[i].isAlive = 1;
            enemies[i].dirY = 1;
            enemies[i].width = 8;
            enemies[i].height = 8;
            enemies[i].x = randBetween(SCREEN_WIDTH, SCREEN_WIDTH * 2);
            enemies[i].y = randBetween(1, SCREEN_HEIGHT - enemies[i].height);
            //enemies[i].x = randBetween(1, SCREEN_WIDTH - enemies[i].width);
            //enemies[i].y = randBetween(1, SCREEN_HEIGHT - enemies[i].height);
        }

    }

}

void checkCollisions() {

    for (int i = 0; i < MAX_BULLETS; i++) {

        if (!bullets[i].isActive) {
            continue;
        }

        for (int j = 0; j < MAX_ENEMIES; j++) {

            if (!enemies[j].isAlive) {
                continue;
            }

            if (bullets[i].x < enemies[j].x + enemies[j].width &&
                bullets[i].x + BULLET_WIDTH + BULLET_SPEED > enemies[j].x &&
                bullets[i].y < enemies[j].y + enemies[j].height &&
                bullets[i].y + 1 > enemies[j].y) {

                    bullets[i].isActive = 0;
                    enemies[j].isAlive = 0;

            }

        }

    }
}

void update() {
    arduboy.pollButtons();
    handleInputs();
    updateBullets();
    updateEnemies();
    checkCollisions();
}

void draw() {
    arduboy.clear();
    drawStarfield();
    drawPlayer();
    drawBullets();
    drawEnemies();
    arduboy.display();
}

void loop() {

    if (!(arduboy.nextFrame())) {
        return;
    }

    update();
    draw();

}

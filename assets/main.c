#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

int check = 0;

#define DIFFICULTY_EASY 0
#define DIFFICULTY_MEDIUM 1
#define DIFFICULTY_HARD 2
#define DIFFICULTY_IMPOSSIBLE 3
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define TILE_SIZE 20
#define MAP_WIDTH 30
#define MAP_HEIGHT 20
#define MAX_GHOSTS 5
#define GHOST_MOVE_DELAY 30
#define PACMAN_MOVE_DELAY 12
#define SPEED_BOOST_DURATION 300
#define MAX_APPLES 2
#define MAX_PEPPERS 2
#define MAX_CHERRIES 1
#define MAX_MUSHROOMS 1

#define DIRECTION_UP 0
#define DIRECTION_DOWN 1
#define DIRECTION_LEFT 2
#define DIRECTION_RIGHT 3

Texture2D pacmanOpenDown;
Texture2D pacmanOpenUp;
Texture2D pacmanOpenLeft;
Texture2D pacmanOpen;
Texture2D pacmanClose;

Texture2D ghostTextures[MAX_GHOSTS][4];
Texture2D followingGhostTexture;
Texture2D appleTexture;
Texture2D pepperTexture;
Texture2D cherryTexture;
Texture2D mushroomTexture;
Texture2D starTexture;

char map[MAP_HEIGHT][MAP_WIDTH];
char originalMap[MAP_HEIGHT][MAP_WIDTH];
int highScore = 0;
int latestScore = 0;
int lifeCount = 1;

int isSpeedBoosted = 0;
int speedBoostCounter = 0;

int canEatGhosts = 0;
int difficult = 0;

Sound eatPelletSound;
Sound eatGhostSound;
Sound eatAppleSound;
Sound eatPepperSound;
Sound eatCherrySound;
Sound eatMushroomSound;
Sound gameOverSound;
Sound backgroundMusic;

Color mapp = DARKGRAY;

const char *mapTemplates[4] = {
    "##############################\n"
    "#............................#\n"
    "#.######.#######.######.######\n"
    "#.#......#.......#......#....#\n"
    "#.####.#.#####.#.####.#.####.#\n"
    "#....#.#.......#.#....#.....##\n"
    "#.##.#.#.#####.#.#.##.#.####.#\n"
    "#....#............#..........#\n"
    "#.##.#.#.#.###.#.#.#.#.##.#..#\n"
    "#......#.#.#...#.#.#.#.##.#..#\n"
    "#.##.#.#.#.#...#.#.#.#......##\n"
    "#.##.#.#.#.#######.#.#.#####.#\n"
    "#....#.............#.........#\n"
    "#.##.#.#.#####.#.#.##.#.######\n"
    "#....#.#.......#.#....#.....##\n"
    "#.####.#.#####.#.####.#.######\n"
    "#.#......#.......#...........#\n"
    "#.######.#######.######.######\n"
    "#............................#\n"
    "##############################",

    "##############################\n"
    "#............................#\n"
    "#.##########.##########.######\n"
    "#.#.......................#..#\n"
    "#.#.##########.##########.#..#\n"
    "#.#.......................#..#\n"
    "#.##########################.#\n"
    "#............................#\n"
    "#.##########.##########.######\n"
    "#.#.......................#..#\n"
    "#.#.##########.##########.#..#\n"
    "#.#.......................#..#\n"
    "#.##########################.#\n"
    "#............................#\n"
    "#.##########.##########.######\n"
    "#.#..............#...........#\n"
    "#.#.##########.####..####.#..#\n"
    "#.#.......................#..#\n"
    "#.##########################.#\n"
    "##############################",

    "##############################\n"
    "#.......##......##...##...####\n"
    "#...#.#.##.#....##.#....#..###\n"
    "#.#.#...##.#.##.##...##.#...##\n"
    "#...#.#.##.#.##.##...##.###.##\n"
    "#.#.#...##...##....#.##.###.##\n"
    "#...#.#.##...##.##.#....###.##\n"
    "#.#.#.#.##.#............##..##\n"
    "#...#...##.#.##.##.#.##.##..##\n"
    "#.#.#.#.##...##....#.##.##..##\n"
    "#.#.#.#.##.#.##.##.#.##.....##\n"
    "#.#.#......#....##.#.##.###.##\n"
    "#.#.#...##.#.##.##.#.##.....##\n"
    "#.#...#.##...##.##.#.##.###.##\n"
    "#.#.#....#.#.##.##.#........##\n"
    "#...####.#...##.##.#...####.##\n"
    "#.#......#.#.##...##...####.##\n"
    "#.#.####.#...##.####........##\n"
    "#..........#.##........#######\n"
    "##############################",

    "##############################\n"
    "#............................#\n"
    "#.######.###########.######.##\n"
    "#.#.......................#..#\n"
    "#.#.######.#####.######.#.#..#\n"
    "#.#......#.....#......#.#.#..#\n"
    "#.######.#.###.#.######.#.#..#\n"
    "#........#.....#........#.#..#\n"
    "######.###########.######.#..#\n"
    "#........................#...#\n"
    "#.######.#####.######.######.#\n"
    "#.#......#.....#......#......#\n"
    "#.#.######.###.#.######.######\n"
    "#.#.......................#..#\n"
    "#.######.###########.######.##\n"
    "#............................#\n"
    "###...#######..###############\n"
    "#............................#\n"
    "#..#.#.#.#.#.#.#.#.#.#..#.#.##\n"
    "##############################"
};

int SelectDifficulty() {
    int selection = 0;
    const char* options[] = {"Easy", "Medium", "Hard", "AmirKabir Mode"};
    const int optionCount = sizeof(options) / sizeof(options[0]);
    const char* itemCounts[] = {
        "Apples: 3, Peppers: 2, Cherries: 2, Mushrooms: 0",
        "Apples: 2, Peppers: 2, Cherries: 1, Mushrooms: 1",
        "Apples: 1, Peppers: 0, Cherries: 0, Mushrooms: 4",
        "RUN AWAY!!!"
    };

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("Select Difficulty", SCREEN_WIDTH / 2 - MeasureText("Select Difficulty", 40) / 2, SCREEN_HEIGHT / 2 - 200, 40, YELLOW);

        for (int i = 0; i < optionCount; i++) {
            Color color = (i == selection) ? YELLOW : WHITE;
            DrawText(options[i], SCREEN_WIDTH / 2 - MeasureText(options[i], 20) / 2, SCREEN_HEIGHT / 2 - 100 + i * 40, 20, color);

            if (i == selection) {
                DrawText(itemCounts[i], SCREEN_WIDTH / 2 - MeasureText(itemCounts[i], 20) / 2, SCREEN_HEIGHT / 2 + 200, 20, color);
            }
        }

        EndDrawing();

        if (IsKeyPressed(KEY_UP)) selection--;
        else if (IsKeyPressed(KEY_DOWN)) selection++;

        if (selection < 0) selection += optionCount;
        if (selection >= optionCount) selection -= optionCount;

        if (IsKeyPressed(KEY_ENTER)) {
            return selection;
        }
    }

    return -1;
}

int remainingDots = 10;

void LoadRandomMap(int difficulty) {
    srand(time(NULL));
    int random_number = rand() % 4;
    const char *selectedMap = mapTemplates[random_number];

    remainingDots = 10;

    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            map[i][j] = selectedMap[i * (MAP_WIDTH + 1) + j];
            originalMap[i][j] = map[i][j];
        }
    }

    int numApples, numPeppers, numCherries, numMushrooms;
    switch (difficulty) {
        case DIFFICULTY_EASY:
            difficult = 0;
            numApples = 3;
            numPeppers = 2;
            numCherries = 2;
            numMushrooms = 0;
            break;
        case DIFFICULTY_MEDIUM:
            difficult = 1;
            numApples = 2;
            numPeppers = 2;
            numCherries = 1;
            numMushrooms = 1;
            break;
        case DIFFICULTY_HARD:
            difficult = 2;
            numApples = 1;
            numPeppers = 0;
            numCherries = 0;
            numMushrooms = 4;
            break;
        case DIFFICULTY_IMPOSSIBLE:
            return;
        default:
            numApples = 2;
            numPeppers = 2;
            numCherries = 1;
            numMushrooms = 1;
            break;
    }

    for (int i = 0; i < numApples; i++) {
        int x, y;
        do {
            x = rand() % MAP_WIDTH;
            y = rand() % MAP_HEIGHT;
        } while (map[y][x] != '.');

        map[y][x] = 'A';
        originalMap[y][x] = 'A';
    }

    for (int i = 0; i < numPeppers; i++) {
        int x, y;
        do {
            x = rand() % MAP_WIDTH;
            y = rand() % MAP_HEIGHT;
        } while (map[y][x] != '.');

        map[y][x] = 'E';
        originalMap[y][x] = 'E';
    }

    for (int i = 0; i < numCherries; i++) {
        int x, y;
        do {
            x = rand() % MAP_WIDTH;
            y = rand() % MAP_HEIGHT;
        } while (map[y][x] != '.');

        map[y][x] = 'C';
        originalMap[y][x] = 'C';
    }

    for (int i = 0; i < numMushrooms; i++) {
        int x, y;
        do {
            x = rand() % MAP_WIDTH;
            y = rand() % MAP_HEIGHT;
        } while (map[y][x] != '.');

        map[y][x] = 'M';
        originalMap[y][x] = 'M';
    }

    for (int i = 0; i < 10; i++) {
        int x, y;
        do {
            x = rand() % MAP_WIDTH;
            y = rand() % MAP_HEIGHT;
        } while (map[y][x] != '.');

        map[y][x] = 'S';
        originalMap[y][x] = 'S';
    }

    for (int i = 0; i < MAX_GHOSTS; i++) {
        int x, y;
        do {
            x = rand() % MAP_WIDTH;
            y = rand() % MAP_HEIGHT;
        } while (map[y][x] != '.');

        map[y][x] = 'G';
        originalMap[y][x] = 'G';
    }
}

typedef struct {
    int x;
    int y;
} Position;

Position ghosts[MAX_GHOSTS];
Position pacman = {1, 1};

int animCounter = 0;
int score = 0;
int pacmanDirection = DIRECTION_RIGHT;
int pacmanMoveCounter = 0;
int ghostDirections[MAX_GHOSTS];

void LoadTextures() {
    Image pacmanOpenImage = LoadImage("../pacmanopen.png");
    Image pacmanCloseImage = LoadImage("../pacmanclose.png");
    Image pacmanOpenDownImage = LoadImage("../pacmanopen_down.png");
    Image pacmanOpenUpImage = LoadImage("../pacmanopen_up.png");
    Image pacmanOpenLeftImage = LoadImage("../pacmanopen_left.png");

    ImageResizeNN(&pacmanOpenImage, TILE_SIZE, TILE_SIZE);
    ImageResizeNN(&pacmanCloseImage, TILE_SIZE, TILE_SIZE);
    ImageResizeNN(&pacmanOpenDownImage, TILE_SIZE, TILE_SIZE);
    ImageResizeNN(&pacmanOpenUpImage, TILE_SIZE, TILE_SIZE);
    ImageResizeNN(&pacmanOpenLeftImage, TILE_SIZE, TILE_SIZE);

    pacmanOpen = LoadTextureFromImage(pacmanOpenImage);
    pacmanClose = LoadTextureFromImage(pacmanCloseImage);
    pacmanOpenDown = LoadTextureFromImage(pacmanOpenDownImage);
    pacmanOpenUp = LoadTextureFromImage(pacmanOpenUpImage);
    pacmanOpenLeft = LoadTextureFromImage(pacmanOpenLeftImage);

    Image appleImage = LoadImage("../apple.png");
    ImageResizeNN(&appleImage, TILE_SIZE, TILE_SIZE);
    appleTexture = LoadTextureFromImage(appleImage);
    UnloadImage(appleImage);

    Image pepperImage = LoadImage("../pepper.png");
    ImageResizeNN(&pepperImage, TILE_SIZE, TILE_SIZE);
    pepperTexture = LoadTextureFromImage(pepperImage);
    UnloadImage(pepperImage);

    Image cherryImage = LoadImage("../cherry.png");
    ImageResizeNN(&cherryImage, TILE_SIZE, TILE_SIZE);
    cherryTexture = LoadTextureFromImage(cherryImage);
    UnloadImage(cherryImage);

    Image mushroomImage = LoadImage("../mushroom.png");
    ImageResizeNN(&mushroomImage, TILE_SIZE, TILE_SIZE);
    mushroomTexture = LoadTextureFromImage(mushroomImage);
    UnloadImage(mushroomImage);

    Image starImage = LoadImage("../star.png");
    ImageResizeNN(&starImage, TILE_SIZE, TILE_SIZE);
    starTexture = LoadTextureFromImage(starImage);
    UnloadImage(starImage);

    UnloadImage(pacmanOpenImage);
    UnloadImage(pacmanCloseImage);
    UnloadImage(pacmanOpenDownImage);
    UnloadImage(pacmanOpenUpImage);
    UnloadImage(pacmanOpenLeftImage);
}

void LoadGhostTextures() {
    Image ghostBlueUp = LoadImage("../ghost_blue_up.png");
    Image ghostBlueDown = LoadImage("../ghost_blue_down.png");
    Image ghostBlueLeft = LoadImage("../ghost_blue_left.png");
    Image ghostBlueRight = LoadImage("../ghost_blue_right.png");

    Image ghostPinkUp = LoadImage("../ghost_pink_up.png");
    Image ghostPinkDown = LoadImage("../ghost_pink_down.png");
    Image ghostPinkLeft = LoadImage("../ghost_pink_left.png");
    Image ghostPinkRight = LoadImage("../ghost_pink_right.png");

    Image ghostYellowUp = LoadImage("../ghost_yellow_up.png");
    Image ghostYellowDown = LoadImage("../ghost_yellow_down.png");
    Image ghostYellowLeft = LoadImage("../ghost_yellow_left.png");
    Image ghostYellowRight = LoadImage("../ghost_yellow_right.png");

    Image ghostRedUp = LoadImage("../ghost_red_up.png");
    Image ghostRedDown = LoadImage("../ghost_red_down.png");
    Image ghostRedLeft = LoadImage("../ghost_red_left.png");
    Image ghostRedRight = LoadImage("../ghost_red_right.png");

    Image followingGhostImage = LoadImage("../following_ghost.png");

    ImageResizeNN(&ghostBlueUp, TILE_SIZE, TILE_SIZE);
    ImageResizeNN(&ghostBlueDown, TILE_SIZE, TILE_SIZE);
    ImageResizeNN(&ghostBlueLeft, TILE_SIZE, TILE_SIZE);
    ImageResizeNN(&ghostBlueRight, TILE_SIZE, TILE_SIZE);

    ImageResizeNN(&ghostPinkUp, TILE_SIZE, TILE_SIZE);
    ImageResizeNN(&ghostPinkDown, TILE_SIZE, TILE_SIZE);
    ImageResizeNN(&ghostPinkLeft, TILE_SIZE, TILE_SIZE);
    ImageResizeNN(&ghostPinkRight, TILE_SIZE, TILE_SIZE);

    ImageResizeNN(&ghostYellowUp, TILE_SIZE, TILE_SIZE);
    ImageResizeNN(&ghostYellowDown, TILE_SIZE, TILE_SIZE);
    ImageResizeNN(&ghostYellowLeft, TILE_SIZE, TILE_SIZE);
    ImageResizeNN(&ghostYellowRight, TILE_SIZE, TILE_SIZE);

    ImageResizeNN(&ghostRedUp, TILE_SIZE, TILE_SIZE);
    ImageResizeNN(&ghostRedDown, TILE_SIZE, TILE_SIZE);
    ImageResizeNN(&ghostRedLeft, TILE_SIZE, TILE_SIZE);
    ImageResizeNN(&ghostRedRight, TILE_SIZE, TILE_SIZE);

    ImageResizeNN(&followingGhostImage, TILE_SIZE, TILE_SIZE);

    ghostTextures[0][DIRECTION_UP] = LoadTextureFromImage(ghostBlueUp);
    ghostTextures[0][DIRECTION_DOWN] = LoadTextureFromImage(ghostBlueDown);
    ghostTextures[0][DIRECTION_LEFT] = LoadTextureFromImage(ghostBlueLeft);
    ghostTextures[0][DIRECTION_RIGHT] = LoadTextureFromImage(ghostBlueRight);

    ghostTextures[1][DIRECTION_UP] = LoadTextureFromImage(ghostPinkUp);
    ghostTextures[1][DIRECTION_DOWN] = LoadTextureFromImage(ghostPinkDown);
    ghostTextures[1][DIRECTION_LEFT] = LoadTextureFromImage(ghostPinkLeft);
    ghostTextures[1][DIRECTION_RIGHT] = LoadTextureFromImage(ghostPinkRight);

    ghostTextures[2][DIRECTION_UP] = LoadTextureFromImage(ghostYellowUp);
    ghostTextures[2][DIRECTION_DOWN] = LoadTextureFromImage(ghostYellowDown);
    ghostTextures[2][DIRECTION_LEFT] = LoadTextureFromImage(ghostYellowLeft);
    ghostTextures[2][DIRECTION_RIGHT] = LoadTextureFromImage(ghostYellowRight);

    ghostTextures[3][DIRECTION_UP] = LoadTextureFromImage(ghostRedUp);
    ghostTextures[3][DIRECTION_DOWN] = LoadTextureFromImage(ghostRedDown);
    ghostTextures[3][DIRECTION_LEFT] = LoadTextureFromImage(ghostRedLeft);
    ghostTextures[3][DIRECTION_RIGHT] = LoadTextureFromImage(ghostRedRight);

    ghostTextures[4][DIRECTION_UP] = LoadTextureFromImage(ghostRedUp);
    ghostTextures[4][DIRECTION_DOWN] = LoadTextureFromImage(ghostRedDown);
    ghostTextures[4][DIRECTION_LEFT] = LoadTextureFromImage(ghostRedLeft);
    ghostTextures[4][DIRECTION_RIGHT] = LoadTextureFromImage(ghostRedRight);

    followingGhostTexture = LoadTextureFromImage(followingGhostImage);

    UnloadImage(ghostBlueUp);
    UnloadImage(ghostBlueDown);
    UnloadImage(ghostBlueLeft);
    UnloadImage(ghostBlueRight);

    UnloadImage(ghostPinkUp);
    UnloadImage(ghostPinkDown);
    UnloadImage(ghostPinkLeft);
    UnloadImage(ghostPinkRight);

    UnloadImage(ghostYellowUp);
    UnloadImage(ghostYellowDown);
    UnloadImage(ghostYellowLeft);
    UnloadImage(ghostYellowRight);

    UnloadImage(ghostRedUp);
    UnloadImage(ghostRedDown);
    UnloadImage(ghostRedLeft);
    UnloadImage(ghostRedRight);

    UnloadImage(followingGhostImage);
}

void LoadSounds() {
    eatPelletSound = LoadSound("../dot.wav");
    eatGhostSound = LoadSound("../eat_ghost.wav");
    eatAppleSound = LoadSound("../apple_eating.wav");
    eatPepperSound = LoadSound("../speed.wav");
    eatCherrySound = LoadSound("../cherry.wav");
    eatMushroomSound = LoadSound("../alert.wav");
    gameOverSound = LoadSound("../gameoverr.wav");
    backgroundMusic = LoadSound("../background2.wav");
}

void UnloadSounds() {
    UnloadSound(eatPelletSound);
    UnloadSound(eatGhostSound);
    UnloadSound(eatAppleSound);
    UnloadSound(eatPepperSound);
    UnloadSound(eatCherrySound);
    UnloadSound(eatMushroomSound);
    UnloadSound(gameOverSound);
    UnloadSound(backgroundMusic);
}

void DrawMap(int offsetX, int offsetY) {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            switch (map[i][j]) {
                case '#':
                    DrawRectangle(offsetX + j * TILE_SIZE, offsetY + i * TILE_SIZE, TILE_SIZE, TILE_SIZE, mapp);
                    break;
                case 'P':
                    if (animCounter < 15) {
                        DrawTexture(pacmanClose, offsetX + j * TILE_SIZE, offsetY + i * TILE_SIZE, WHITE);
                    } else {
                        switch (pacmanDirection) {
                            case DIRECTION_UP:
                                DrawTexture(pacmanOpenUp, offsetX + j * TILE_SIZE, offsetY + i * TILE_SIZE, WHITE);
                                break;
                            case DIRECTION_DOWN:
                                DrawTexture(pacmanOpenDown, offsetX + j * TILE_SIZE, offsetY + i * TILE_SIZE, WHITE);
                                break;
                            case DIRECTION_LEFT:
                                DrawTexture(pacmanOpenLeft, offsetX + j * TILE_SIZE, offsetY + i * TILE_SIZE, WHITE);
                                break;
                            case DIRECTION_RIGHT:
                            default:
                                DrawTexture(pacmanOpen, offsetX + j * TILE_SIZE, offsetY + i * TILE_SIZE, WHITE);
                                break;
                        }
                    }
                    break;
                case 'G':
                    for (int k = 0; k < MAX_GHOSTS; k++) {
                        if (ghosts[k].x == j && ghosts[k].y == i) {
                            DrawTexture(ghostTextures[k][ghostDirections[k]], offsetX + j * TILE_SIZE, offsetY + i * TILE_SIZE, WHITE);
                        }
                    }
                    break;
                case 'S':
                    DrawTexture(starTexture, offsetX + j * TILE_SIZE, offsetY + i * TILE_SIZE, WHITE);
                    break;
                case 'A':
                    DrawTexture(appleTexture, offsetX + j * TILE_SIZE, offsetY + i * TILE_SIZE, WHITE);
                    break;
                case 'E':
                    DrawTexture(pepperTexture, offsetX + j * TILE_SIZE, offsetY + i * TILE_SIZE, WHITE);
                    break;
                case 'C':
                    DrawTexture(cherryTexture, offsetX + j * TILE_SIZE, offsetY + i * TILE_SIZE, WHITE);
                    break;
                case 'M':
                    DrawTexture(mushroomTexture, offsetX + j * TILE_SIZE, offsetY + i * TILE_SIZE, WHITE);
                    break;
            }
        }
    }

    char scoreText[50];
    sprintf(scoreText, "Score: %d", score);
    DrawText(scoreText, 10, 10, 20, YELLOW);

    char lifeText[50];
    sprintf(lifeText, "Lives: %d", lifeCount);
    DrawText(lifeText, 10, 40, 20, YELLOW);
}

void MovePacman() {
    pacmanMoveCounter++;
    int moveDelay = isSpeedBoosted ? PACMAN_MOVE_DELAY / 2 : PACMAN_MOVE_DELAY;
    if (pacmanMoveCounter < moveDelay) return;
    pacmanMoveCounter = 0;

    int newX = pacman.x;
    int newY = pacman.y;

    if (IsKeyDown(KEY_UP)) {
        newY--;
        pacmanDirection = DIRECTION_UP;
    } else if (IsKeyDown(KEY_DOWN)) {
        newY++;
        pacmanDirection = DIRECTION_DOWN;
    } else if (IsKeyDown(KEY_LEFT)) {
        newX--;
        pacmanDirection = DIRECTION_LEFT;
    } else if (IsKeyDown(KEY_RIGHT)) {
        newX++;
        pacmanDirection = DIRECTION_RIGHT;
    }

    if (map[newY][newX] != '#') {
        if (map[newY][newX] == 'S') {
            score += 10;
            map[newY][newX] = ' ';
            remainingDots--;
            PlaySound(eatPelletSound);
        } else if (map[newY][newX] == 'A') {
            lifeCount++;
            map[newY][newX] = ' ';
            PlaySound(eatAppleSound);
            SetSoundVolume(eatAppleSound, 1.2f);
        } else if (map[newY][newX] == 'E') {
            isSpeedBoosted = 1;
            speedBoostCounter = SPEED_BOOST_DURATION;
            map[newY][newX] = ' ';
            PlaySound(eatPepperSound);
            SetSoundVolume(eatPepperSound, 1.2f);
        } else if (map[newY][newX] == 'C') {
            canEatGhosts++;
            map[newY][newX] = ' ';
            PlaySound(eatCherrySound);
            SetSoundVolume(eatCherrySound, 1.2f);
        } else if (map[newY][newX] == 'M') {
            lifeCount--;
            map[newY][newX] = ' ';
            if (lifeCount != 0) {
                PlaySound(eatMushroomSound);
                SetSoundVolume(eatMushroomSound, 1.2f);
            }
        }

        map[pacman.y][pacman.x] = ' ';
        pacman.x = newX;
        pacman.y = newY;
        map[pacman.y][pacman.x] = 'P';
    }
}

void MoveGhosts() {
    static int frameCounter = 0;
    frameCounter++;

    if (frameCounter >= GHOST_MOVE_DELAY) {
        frameCounter = 0;

        int directions[3][2] = {{-1, 0}, {0, -1}, {0, 1}};
        for (int i = 0; i < MAX_GHOSTS; i++) {
            int moved = 0;
            int attempts = 0;

            while (!moved && attempts < 10) {
                int dir = rand() % 3;
                int newX = ghosts[i].x + directions[dir][1];
                int newY = ghosts[i].y + directions[dir][0];

                if (map[newY][newX] != '#' && map[newY][newX] != 'G' && map[newY][newX] != 'P') {
                    if (map[ghosts[i].y][ghosts[i].x] == 'G') {
                        map[ghosts[i].y][ghosts[i].x] = originalMap[ghosts[i].y][ghosts[i].x];
                    }

                    ghosts[i].x = newX;
                    ghosts[i].y = newY;
                    ghostDirections[i] = dir;

                    originalMap[newY][newX] = map[newY][newX];
                    map[newY][newX] = 'G';

                    moved = 1;
                }
                attempts++;
            }
        }
    }
}

int CheckCollisionWithGhosts() {
    for (int i = 0; i < MAX_GHOSTS; i++) {
        if (ghosts[i].x == pacman.x && ghosts[i].y == pacman.y) {
            if (canEatGhosts > 0) {
                score += 20;
                canEatGhosts--;
                map[ghosts[i].y][ghosts[i].x] = originalMap[ghosts[i].y][ghosts[i].x];
                ghosts[i].x = rand() % MAP_WIDTH;
                ghosts[i].y = rand() % MAP_HEIGHT;
                PlaySound(eatGhostSound);
                return 0;
            } else {
                lifeCount--;
                if (lifeCount <= 0) {
                    PlaySound(gameOverSound);
                    return 1;
                } else {
                    map[pacman.y][pacman.x] = ' ';
                    pacman.x = 1;
                    pacman.y = 1;
                    map[pacman.y][pacman.x] = 'P';
                    return 0;
                }
            }
        }
    }
    return 0;
}

void LoadHighScores(int *scores, int *count) {
    FILE *file = fopen("highscores.txt", "r");
    if (file != NULL) {
        *count = 0;
        while (fscanf(file, "%d", &scores[*count]) == 1 && *count < 10) {
            (*count)++;
        }
        fclose(file);
    }
}

void SaveHighScore(int newScore) {
    int scores[10];
    int count = 0;

    LoadHighScores(scores, &count);


    if (count < 10) {
        scores[count++] = newScore;
    } else {

        int minIndex = 0;
        for (int i = 1; i < 10; i++) {
            if (scores[i] < scores[minIndex]) {
                minIndex = i;
            }
        }
        if (newScore > scores[minIndex]) {
            scores[minIndex] = newScore;
        }
    }


    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (scores[i] < scores[j]) {
                int temp = scores[i];
                scores[i] = scores[j];
                scores[j] = temp;
            }
        }
    }

    // Save the scores back to the file
    FILE *file = fopen("highscores.txt", "w");
    if (file != NULL) {
        for (int i = 0; i < count; i++) {
            fprintf(file, "%d\n", scores[i]);
        }
        fclose(file);
    }
}
int ShowGameOverScreen() {
    StopSound(backgroundMusic);
    PlaySound(gameOverSound);

    if (score > highScore) {
        highScore = score;
    }

    SaveHighScore(score);
    latestScore = score;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        DrawText("Game Over!", SCREEN_WIDTH / 2 - MeasureText("Game Over!", 40) / 2, SCREEN_HEIGHT / 2 - 70, 40, RED);

        DrawText("Press R to go back to the main menu", SCREEN_WIDTH / 2 - MeasureText("Press R to go back to the main menu", 20) / 2, SCREEN_HEIGHT / 2 - 10, 20, WHITE);

        // char scoreText[50];
        // sprintf(scoreText, "Latest Score: %d", latestScore);
        // DrawText(scoreText, SCREEN_WIDTH / 2 - MeasureText(scoreText, 20) / 2, SCREEN_HEIGHT / 2 + 30, 20, RED);
        //
        // sprintf(scoreText, "High Score: %d", highScore);
        // DrawText(scoreText, SCREEN_WIDTH / 2 - MeasureText(scoreText, 20) / 2, SCREEN_HEIGHT / 2 + 70, 20, RED);

        EndDrawing();

        if (IsKeyPressed(KEY_R)) {
            return 1;
        }
    }

    return 0;
}
void ShowHighScoresScreen() {
    int scores[10];
    int count = 0;
    LoadHighScores(scores, &count);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        DrawText("High Scores", SCREEN_WIDTH / 2 - MeasureText("High Scores", 40) / 2, 50, 40, YELLOW);

        for (int i = 0; i < count; i++) {
            char scoreText[50];
            sprintf(scoreText, "%d. %d", i + 1, scores[i]);
            DrawText(scoreText, SCREEN_WIDTH / 2 - MeasureText(scoreText, 20) / 2, 150 + i * 30, 20, WHITE);
        }

        DrawText("Press R to go back", SCREEN_WIDTH / 2 - MeasureText("Press R to go back", 20) / 2, SCREEN_HEIGHT - 50, 20, WHITE);

        EndDrawing();

        if (IsKeyPressed(KEY_R)) {
           break;
        }
    }
}
int Menu() {
    int selection = 0;
    const char* options[] = {"Start Game", "Scores and Records", "Exit","new option"};
    const int optionCount = sizeof(options) / sizeof(options[0]);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("Welcome to Pacman", SCREEN_WIDTH / 2 - MeasureText("Welcome to Pacman", 20), SCREEN_HEIGHT / 2 - 150, 40, YELLOW);

        for (int i = 0; i < optionCount; i++) {
            Color color = (i == selection) ? YELLOW : WHITE;
            DrawText(options[i], SCREEN_WIDTH / 2 - MeasureText(options[i], 20) / 2, SCREEN_HEIGHT / 2 - optionCount * 20 + i * 40, 20, color);

            if (!IsSoundPlaying(backgroundMusic))
                PlaySound(backgroundMusic);
        }

        if (selection == 1) {
            int scores[10];
            int count = 0;
            LoadHighScores(scores, &count);

            for (int i = 0; i < count; i++) {
                char scoreText[50];
                sprintf(scoreText, "%d. %d", i + 1, scores[i]);
                DrawText(scoreText, SCREEN_WIDTH / 2 - MeasureText(scoreText, 20) / 2, SCREEN_HEIGHT / 2 + 120 + i * 30, 20, YELLOW);
            }
            if (selection==4) {
                PlaySound(gameOverSound);

            }
        }

        EndDrawing();

        if (IsKeyPressed(KEY_UP)) selection--;
        else if (IsKeyPressed(KEY_DOWN)) selection++;

        if (selection < 0) selection += optionCount;
        if (selection >= optionCount) selection -= optionCount;

        if (IsKeyPressed(KEY_ENTER)) {
            if (selection == 1) {
                ShowHighScoresScreen();
            } else {
                return selection;
            }
        }
    }

    return -1;
}
void ResetGame() {
    latestScore = score;
    if (check == 0)
        score = 0;
    check = 0;

    lifeCount = 1;

    isSpeedBoosted = 0;
    speedBoostCounter = 0;
    canEatGhosts = 0;

    int randd = rand() % 15;
    if (randd == 0) {
        mapp = GREEN;
    } else if (randd == 1) {
        mapp = BLUE;
    } else if (randd == 2) {
        mapp = RED;
    } else if (randd == 3) {
        mapp = PURPLE;
    } else if (randd == 4) {
        mapp = DARKGREEN;
    } else if (randd == 5) {
        mapp = WHITE;
    } else if (randd == 6) {
        mapp = YELLOW;
    } else if (randd == 7) {
        mapp = ORANGE;
    } else if (randd == 8) {
        mapp = GRAY;
    } else if (randd == 9) {
        mapp = PINK;
    } else if (randd == 10) {
        mapp = DARKBLUE;
    } else if (randd == 11) {
        mapp = BROWN;
    } else if (randd == 12) {
        mapp = DARKBROWN;
    } else if (randd == 13) {
        mapp = DARKPURPLE;
    } else if (randd == 14) {
        mapp = GOLD;
    }

    LoadRandomMap(difficult);

    pacman.x = 1;
    pacman.y = 1;
    map[pacman.y][pacman.x] = 'P';
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pac-Man");
    InitAudioDevice();
    SetTargetFPS(60);
    srand(time(NULL));

    LoadTextures();
    LoadGhostTextures();
    LoadSounds();

    PlaySound(backgroundMusic);
    SetSoundVolume(backgroundMusic, 1.0f);

    while (!WindowShouldClose()) {
        int menuSelection = Menu();
        if (menuSelection == 2 || menuSelection == -1) {
            CloseWindow();
            return 0;
        }

        int difficulty = SelectDifficulty();
        if (difficulty == DIFFICULTY_IMPOSSIBLE) {
            ShowGameOverScreen();
            continue;
        }

        LoadRandomMap(difficulty);

        for (int i = 0; i < MAX_GHOSTS; i++) {
            int x, y;
            do {
                x = rand() % MAP_WIDTH;
                y = rand() % MAP_HEIGHT;
            } while (map[y][x] != '.');

            ghosts[i].x = x;
            ghosts[i].y = y;
            map[y][x] = 'G';
        }

        map[pacman.y][pacman.x] = 'P';

        int mapWidthPixels = MAP_WIDTH * TILE_SIZE;
        int mapHeightPixels = MAP_HEIGHT * TILE_SIZE;
        int offsetX = (SCREEN_WIDTH - mapWidthPixels) / 2;
        int offsetY = (SCREEN_HEIGHT - mapHeightPixels) / 2;

        while (!WindowShouldClose()) {
            animCounter++;
            if (animCounter >= 30) animCounter = 0;

            MovePacman();
            MoveGhosts();

            if (CheckCollisionWithGhosts() && lifeCount > 0) {
                if (lifeCount <= 0) {
                    StopSound(backgroundMusic);
                    int result = ShowGameOverScreen();
                    if (result == 1) {
                        ResetGame();
                        PlaySound(backgroundMusic);
                        break;
                    } else {
                        CloseWindow();
                        return 0;
                    }
                }
            } else if (lifeCount <= 0) {
                StopSound(backgroundMusic);
                PlaySound(gameOverSound);
                int result = ShowGameOverScreen();
                if (result == 1) {
                    ResetGame();
                    PlaySound(backgroundMusic);
                    break;
                } else {
                    CloseWindow();
                    return 0;
                }
            }

            if (isSpeedBoosted) {
                speedBoostCounter--;
                if (speedBoostCounter <= 0) {
                    isSpeedBoosted = 0;
                }
            }

            if (remainingDots == 0) {
                check = 1;
                ResetGame();
            }

            if (!IsSoundPlaying(backgroundMusic)) {
                PlaySound(backgroundMusic);
            }

            BeginDrawing();
            ClearBackground(BLACK);
            DrawMap(offsetX, offsetY);
            EndDrawing();
        }
    }

    UnloadSounds();
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
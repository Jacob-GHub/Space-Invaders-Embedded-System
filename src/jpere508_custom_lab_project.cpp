//    Your Name & E-mail: Jacob Perez jpere508@ucr.edu
//    Discussion Section: 021
//    Assignment: Custom Lab Project
//    Exercise Description:
//    I acknowledge all content contained herein, excluding template or example
//    code, is my own original work.

//    Demo Link: https://youtu.be/HwmESZimtEM

//    I used the following datasheet for understanding the SSD1306 screen: https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf
//    I also dug through and understood the code in this library in order to implement my own versions for my own use cases for this project: https://github.com/adafruit/Adafruit_SSD1306

#include "timerISR.h"
#include "jpere508_ssd1306.h"
#include <avr/io.h>

#define NUM_TASKS 8

typedef struct _task
{
    signed char state;
    unsigned long period;
    unsigned long elapsedTime;
    int (*TickFct)(int);
} task;

const unsigned long TASK1_PERIOD = 50;
const unsigned long TASK2_PERIOD = 100;
const unsigned long TASK3_PERIOD = 50;
const unsigned long TASK4_PERIOD = 250;
const unsigned long TASK5_PERIOD = 20;
const unsigned long TASK6_PERIOD = 100;
const unsigned long TASK7_PERIOD = 100;
const unsigned long TASK8_PERIOD = 50;
const unsigned long GCD_PERIOD = 10;

task tasks[NUM_TASKS];

unsigned char playerX;
int score;
unsigned char lives;
unsigned char gameState;
int enemyDir;
unsigned char needsReset;
unsigned char soundTrigger;

unsigned char currentWave;
unsigned char enemiesAlive;
unsigned long enemyBasePeriod;

int highScore;

unsigned char musicNote;
unsigned char musicEnabled;

unsigned long powerupSpawnTimer;
unsigned char invincibilityActive;
unsigned int invincibilityTimer;
unsigned char multiShotActive;
unsigned int multiShotTimer;
unsigned char shieldActive;

typedef struct
{
    unsigned char x, y;
    unsigned char alive;
} Enemy;

typedef struct
{
    unsigned char x, y;
    unsigned char active;
} Bullet;

typedef struct
{
    unsigned char x, y;
    unsigned char active;
    unsigned char type;
} PowerUp;

Enemy enemies[15];
Bullet bullets[3];
PowerUp powerups[2];

enum InputStates
{
    INPUT_INIT,
    INPUT_CHECK
} inputState;

enum GameStates
{
    GAME_TITLE,
    GAME_PLAY,
    GAME_OVER,
    GAME_WIN
} gameLogicState;

enum DisplayStates
{
    DISPLAY_UPDATE
} displayState;

enum EnemyStates
{
    ENEMY_MOVE
} enemyState;

enum BulletStates
{
    BULLET_MOVE
} bulletState;

enum BuzzerStates
{
    BUZZER_OFF,
    BUZZER_PLAY
} buzzerState;

enum SpeedStates
{
    SPEED_UPDATE
} speedState;

enum PowerUpStates
{
    POWERUP_UPDATE
} powerupState;

int input_tick(int state);
int game_logic_tick(int state);
int display_tick(int state);
int enemy_tick(int state);
int bullet_tick(int state);
int buzzer_tick(int state);
int speed_controller_tick(int state);
int powerup_tick(int state);

void init_game(void);
void init_enemies(void);
unsigned char check_collision(unsigned char x1, unsigned char y1, unsigned char w1, unsigned char h1,
                              unsigned char x2, unsigned char y2, unsigned char w2, unsigned char h2);

void eeprom_write(unsigned int addr, int value);
int eeprom_read(unsigned int addr);
void spawn_powerup(void);

void TimerISR()
{
    for (unsigned int i = 0; i < NUM_TASKS; i++)
    {
        if (tasks[i].elapsedTime >= tasks[i].period)
        {
            tasks[i].state = tasks[i].TickFct(tasks[i].state);
            tasks[i].elapsedTime = 0;
        }
        tasks[i].elapsedTime += GCD_PERIOD;
    }
}

int main(void)
{
    DDRD = (1 << PD6);
    PORTD = 0xFF;
    OCR0A = 255;
    TCCR0A |= (1 << COM0A1);
    TCCR0A |= (1 << WGM01) | (1 << WGM00);

    setup_display();
    init_game();

    tasks[0].period = TASK1_PERIOD;
    tasks[0].state = INPUT_INIT;
    tasks[0].elapsedTime = 0;
    tasks[0].TickFct = &input_tick;

    tasks[1].period = TASK2_PERIOD;
    tasks[1].state = GAME_TITLE;
    tasks[1].elapsedTime = 0;
    tasks[1].TickFct = &game_logic_tick;

    tasks[2].period = TASK3_PERIOD;
    tasks[2].state = DISPLAY_UPDATE;
    tasks[2].elapsedTime = 0;
    tasks[2].TickFct = &display_tick;

    tasks[3].period = TASK4_PERIOD;
    tasks[3].state = ENEMY_MOVE;
    tasks[3].elapsedTime = 0;
    tasks[3].TickFct = &enemy_tick;

    tasks[4].period = TASK5_PERIOD;
    tasks[4].state = BULLET_MOVE;
    tasks[4].elapsedTime = 0;
    tasks[4].TickFct = &bullet_tick;

    tasks[5].period = TASK6_PERIOD;
    tasks[5].state = BUZZER_OFF;
    tasks[5].elapsedTime = 0;
    tasks[5].TickFct = &buzzer_tick;

    tasks[6].period = TASK7_PERIOD;
    tasks[6].state = SPEED_UPDATE;
    tasks[6].elapsedTime = 0;
    tasks[6].TickFct = &speed_controller_tick;

    tasks[7].period = TASK8_PERIOD;
    tasks[7].state = POWERUP_UPDATE;
    tasks[7].elapsedTime = 0;
    tasks[7].TickFct = &powerup_tick;

    TimerSet(GCD_PERIOD);
    TimerOn();

    while (1)
    {
    }

    return 0;
}

void eeprom_write(unsigned int addr, int value)
{
    while (EECR & (1 << EEPE))
        ;

    EEAR = addr;
    EEDR = (unsigned char)(value & 0xFF);
    EECR |= (1 << EEMPE);
    EECR |= (1 << EEPE);

    while (EECR & (1 << EEPE))
        ;

    EEAR = addr + 1;
    EEDR = (unsigned char)((value >> 8) & 0xFF);
    EECR |= (1 << EEMPE);
    EECR |= (1 << EEPE);
}

int eeprom_read(unsigned int addr)
{
    int value = 0;

    while (EECR & (1 << EEPE))
        ;

    EEAR = addr;
    EECR |= (1 << EERE);
    value = EEDR;

    while (EECR & (1 << EEPE))
        ;

    EEAR = addr + 1;
    EECR |= (1 << EERE);
    value |= (EEDR << 8);

    return value;
}

int input_tick(int state)
{
    unsigned char leftBtn = !(PIND & (1 << PD2));
    unsigned char rightBtn = !(PIND & (1 << PD3));
    unsigned char fireBtn = !(PIND & (1 << PD4));
    unsigned char resetBtn = !(PIND & (1 << PD5));

    static unsigned char prevFire = 0;
    static unsigned char prevReset = 0;

    switch (state)
    {
    case INPUT_INIT:
    {
        state = INPUT_CHECK;
        break;
    }
    case INPUT_CHECK:
    {
        break;
    }
    default:
    {
        state = INPUT_INIT;
        break;
    }
    }

    switch (state)
    {
    case INPUT_CHECK:
    {
        if (gameState == 1)
        {
            if (leftBtn && playerX > 4)
            {
                playerX -= 2;
            }
            if (rightBtn && playerX < 120)
            {
                playerX += 2;
            }

            if (fireBtn && !prevFire)
            {
                if (multiShotActive)
                {
                    unsigned char fired = 0;
                    for (unsigned char i = 0; i < 3; i++)
                    {
                        if (!bullets[i].active)
                        {
                            bullets[i].y = 54;
                            bullets[i].active = 1;
                            soundTrigger = 1;

                            if (fired == 0)
                            {
                                bullets[i].x = playerX - 4;
                            }
                            else if (fired == 1)
                            {
                                bullets[i].x = playerX;
                            }
                            else
                            {
                                bullets[i].x = playerX + 4;
                            }
                            fired++;
                        }
                    }
                }
                else
                {
                    for (unsigned char i = 0; i < 3; i++)
                    {
                        if (!bullets[i].active)
                        {
                            bullets[i].x = playerX;
                            bullets[i].y = 54;
                            bullets[i].active = 1;
                            soundTrigger = 1;
                            break;
                        }
                    }
                }
            }
        }

        if (resetBtn && !prevReset)
        {
            needsReset = 1;
        }

        prevFire = fireBtn;
        prevReset = resetBtn;
        break;
    }
    }

    return state;
}

int game_logic_tick(int state)
{
    static unsigned char gameOverSoundPlayed = 0;
    static unsigned char winSoundPlayed = 0;

    switch (state)
    {
    case GAME_TITLE:
    {
        if (!(PIND & (1 << PD4)))
        {
            state = GAME_PLAY;
            gameOverSoundPlayed = 0;
            winSoundPlayed = 0;
        }
        break;
    }
    case GAME_PLAY:
    {
        if (needsReset)
        {
            state = GAME_TITLE;
        }
        else if (lives == 0)
        {
            state = GAME_OVER;
        }
        else
        {
            unsigned char allDead = 1;
            for (unsigned char i = 0; i < 15; i++)
            {
                if (enemies[i].alive)
                {
                    allDead = 0;
                    break;
                }
            }

            if (allDead)
            {
                if (currentWave >= 3)
                {
                    state = GAME_WIN;
                }
                else
                {
                    currentWave++;
                    init_enemies();
                    soundTrigger = 4;
                }
            }
        }
        break;
    }
    case GAME_OVER:
    case GAME_WIN:
    {
        if (needsReset)
        {
            state = GAME_TITLE;
        }
        break;
    }
    default:
    {
        state = GAME_TITLE;
        break;
    }
    }

    switch (state)
    {
    case GAME_TITLE:
    {
        gameState = 0;
        if (needsReset)
        {
            init_game();
            needsReset = 0;
        }
        break;
    }
    case GAME_PLAY:
    {
        gameState = 1;

        if (invincibilityActive)
        {
            if (invincibilityTimer > 0)
            {
                invincibilityTimer--;
            }
            else
            {
                invincibilityActive = 0;
            }
        }

        if (multiShotActive)
        {
            if (multiShotTimer > 0)
            {
                multiShotTimer--;
            }
            else
            {
                multiShotActive = 0;
            }
        }

        // collision
        if (!invincibilityActive)
        {
            for (unsigned char i = 0; i < 15; i++)
            {
                if (enemies[i].alive)
                {
                    if (enemies[i].y >= 45)
                    {
                        if (check_collision(enemies[i].x, enemies[i].y, 8, 6,
                                            playerX - 3, 52, 6, 6))
                        {
                            if (shieldActive)
                            {
                                shieldActive = 0;
                                enemies[i].alive = 0;
                                enemiesAlive--;
                            }
                            else
                            {
                                lives--;
                                enemies[i].alive = 0;
                                enemiesAlive--;
                                if (lives == 0 && !gameOverSoundPlayed)
                                {
                                    soundTrigger = 3;
                                    gameOverSoundPlayed = 1;
                                }
                            }
                        }
                    }
                }
            }
        }
        break;
    }
    case GAME_OVER:
    {
        gameState = 2;
        if (!gameOverSoundPlayed)
        {
            soundTrigger = 3;
            gameOverSoundPlayed = 1;
        }
        if (score > highScore)
        {
            highScore = score;
            eeprom_write(0, highScore);
        }
        break;
    }
    case GAME_WIN:
    {
        gameState = 3;
        if (!winSoundPlayed)
        {
            soundTrigger = 4;
            winSoundPlayed = 1;
        }
        if (score > highScore)
        {
            highScore = score;
            eeprom_write(0, highScore);
        }
        break;
    }
    }

    return state;
}

int display_tick(int state)
{
    switch (state)
    {
    case DISPLAY_UPDATE:
    {
        break;
    }
    default:
    {
        state = DISPLAY_UPDATE;
        break;
    }
    }

    switch (state)
    {
    case DISPLAY_UPDATE:
    {
        clear_display();

        if (gameState == 0)
        {
            draw_string(20, 15, "SPACE");
            draw_string(10, 25, "INVADERS");

            draw_string(15, 42, "HIGH:");
            draw_number(50, 42, highScore);

            draw_string(10, 54, "PRESS FIRE");
        }
        else if (gameState == 1)
        {
            draw_string(0, 2, "S:");
            draw_number(12, 2, score);

            draw_string(50, 2, "W:");
            draw_number(62, 2, currentWave);

            draw_string(90, 2, "L:");
            draw_number(102, 2, lives);

            draw_rectangle(playerX - 3, 56, 6, 2, 1);
            draw_rectangle(playerX - 2, 54, 4, 2, 1);
            draw_rectangle(playerX - 1, 52, 2, 2, 1);

            for (unsigned char i = 0; i < 15; i++)
            {
                if (enemies[i].alive)
                {
                    unsigned char ex = enemies[i].x;
                    unsigned char ey = enemies[i].y;

                    unsigned char drawY = ey < 2 ? 2 : ey;

                    draw_pixel(ex + 2, drawY, 1);
                    draw_pixel(ex + 5, drawY, 1);

                    draw_pixel(ex + 1, drawY + 1, 1);
                    draw_pixel(ex + 2, drawY + 1, 1);
                    draw_pixel(ex + 3, drawY + 1, 1);
                    draw_pixel(ex + 4, drawY + 1, 1);
                    draw_pixel(ex + 5, drawY + 1, 1);
                    draw_pixel(ex + 6, drawY + 1, 1);

                    draw_pixel(ex, drawY + 2, 1);
                    draw_pixel(ex + 1, drawY + 2, 1);
                    draw_pixel(ex + 2, drawY + 2, 1);
                    draw_pixel(ex + 3, drawY + 2, 1);
                    draw_pixel(ex + 4, drawY + 2, 1);
                    draw_pixel(ex + 5, drawY + 2, 1);
                    draw_pixel(ex + 6, drawY + 2, 1);
                    draw_pixel(ex + 7, drawY + 2, 1);

                    draw_pixel(ex, drawY + 3, 1);
                    draw_pixel(ex + 1, drawY + 3, 1);
                    draw_pixel(ex + 3, drawY + 3, 1);
                    draw_pixel(ex + 4, drawY + 3, 1);
                    draw_pixel(ex + 6, drawY + 3, 1);
                    draw_pixel(ex + 7, drawY + 3, 1);

                    draw_pixel(ex, drawY + 4, 1);
                    draw_pixel(ex + 1, drawY + 4, 1);
                    draw_pixel(ex + 2, drawY + 4, 1);
                    draw_pixel(ex + 3, drawY + 4, 1);
                    draw_pixel(ex + 4, drawY + 4, 1);
                    draw_pixel(ex + 5, drawY + 4, 1);
                    draw_pixel(ex + 6, drawY + 4, 1);
                    draw_pixel(ex + 7, drawY + 4, 1);

                    draw_pixel(ex + 1, drawY + 5, 1);
                    draw_pixel(ex + 2, drawY + 5, 1);
                    draw_pixel(ex + 5, drawY + 5, 1);
                    draw_pixel(ex + 6, drawY + 5, 1);
                }
            }

            for (unsigned char i = 0; i < 2; i++)
            {
                if (powerups[i].active)
                {
                    unsigned char px = powerups[i].x;
                    unsigned char py = powerups[i].y;

                    if (py >= 2 && py < 60)
                    {
                        draw_rectangle(px, py, 4, 4, 1);

                        if (powerups[i].type == 0)
                        {
                            draw_pixel(px + 1, py + 1, 0);
                            draw_pixel(px + 2, py + 1, 0);
                        }
                        else if (powerups[i].type == 1)
                        {
                            draw_pixel(px + 1, py + 2, 0);
                        }
                        else if (powerups[i].type == 2)
                        {
                            draw_pixel(px + 1, py + 1, 0);
                            draw_pixel(px + 2, py + 2, 0);
                        }
                    }
                }
            }

            if (invincibilityActive)
            {
                draw_string(0, 56, "I");
            }
            if (multiShotActive)
            {
                draw_string(112, 56, "M");
            }
            if (shieldActive)
            {
                draw_string(120, 56, "S");
            }

            for (unsigned char i = 0; i < 3; i++)

                for (unsigned char i = 0; i < 3; i++)
                {
                    if (bullets[i].active)
                    {
                        draw_rectangle(bullets[i].x, bullets[i].y, 2, 4, 1);
                    }
                }
        }
        else if (gameState == 2)
        {
            draw_string(25, 15, "GAME OVER");

            draw_string(10, 30, "WAVE:");
            draw_number(45, 30, currentWave);

            draw_string(10, 42, "SCORE:");
            draw_number(50, 42, score);

            if (score >= highScore)
            {
                draw_string(10, 54, "NEW HIGH!");
            }
            else
            {
                draw_string(10, 54, "HIGH:");
                draw_number(45, 54, highScore);
            }
        }
        else if (gameState == 3)
        {
            draw_string(30, 15, "YOU WIN!");
            draw_string(10, 28, "ALL WAVES");

            draw_string(10, 42, "SCORE:");
            draw_number(50, 42, score);

            if (score >= highScore)
            {
                draw_string(10, 54, "NEW HIGH!");
            }
            else
            {
                draw_string(10, 54, "HIGH:");
                draw_number(45, 54, highScore);
            }
        }

        update_display();
        break;
    }
    }

    return state;
}

int enemy_tick(int state)
{
    switch (state)
    {
    case ENEMY_MOVE:
    {
        break;
    }
    default:
    {
        state = ENEMY_MOVE;
        break;
    }
    }

    switch (state)
    {
    case ENEMY_MOVE:
    {
        if (gameState == 1)
        {
            unsigned char changeDir = 0;

            for (unsigned char i = 0; i < 15; i++)
            {
                if (enemies[i].alive)
                {
                    enemies[i].x += enemyDir * 3;
                    if (enemies[i].x < 5 || enemies[i].x > 115)
                    {
                        changeDir = 1;
                    }
                }
            }

            if (changeDir)
            {
                enemyDir = -enemyDir;
                for (unsigned char i = 0; i < 15; i++)
                {
                    if (enemies[i].alive)
                    {
                        enemies[i].y += 3;
                    }
                }
            }
        }
        break;
    }
    }

    return state;
}

int bullet_tick(int state)
{
    switch (state)
    {
    case BULLET_MOVE:
    {
        break;
    }
    default:
    {
        state = BULLET_MOVE;
        break;
    }
    }

    switch (state)
    {
    case BULLET_MOVE:
    {
        if (gameState == 1)
        {
            for (unsigned char i = 0; i < 3; i++)
            {
                if (bullets[i].active)
                {
                    if (bullets[i].y > 10)
                    {
                        bullets[i].y -= 4;
                    }
                    else
                    {
                        bullets[i].active = 0;
                        continue;
                    }

                    for (unsigned char j = 0; j < 15; j++)
                    {
                        if (enemies[j].alive)
                        {
                            if (check_collision(bullets[i].x, bullets[i].y, 2, 4,
                                                enemies[j].x, enemies[j].y, 8, 6))
                            {
                                enemies[j].alive = 0;
                                enemiesAlive--;
                                bullets[i].active = 0;
                                score += 10 * currentWave;
                                soundTrigger = 2;
                                break;
                            }
                        }
                    }
                }
            }
        }
        break;
    }
    }

    return state;
}

int speed_controller_tick(int state)
{
    switch (state)
    {
    case SPEED_UPDATE:
    {
        break;
    }
    default:
    {
        state = SPEED_UPDATE;
        break;
    }
    }

    switch (state)
    {
    case SPEED_UPDATE:
    {
        if (gameState == 1)
        {
            unsigned char enemiesKilled = 15 - enemiesAlive;
            unsigned long speedBoost = (enemiesKilled / 3) * 50;

            unsigned long newPeriod;
            if (enemyBasePeriod > speedBoost + 50)
            {
                newPeriod = enemyBasePeriod - speedBoost;
            }
            else
            {
                newPeriod = 50;
            }

            tasks[3].period = newPeriod;
        }
        break;
    }
    }

    return state;
}

int buzzer_tick(int state)
{
    static unsigned char noteCounter = 0;

    switch (state)
    {
    case BUZZER_OFF:
    {
        if (soundTrigger != 0)
        {
            state = BUZZER_PLAY;
            noteCounter = 0;
        }
        else if (gameState == 1 && musicEnabled)
        {
            state = BUZZER_PLAY;
        }
        break;
    }
    case BUZZER_PLAY:
    {
        if (soundTrigger != 0)
        {
            noteCounter++;

            if ((soundTrigger == 1 && noteCounter >= 2) ||
                (soundTrigger == 2 && noteCounter >= 3) ||
                (soundTrigger == 3 && noteCounter >= 8) ||
                (soundTrigger == 4 && noteCounter >= 12))
            {
                state = BUZZER_OFF;
                soundTrigger = 0;
                noteCounter = 0;
            }
        }
        else if (gameState == 1 && musicEnabled)
        {
            musicNote++;
            if (musicNote >= 32)
            {
                musicNote = 0;
            }
        }
        else
        {
            state = BUZZER_OFF;
        }
        break;
    }
    default:
    {
        state = BUZZER_OFF;
        break;
    }
    }

    switch (state)
    {
    case BUZZER_OFF:
    {
        OCR0A = 255;
        TCCR0B = 0x00;
        break;
    }
    case BUZZER_PLAY:
    {
        if (soundTrigger == 1)
        {
            OCR0A = 128;
            if (noteCounter == 1)
            {
                TCCR0B = (TCCR0B & 0xF8) | 0x04;
            }
            else
            {
                TCCR0B = (TCCR0B & 0xF8) | 0x03;
            }
        }
        else if (soundTrigger == 2)
        {
            OCR0A = 128;
            if (noteCounter == 1)
            {
                TCCR0B = (TCCR0B & 0xF8) | 0x03;
            }
            else if (noteCounter == 2)
            {
                TCCR0B = (TCCR0B & 0xF8) | 0x04;
            }
            else
            {
                TCCR0B = (TCCR0B & 0xF8) | 0x05;
            }
        }
        else if (soundTrigger == 3)
        {
            if (noteCounter == 1)
            {
                TCCR0B = (TCCR0B & 0xF8) | 0x03;
                OCR0A = 128;
            }
            else if (noteCounter == 2)
            {
                TCCR0B = (TCCR0B & 0xF8) | 0x04;
                OCR0A = 110;
            }
            else if (noteCounter == 3 || noteCounter == 4)
            {
                TCCR0B = (TCCR0B & 0xF8) | 0x04;
                OCR0A = 128;
            }
            else if (noteCounter == 5 || noteCounter == 6)
            {
                TCCR0B = (TCCR0B & 0xF8) | 0x05;
                OCR0A = 110;
            }
            else
            {
                TCCR0B = (TCCR0B & 0xF8) | 0x05;
                OCR0A = 140;
            }
        }
        else if (soundTrigger == 4)
        {
            if (noteCounter == 1 || noteCounter == 2 || noteCounter == 3)
            {
                TCCR0B = (TCCR0B & 0xF8) | 0x05;
                OCR0A = 128;
            }
            else if (noteCounter == 4)
            {
                TCCR0B = (TCCR0B & 0xF8) | 0x05;
                OCR0A = 110;
            }
            else if (noteCounter == 5 || noteCounter == 6 || noteCounter == 7)
            {
                TCCR0B = (TCCR0B & 0xF8) | 0x04;
                OCR0A = 128;
            }
            else if (noteCounter == 8)
            {
                TCCR0B = (TCCR0B & 0xF8) | 0x04;
                OCR0A = 110;
            }
            else if (noteCounter == 9 || noteCounter == 10 || noteCounter == 11)
            {
                TCCR0B = (TCCR0B & 0xF8) | 0x03;
                OCR0A = 128;
            }
            else
            {
                TCCR0B = (TCCR0B & 0xF8) | 0x02;
                OCR0A = 100;
            }
        }
        else if (gameState == 1 && musicEnabled)
        {

            if (musicNote >= 0 && musicNote < 2)
            {
                OCR0A = 120;
                /
                    TCCR0B = (TCCR0B & 0xF8) | 0x03;
            }
            else if (musicNote >= 2 && musicNote < 4)
            {
                OCR0A = 100;
                TCCR0B = (TCCR0B & 0xF8) | 0x03;
            }
            else if (musicNote >= 4 && musicNote < 6)
            {
                OCR0A = 140;
                TCCR0B = (TCCR0B & 0xF8) | 0x03;
            }
            else if (musicNote >= 6 && musicNote < 8)
            {
                OCR0A = 160;
                TCCR0B = (TCCR0B & 0xF8) | 0x03;
            }

            else if (musicNote >= 8 && musicNote < 10)
            {
                OCR0A = 120;
                TCCR0B = (TCCR0B & 0xF8) | 0x03;
            }
            else if (musicNote >= 10 && musicNote < 12)
            {
                OCR0A = 100;
                TCCR0B = (TCCR0B & 0xF8) | 0x03;
            }
            else if (musicNote >= 12 && musicNote < 14)
            {
                OCR0A = 140;
                TCCR0B = (TCCR0B & 0xF8) | 0x03;
            }
            else if (musicNote >= 14 && musicNote < 16)
            {
                OCR0A = 160;
                TCCR0B = (TCCR0B & 0xF8) | 0x03;
            }

            else if (musicNote >= 16 && musicNote < 18)
            {
                OCR0A = 110;
                TCCR0B = (TCCR0B & 0xF8) | 0x03;
            }
            else if (musicNote >= 18 && musicNote < 20)
            {
                OCR0A = 90;
                TCCR0B = (TCCR0B & 0xF8) | 0x03;
            }
            else if (musicNote >= 20 && musicNote < 22)
            {
                OCR0A = 130;
                TCCR0B = (TCCR0B & 0xF8) | 0x03;
            }
            else if (musicNote >= 22 && musicNote < 24)
            {
                OCR0A = 150;
                TCCR0B = (TCCR0B & 0xF8) | 0x03;
            }
            else if (musicNote >= 24 && musicNote < 26)
            {
                OCR0A = 120;
                TCCR0B = (TCCR0B & 0xF8) | 0x03;
            }
            else if (musicNote >= 26 && musicNote < 28)
            {
                OCR0A = 100;
                TCCR0B = (TCCR0B & 0xF8) | 0x03;
            }
            else if (musicNote >= 28 && musicNote < 30)
            {
                OCR0A = 140;
                TCCR0B = (TCCR0B & 0xF8) | 0x03;
            }
            else
            {
                OCR0A = 160;
                TCCR0B = (TCCR0B & 0xF8) | 0x03;
            }
        }
        break;
    }
    }

    return state;
}

int powerup_tick(int state)
{
    switch (state)
    {
    case POWERUP_UPDATE:
    {
        break;
    }
    default:
    {
        state = POWERUP_UPDATE;
        break;
    }
    }

    switch (state)
    {
    case POWERUP_UPDATE:
    {
        if (gameState == 1)
        {
            powerupSpawnTimer++;

            if (powerupSpawnTimer >= 150)
            {
                spawn_powerup();
                powerupSpawnTimer = 0;
            }

            for (unsigned char i = 0; i < 2; i++)
            {
                if (powerups[i].active)
                {
                    powerups[i].y += 1;

                    if (powerups[i].y > 60)
                    {
                        powerups[i].active = 0;
                    }

                    if (check_collision(powerups[i].x, powerups[i].y, 4, 4,
                                        playerX - 3, 52, 6, 6))
                    {
                        if (powerups[i].type == 0)
                        {
                            invincibilityActive = 1;
                            invincibilityTimer = 50;
                        }
                        else if (powerups[i].type == 1)
                        {
                            multiShotActive = 1;
                            multiShotTimer = 50;
                        }
                        else if (powerups[i].type == 2)
                        {
                            shieldActive = 1;
                        }

                        powerups[i].active = 0;
                        soundTrigger = 2;
                    }
                }
            }
        }
        break;
    }
    }

    return state;
}

void init_game(void)
{
    playerX = 64;
    score = 0;
    lives = 3;
    gameState = 0;
    enemyDir = 1;
    needsReset = 0;
    soundTrigger = 0;
    currentWave = 1;
    enemiesAlive = 15;
    enemyBasePeriod = 250;
    musicNote = 0;
    musicEnabled = 1;
    powerupSpawnTimer = 0;
    invincibilityActive = 0;
    invincibilityTimer = 0;
    multiShotActive = 0;
    multiShotTimer = 0;
    shieldActive = 0;

    highScore = eeprom_read(0);

    if (highScore == -1 || highScore < 0)
    {
        highScore = 0;
        eeprom_write(0, 0);
    }

    init_enemies();

    for (unsigned char i = 0; i < 3; i++)
    {
        bullets[i].active = 0;
    }

    for (unsigned char i = 0; i < 2; i++)
    {
        powerups[i].active = 0;
    }
}

void init_enemies(void)
{
    unsigned char index = 0;
    for (unsigned char row = 0; row < 3; row++)
    {
        for (unsigned char col = 0; col < 5; col++)
        {
            enemies[index].x = 15 + col * 20;
            enemies[index].y = 15 + row * 10;
            enemies[index].alive = 1;
            index++;
        }
    }

    enemiesAlive = 15;

    if (currentWave == 1)
    {
        enemyBasePeriod = 300;
    }
    else if (currentWave == 2)
    {
        enemyBasePeriod = 200;
    }
    else
    {
        enemyBasePeriod = 150;
    }

    tasks[3].period = enemyBasePeriod;
}

void spawn_powerup(void)
{
    for (unsigned char i = 0; i < 2; i++)
    {
        if (!powerups[i].active)
        {
            powerups[i].x = 20 + (score % 90);
            powerups[i].y = 10;
            powerups[i].active = 1;
            powerups[i].type = (score + powerupSpawnTimer + playerX) % 3;
            break;
        }
    }
}

unsigned char check_collision(unsigned char x1, unsigned char y1, unsigned char w1, unsigned char h1,
                              unsigned char x2, unsigned char y2, unsigned char w2, unsigned char h2)
{
    return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);
}
#include <stm32f031x6.h>
#include "display.h"
#include "musical_notes.h"
#include <math.h>
#include <stdbool.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 160

#define RIGHT_WALL (SCREEN_WIDTH - 2)
#define LEFT_WALL 2

#define PLAYERONE_GOAL 145
#define PLAYERTWO_GOAL 15

#define BAT_WIDTH 24
#define BAT_HEIGHT 4

#define BALL_WIDTH 4
#define BALL_HEIGHT 4

#define END_SCORE 2

void initClock(void);
void initSysTick(void);
void SysTick_Handler(void);
void delay(volatile uint32_t dly);
void setupIO();
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py);
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber);
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);
void menu();
void update();
void gameOver(int Winner,int playerOneScore, int playerTwoScore);
void hideBall(void);
void showBall(void);
void moveBall(uint32_t newX, uint32_t newY);
void randomize(void);
uint32_t random(uint32_t lower, uint32_t upper);
void redOn(void);
void redOff(void);
void blueOn(void);
void blueOff(void);
void flashRed(int times, int rate);
void flashBlue(int times, int rate);
void flashBoth(int times, int rate);
void ScoreCount();
void playNote(uint32_t Freq,uint32_t duration);
void initTimer();
void playoutroSong();
int rightButtonPlayer1();
int leftButtonPlayer1();
int rightButtonPlayer2();
int leftButtonPlayer2();

uint16_t ball_x;
uint16_t ball_y;

bool playerOneHit = false;
bool PlayerTwoHit = false;

volatile uint32_t milliseconds;

const uint16_t PlayerOneBat[] =
{
    40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,40224,};

const uint16_t PlayerTwoBat[] = 
{
    50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,50585,};

int main()
{
    initClock();
    initSysTick();
    setupIO();
    menu(); 
    return 0;
}
void update()
{
    //update function where the structure of the game is.
   
    int Player1moved = 0;
    int Player2moved = 0;
    int BallMoveCount = 0;
    uint16_t PlayerOneBatx = 50;
    uint16_t PlayerOneBaty = 142;
    uint16_t PlayerTwoBatx = 50;
    uint16_t PlayerTwoBaty = 15;  

    uint16_t oldPlayerOneBatx = 50;
    uint16_t oldPlayerOneBaty = 50;
    uint16_t oldPlayerTwoBatx = 50;
    uint16_t oldPlayerTwoBaty = 50;

    int16_t BallXVelocity = 1;
    int16_t BallYVelocity = 1;

    //both bats are displayed
    putImage(PlayerOneBatx,PlayerOneBaty,BAT_WIDTH,BAT_HEIGHT,PlayerOneBat,0,0);
    putImage(PlayerTwoBatx,PlayerTwoBaty,BAT_WIDTH,BAT_HEIGHT,PlayerTwoBat,0,0);

    // Ball is generated at a random location
    randomize();  
    ball_x = random(0,SCREEN_WIDTH);
    ball_y = random(50,(SCREEN_HEIGHT - 50)); 

    while (1)
    {
        // game loop
        Player1moved = Player2moved = 0;

        if (leftButtonPlayer1()) // Player 1 left pressed
        {
            if (PlayerOneBatx > 3)
            {
                PlayerOneBatx--;
                Player1moved = 1;
            }
        }
        if (rightButtonPlayer1()) // Player 1 right pressed
        {          
            if (PlayerOneBatx < 105)
            {
                PlayerOneBatx++;
                Player1moved = 1;
            }      
        }
        if (leftButtonPlayer2()) // Player 2 left pressed
        {      
            if (PlayerTwoBatx < 105)
            {
                PlayerTwoBatx++;
                Player2moved = 1;
            }
        }
        if (rightButtonPlayer2()) // Player 2 right pressed
        {                  
            if (PlayerTwoBatx > 3)
            {
                PlayerTwoBatx--;
                Player2moved = 1;
            }
        }
        if ((Player1moved) || (Player2moved))
        {
            // only redraw if there has been some movement (reduces flicker)
            fillRectangle(oldPlayerOneBatx,oldPlayerOneBaty,BAT_WIDTH,BAT_HEIGHT,0);
            fillRectangle(oldPlayerTwoBatx,oldPlayerTwoBaty,BAT_WIDTH,BAT_HEIGHT,0);
            oldPlayerOneBatx = PlayerOneBatx;
            oldPlayerOneBaty = PlayerOneBaty; 
            oldPlayerTwoBatx = PlayerTwoBatx;
            oldPlayerTwoBaty = PlayerTwoBaty; 

            putImage(PlayerOneBatx,PlayerOneBaty,BAT_WIDTH,BAT_HEIGHT,PlayerOneBat,0,0);                
            putImage(PlayerTwoBatx,PlayerTwoBaty,BAT_WIDTH,BAT_HEIGHT,PlayerTwoBat,0,0);

            BallMoveCount = 1;
        } 
        // ball wont move until a button is pressed
        if (BallMoveCount == 1)
        {
            moveBall(ball_x+BallXVelocity,ball_y+BallYVelocity);
        }
        // barrier made on the right and left wall, inverts the ball velocity 
        if (ball_x == LEFT_WALL || ball_x == RIGHT_WALL)
        {
            BallXVelocity = BallXVelocity * (-1.5);
            playNote(G4,50);
        }

        ScoreCount();

        if (isInside(ball_x,ball_y,BAT_WIDTH,BAT_HEIGHT,PlayerOneBatx+BAT_WIDTH,PlayerOneBaty))
        {
            // when the ball hits the playerone bat
            BallYVelocity = BallYVelocity * (-1.5); // y velocity inverted
            playerOneHit = true;
            PlayerTwoHit = false;
            redOn();
            blueOff();
            playNote(A4,50);
        }
        if (isInside(ball_x,ball_y,BAT_WIDTH,BAT_HEIGHT,PlayerTwoBatx+BAT_WIDTH,PlayerTwoBaty+BAT_HEIGHT))
        {
            // when the ball hits the playertwo bat
            BallYVelocity = BallYVelocity * (-1.5); // y velocity inverted
            PlayerTwoHit = true;
            playerOneHit = false;
            redOff();
            blueOn();
            playNote(A4,50);
        }  
      
        delay(50);
    } 
}
void initSysTick(void)
{
    SysTick->LOAD = 48000;
    SysTick->CTRL = 7;
    SysTick->VAL = 10;
    __asm(" cpsie i "); // enable interrupts
}
void SysTick_Handler(void)
{
    milliseconds++;
}
void initClock(void)
{
// This is potentially a dangerous function as it could
// result in a system with an invalid clock signal - result: a stuck system
        // Set the PLL up
        // First ensure PLL is disabled
        RCC->CR &= ~(1u<<24);
        while( (RCC->CR & (1 <<25))); // wait for PLL ready to be cleared
       
// Warning here: if system clock is greater than 24MHz then wait-state(s) need to be
// inserted into Flash memory interface
               
        FLASH->ACR |= (1 << 0);
        FLASH->ACR &=~((1u << 2) | (1u<<1));
        // Turn on FLASH prefetch buffer
        FLASH->ACR |= (1 << 4);
        // set PLL multiplier to 12 (yielding 48MHz)
        RCC->CFGR &= ~((1u<<21) | (1u<<20) | (1u<<19) | (1u<<18));
        RCC->CFGR |= ((1<<21) | (1<<19) );

        // Need to limit ADC clock to below 14MHz so will change ADC prescaler to 4
        RCC->CFGR |= (1<<14);

        // and turn the PLL back on again
        RCC->CR |= (1<<24);        
        // set PLL as system clock source
        RCC->CFGR |= (1<<1);
}
void delay(volatile uint32_t dly)
{
    uint32_t end_time = dly + milliseconds;
    while(milliseconds != end_time)
        __asm(" wfi "); // sleep
}
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber)
{
    Port->PUPDR = Port->PUPDR &~(3u << BitNumber*2); // clear pull-up resistor bits
    Port->PUPDR = Port->PUPDR | (1u << BitNumber*2); // set pull-up bit
}
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode)
{
    /*
    */
    uint32_t mode_value = Port->MODER;
    Mode = Mode << (2 * BitNumber);
    mode_value = mode_value & ~(3u << (BitNumber * 2));
    mode_value = mode_value | Mode;
    Port->MODER = mode_value;
}
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py)
{
    // checks to see if point px,py is within the rectange defined by x,y,w,h
    uint16_t x2,y2;
    x2 = x1+w;
    y2 = y1+h;
    int rvalue = 0;
    if ( (px >= x1) && (px <= x2))
    {
        // ok, x constraint met
        if ( (py >= y1) && (py <= y2))
            rvalue = 1;
    }
    return rvalue;
}
void setupIO()
{
    RCC->AHBENR |= (1 << 18) + (1 << 17); // enable Ports A and B
    display_begin();
    pinMode(GPIOB,4,0);
    pinMode(GPIOB,5,0);
    pinMode(GPIOB,0,1);
    pinMode(GPIOB,3,1);
    pinMode(GPIOA,8,0);
    pinMode(GPIOA,11,0);
    enablePullUp(GPIOB,4);
    enablePullUp(GPIOB,5);
    enablePullUp(GPIOA,11);
    enablePullUp(GPIOA,8);

    initTimer();
}
void menu()
{
    //This fucntion is the menu of the game
    while (1)
    {
        printTextX2("PONG!", 35, SCREEN_HEIGHT/3, RGBToWord(0xff,0xff,0), 0);
        printText("Press any button", 10, (SCREEN_HEIGHT - SCREEN_HEIGHT/3), RGBToWord(0xff,0xff,0), 0);
        printText("To play", 40, (SCREEN_HEIGHT - SCREEN_HEIGHT/3 + 10), RGBToWord(0xff,0xff,0), 0);
        if (rightButtonPlayer1() || leftButtonPlayer1() || rightButtonPlayer2() || rightButtonPlayer2())
        {
            //starts game when the User presses any button
            fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
            delay(1000);
            update();
        }
    }
}
void gameOver(int Winner, int PlayerOneScore, int PlayerTwoScore)
{
    // gameover screen, User can restart or quiz the game
    fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0); // resets screen
    redOff();
    blueOff();
    delay(500);
    printTextX2("GAME OVER!", 7, 70, RGBToWord(0xff,0xff,0), 0);
    delay(2000);
    int freq;
    int note_number = 0;
    int temp = 70;
    for (int i = 0; i < 100; i++)
    {
        // Gameover animation + freqence formula used to make a tone with the animation
        fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
        printTextX2("GAME OVER!", 7, temp, RGBToWord(0xff,0xff,0), 0);
        freq = 73 * pow(2,note_number/12);
        playNote(freq,50);
        note_number++;
        temp--;
        i++;
    }
    if (Winner == 1)
    {
        // if playerone won
        playNote(A4,200);
        playNote(B4,200);
        playNote(C4,200);
        printTextX2("PLAYER ONE", 7, 65, RGBToWord(255,50,50), 0);
        printTextX2("WON!", 40, 85, RGBToWord(255,50,50), 0);
        playNote(A5,200);
        delay(3000);

        printNumberX2(PlayerOneScore, 40, 120, RGBToWord(255,50,50), 0);
        printTextX2(":", 55, 120, RGBToWord(255,255,255), 0);
        printNumberX2(PlayerTwoScore, 70, 120, RGBToWord(50,50,255), 0);
        playNote(C4,50);
        delay(5000);
    }
    if (Winner == 2)
    {
        // if playertwo won
        playNote(A4,200);
        playNote(B4,200);
        playNote(C4,200);
        printTextX2("PLAYER TWO", 7, 65, RGBToWord(50,50,255), 0);
        printTextX2("WON!", 40, 85, RGBToWord(50,50,255), 0);
        playNote(A5,200);
        delay(3000);
        
        printNumberX2(PlayerTwoScore, 40, 120, RGBToWord(50,50,255), 0);
        printTextX2(":", 55, 120, RGBToWord(255,255,255), 0);
        printNumberX2(PlayerOneScore, 70, 120, RGBToWord(255,50,50), 0);
        playNote(C4,50);
        delay(5000);
    }

    // screen resets, restart and exit options displayed
    fillRectangle(0, 100, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    printText("press < or >", 25, 105, RGBToWord(255,255,255), 0);
    printText("to restart", 30, 115, RGBToWord(255,255,255), 0);
    printText("press ^ or v", 25, 130, RGBToWord(255,255,255), 0);
    printText("to quit", 30, 140, RGBToWord(255,255,255), 0);
    playNote(C4,50);

    while (true)
    {
        // if user presses the < or > buuton,game resets to the menu
        if (rightButtonPlayer1() || rightButtonPlayer2())
        {   
            playNote(C4,50);
            menu();
        }
        // if user presses the ^ or v button, screen resets and game ends                   
        if (leftButtonPlayer1() || leftButtonPlayer2())
        {
            playoutroSong();
            fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
            exit(0);
        }  
    }
}
void hideBall(void)
{
    // ball is hidden
    fillRectangle(ball_x,ball_y,BALL_WIDTH,BALL_HEIGHT,0);
}
void showBall(void)
{
    // ball is shown and changes colour depending on the last bat it hits
    if (playerOneHit)
    {
        fillRectangle(ball_x,ball_y,BALL_WIDTH,BALL_HEIGHT,RGBToWord(255,50,50));
    }
    else if (PlayerTwoHit)
    {
        fillRectangle(ball_x,ball_y,BALL_WIDTH,BALL_HEIGHT,RGBToWord(50,50,255));
    }
    else
    {
        fillRectangle(ball_x,ball_y,BALL_WIDTH,BALL_HEIGHT,RGBToWord(255,255,0));
    }
}
void moveBall(uint32_t newX, uint32_t newY)
{
    // using the hideBall() and showBall() the ball is able to move
    hideBall();
    ball_x = newX;
    ball_y = newY;
    showBall();
}
static uint32_t prbs_shift_register = 0;
void randomize(void)
{
    while(prbs_shift_register == 0) // can't have a value of zero here
        prbs_shift_register=milliseconds;
    
}
uint32_t random(uint32_t lower, uint32_t upper)
{
    // generates a random bit
    uint32_t new_bit=0;    
    uint32_t return_value;
    new_bit= ((prbs_shift_register & (1<<27))>>27) ^ ((prbs_shift_register & (1<<30))>>30);
    new_bit= ~new_bit;
    new_bit = new_bit & 1;
    prbs_shift_register=prbs_shift_register << 1;
    prbs_shift_register=prbs_shift_register | (new_bit);
    return_value = prbs_shift_register;
    return_value = (return_value)%(upper-lower)+lower;
    return return_value;
}
void redOn(void)
{
    // red LED on port PB0 on
    GPIOB->ODR |= (1<<0);
}
void redOff(void)
{
    // red LED on port PB0 off
    GPIOB->ODR &= ~(1<<0); 
}
void blueOn(void)
{
    // Blue LED on port PB3 on
    GPIOB->ODR |= (1<<3); 
}
void blueOff(void)
{
    // Blue LED on port PB3 off
    GPIOB->ODR &= ~(1<<3); 
}
void flashRed(int times, int rate)
{
    // Red LED flashes a certain amount of times at a rate
    int temp = 0;
    while (temp < times)
    {
        redOn();
        playNote(C5,rate);
        redOff();
        playNote(B4,rate);
        temp++;
    }
    playNote(C5,2*rate);
}
void flashBlue(int times, int rate)
{
    // Blue LED flashes a certain amount of times at a rate
    int temp = 0;
    while (temp < times)
    {
        blueOn();
        playNote(C5,rate);
        blueOff();
        playNote(B4,rate);
        temp++;
    }
    playNote(C5,2*rate);
}
void flashBoth(int times, int rate)
{
    // Both LED flashes a certain amount of times at a rate
    int temp = 0;
    while (temp < times)
    {
        blueOn();
        redOff();
        playNote(B4,rate);
        blueOff();
        redOn();
        playNote(C5,rate);
        temp++;
    }
    playNote(C5,rate);
}
void ScoreCount()
{
    // keeps track of the goals and score, calls the gameOver() when the score limit is reached
    static int playerOneScore = 0;
    static int playerTwoScore = 0;

    if (ball_y == (PLAYERONE_GOAL + BAT_HEIGHT))
    {
        moveBall(random(10, SCREEN_WIDTH-10), SCREEN_HEIGHT/2);
        playerTwoScore++;
        printNumber(playerTwoScore, 5, 5, RGBToWord(50,50,255), 0);
        flashBlue(5,200);
        delay(500);
    }
    if (ball_y == (PLAYERTWO_GOAL - BAT_HEIGHT))
    {
        moveBall(random(10, SCREEN_WIDTH-10), SCREEN_HEIGHT/2);
        playerOneScore++;
        printNumber(playerOneScore, 5, SCREEN_HEIGHT - 10  , RGBToWord(255,50,50), 0);
        flashRed(5,200);
        delay(500);
    }
    int oldPlayerOneScore = playerOneScore;
    int oldPlayerOnTwoScore = playerTwoScore;
 
    if (playerOneScore == END_SCORE)
    {
        playerOneScore = 0;
        playerTwoScore = 0;
        flashBoth(2,500);
        gameOver(1,oldPlayerOneScore,oldPlayerOnTwoScore);
    }
    if (playerTwoScore == END_SCORE)
    {
        playerOneScore = 0;
        playerTwoScore = 0;
        flashBoth(2,500);
        gameOver(2,oldPlayerOneScore,oldPlayerOnTwoScore);
    }
}
void playNote(uint32_t Freq,uint32_t duration)
{
    TIM14->CR1 = 0; // Set Timer 14 to default values
    TIM14->CCMR1 = (1 << 6) + (1 << 5);
    TIM14->CCER |= (1 << 0);
    TIM14->PSC = 48000000UL/65536UL; // yields maximum frequency of 21kHz when ARR = 2;
    TIM14->ARR = (48000000UL/(uint32_t)(TIM14->PSC))/((uint32_t)Freq);
    TIM14->CCR1 = TIM14->ARR/2;    
    TIM14->CNT = 0;
    TIM14->CR1 |= (1 << 0);
    uint32_t end_time=milliseconds+duration;
    while(milliseconds < end_time);
    TIM14->CR1 &= ~(1u << 0);
}
void initTimer()
{
    // Power up the timer module
    RCC->APB1ENR |= (1 << 8);
    pinMode(GPIOB,1,2); // Assign a non-GPIO (alternate) function to GPIOB bit 1
    GPIOB->AFR[0] &= ~(0x0fu << 4); // Assign alternate function 0 to GPIOB 1 (Timer 14 channel 1)
    TIM14->CR1 = 0; // Set Timer 14 to default values
    TIM14->CCMR1 = (1 << 6) + (1 << 5);
    TIM14->CCER |= (1 << 0);
    TIM14->PSC = 48000000UL/65536UL; // yields maximum frequency of 21kHz when ARR = 2;
    TIM14->ARR = (48000000UL/(uint32_t)(TIM14->PSC))/((uint32_t)440);
    TIM14->CCR1 = TIM14->ARR/2;    
    TIM14->CNT = 0;
}
void playoutroSong() 
{
    // Define note durations in milliseconds  
    uint32_t quarterNote = 500;
    uint32_t eighthNote = quarterNote / 2;

    // Play the intro song
    playNote(E4, quarterNote);
    playNote(G4, eighthNote);
    playNote(A4, eighthNote);
    playNote(G4, quarterNote);
    playNote(B4, quarterNote);
    playNote(A4, quarterNote);
    playNote(G4, eighthNote);
    playNote(G4, eighthNote);
    playNote(A4, quarterNote);
    playNote(B4, quarterNote);
    playNote(D5, quarterNote);
    playNote(G4, quarterNote);
}
int rightButtonPlayer1()
{
    if ((GPIOB->IDR & (1 << 4)) == 0)
    {
        return 1;
    }
    return 0;
}
int leftButtonPlayer1()
{
    if ((GPIOA->IDR & (1 << 11)) == 0)
    {
        return 1;
    }
    return 0;
}
int rightButtonPlayer2()
{
    if ((GPIOB->IDR & (1 << 5)) == 0)
    {
        return 1;
    }
    return 0;
}
int leftButtonPlayer2()
{
    if ((GPIOA->IDR & (1 << 8)) == 0)
    {
        return 1;
    }
    return 0;
}
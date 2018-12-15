/*********************************************************************
This is an example sketch for our Monochrome Nokia 5110 LCD Displays
  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/338
These displays use SPI to communicate, 4 or 5 pins are required to
interface
Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!
Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

// Software SPI (slower updates, more flexible pin options):
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
//Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);

// Hardware SPI (faster, but must use certain hardware pins):
// SCK is LCD serial clock (SCLK) - this is pin 13 on Arduino Uno
// MOSI is LCD DIN - this is pin 11 on an Arduino Uno
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
 Adafruit_PCD8544 display = Adafruit_PCD8544(5, 4, 3);
// Note with hardware SPI MISO and SS pins aren't used but will still be read
// and written to during SPI transfer.  Be careful sharing these pins!

const uint8_t up = 1;
const uint8_t right = 2;
const uint8_t down = 3;
const uint8_t left = 4;  
const uint8_t apple = 10;                              


uint8_t dir;

const uint8_t ROWS = 12;
const uint8_t COLS = 21;

const uint8_t up_pin = 7;
const uint8_t down_pin = 6;
const uint8_t left_pin = 9;
const uint8_t right_pin = 8;

uint8_t board[ROWS][COLS];

uint8_t front[2], back[2];    

// variables for intervals between tasks
int move_forward_interval = 200;
long move_forward_last = millis();

// level and score variables
int level = 1;
int score = 0;

// apple variables
bool ate_apple = false;
bool add_to_back = false;
int num_apples_ate = 0;
uint8_t apple_pos[2];

void setup()   
{
  for(uint8_t i = 0; i < ROWS; i++)
    for(uint8_t j = 0; j < COLS; j++)
      board[i][j] = 0;

  front[0] = back[0] = ROWS/2;
  back[1] = 0;
  front[1] = 0;
  
  board[front[0]][front[1]] = right;
  board[back[0]][back[1]] = right;
  
  dir = right;
  
  pinMode(up_pin, INPUT);
  pinMode(down_pin, INPUT);
  pinMode(left_pin, INPUT);
  pinMode(right_pin, INPUT);
  
  Serial.begin(9600);

  display.begin();

  // you can change the contrast around to adapt the display
  // for the best viewing!
  display.setContrast(60);

  display.display(); // show splashscreen
  delay(200);
  display.clearDisplay();   // clears the screen and buffer

  reset();
  
  drawBoard();
}

void loop()
{
  moveSnake();
  
  if ( millis() - move_forward_last > move_forward_interval)
  {
    if (apple_in_front())
      eat_apple();
    else 
    {
      if (isDead())
      {
        printEndScreen();
        reset();
      }
      
      move_forward_last = millis();
      moveForward();
    }
  }

  drawBoard();
}

bool apple_in_front()
{
  if (dir == up)
    return board[front[0]+1][front[1]] == apple;
  else if (dir == right)
    return board[front[0]][front[1]+1] == apple;
  else if (dir == down)
    return board[front[0]-1][front[1]] == apple;
  else if (dir == left)
    return board[front[0]][front[1]-1] == apple;
}

void eat_apple()
{
  score += level*10;
  num_apples_ate++;

  ate_apple = true;
  add_to_back = true;

  if (num_apples_ate >= 3)
  {
    level++;
    num_apples_ate = 0;

    if (move_forward_interval > 50)
      move_forward_interval -= 25;
  }

  board[apple_pos[0]][apple_pos[1]] = dir;
  front[0] = apple_pos[0];
  front[1] = apple_pos[1];
  
  create_apple();
}

void create_apple()
{
  int row = random(0, ROWS);
  int col = random(0, COLS);

  while(board[row][col] != 0)
  {
    row = random(0, ROWS);
    col = random(0, COLS);
  }

  apple_pos[0] = row;
  apple_pos[1] = col;
  board[row][col] = apple;
}

void printEndScreen()
{
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(BLACK);
  display.setCursor(0,0);

  display.println("GAME");
  display.println("   OVER");

  display.display();

  delay(200);

  while( !digitalRead(up_pin) && !digitalRead(down_pin) && !digitalRead(left_pin) && !digitalRead(right_pin) )
  {}

  display.clearDisplay();
  display.setTextSize(1);

  display.setCursor(0,10);
  display.print("Level:");
  display.println(level);

  display.setCursor(0,25);
  display.print("Score:");
  display.println(score);

  display.display();

  delay(200);

  while( !digitalRead(up_pin) && !digitalRead(down_pin) && !digitalRead(left_pin) && !digitalRead(right_pin) )
  {}
  
}

void moveSnake()
{
  // read the controller to see if a button was pushed
  uint8_t d = read_controller();

  // if 0 was returned, no button was pushed, 
  // or if the new direction equals the old direction then exit
  if ( (d == 0) || (d == dir) )
    return;
  else if ( ( (dir == right) || (dir == left) ) && ( (d != up) && (d != down) ) )
    return;
  else if ( ( (dir == up) || (dir == down) ) && ( (d != right) && (d != left) ) )
    return;

  // set direction of current spot to reflect the change
  board[front[0]][front[1]] = d;

  // set direction of the snake to new direction
  dir = d;

}

void drawBoard()
{
  //display.clearDisplay();
  
  for(uint8_t i = 0; i < ROWS; i++)
    for(uint8_t j = 0; j < COLS; j++)
    {
      if (board[i][j] != 0)
        display.fillRect(j*4, i*4, 4, 4, BLACK);
      else
         display.fillRect(j*4, i*4, 4, 4, WHITE);
    }

  display.drawRect(0, 0, 84, 48, BLACK);

  display.display();
}

uint8_t read_controller()
{
  if (digitalRead(up_pin))
    return up;
  else if (digitalRead(down_pin))
    return down;
  else if (digitalRead(left_pin))
    return left;
  else if (digitalRead(right_pin))
    return right;

  return 0;
}

void reset()
{
  for(uint8_t i = 0; i < ROWS; i++)
    for(uint8_t j = 0; j < COLS; j++)
      board[i][j] = 0;

  front[0] = back[0] = ROWS/2;
  back[1] = 0;
  front[1] = 1;
  
  board[front[0]][front[1]] = right;
  board[back[0]][back[1]] = right;
  
  dir = right;

  level = 1;
  score = 0;

  create_apple();

  move_forward_interval = 200;  
}

bool isDead()
{
  // if moving UP and at the TOP or if moving up and body right above then dead
  if (dir == up && (front[0] == ROWS-1 || board[front[0]+1][front[1]] != 0) )
    return true;

  // if moving RIGHT and at the far RIGHT or if moving RIGHT and body is to the right then dead
  if (dir == right && (front[1] == COLS-1 || board[front[0]][front[1]+1] != 0) )
    return true;

  // if moving DOWN and at the BOTTOM or if moving DOWN and body right below then dead
  if (dir == down && (front[0] == 0 || board[front[0]-1][front[1]] != 0) )
    return true;

  // if moving LEFT and at the far LEFT or if moving LEFT and body is to the left then dead
  if (dir == left && (front[1] == 0 || board[front[0]][front[1]-1] != 0) )
    return true;
    
  return false;
}

void moveForward()
{
  // get the direction the back side was moving when it was set
  int back_direction = board[back[0]][back[1]];
    
  if (dir == up)
    front[0]++;

  else if (dir == right)
    front[1]++;

  else if (dir == down)
    front[0]--;

  else if (dir == left)
    front[1]--;
    
  // if snake ate apple and we need to add to the back of snake
  // skip part that moves back the snake
  /*
  if (add_to_back)
  {
    add_to_back = false;
    return;
  }
  */
  
  board[front[0]][front[1]] = dir;
  
  board[back[0]][back[1]] = 0;
    
  if (back_direction == up)
    back[0]++;
 
  else if (back_direction == right)
    back[1]++;

  else if (back_direction == down)
    back[0]--;

  else if (back_direction == left)
    back[1]--;
}

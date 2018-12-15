#include "Snake.h"

  Snake::Snake(int inRows, int inCols, int (*inBoard)[48][84])
  {
    // set up number of columns and rows
    rows = inRows;
    cols = inCols;

    // intially moving to the right
    dir = right;

    board[0][0] = inBoard[0][0];

    // starts in the middle row, all the way to the left
    *board[rows/2][0] = dir;

    // save the position of the front and back of the snake
    front[0] = back[0] = rows/2;
    front[1] = back[1] = 0;

  }
  
  void Snake::reset()
  {
    for(int i = 0; i < rows; i++)
      for(int j = 0; j < cols; j++)
        board[i][j] = 0;

    *board[rows/2][0] = right;

    front[0] = back[0] = rows/2;
    front[1] = back[1] = 0;
  }

  bool Snake::isDead()
  {
    // if moving UP and at the TOP or if moving up and body right above then dead
    if (dir == up && (front[0] == rows-1 || board[front[0]+1][front[1]] != 0) )
      return true;

   // if moving RIGHT and at the far RIGHT or if moving RIGHT and body is to the right then dead
    if (dir == right && (front[1] == cols+1 || board[front[0]][front[1]+1] != 0) )
      return true;

   // if moving DOWN and at the BOTTOM or if moving DOWN and body right below then dead
    if (dir == down && (front[0] == 0 || board[front[0]-1][front[1]] != 0) )
      return true;

   // if moving LEFT and at the far LEFT or if moving LEFT and body is to the left then dead
    if (dir == left && (front[1] == 0 || board[front[0]][front[1]-1] != 0) )
      return true;
    
    return false;
  }

  void Snake::moveForward()
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
    
    *board[front[0]][front[1]] = dir;
    
    *board[back[0]][back[1]] = 0;
    
    if (back_direction == up)
      back[0]++;
 
    else if (back_direction == right)
      back[1]++;

    else if (back_direction == down)
      back[0]--;

    else if (back_direction == left)
      back[1]--;
  }

  void Snake::setDirection(directions d)
  {
    dir = d;
  }


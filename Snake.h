#ifndef _SNAKE_H
#define _SNAKE_H



enum directions {
                    blank,
                    up,
                    right,
                    down,
                    left                                     
                 };

class Snake 
{

  public:
  
  Snake(int inRows, int inCols, int (*inBoard)[48][84]);
  
  void reset();
  
  bool isDead();

  void moveForward();

  void setDirection(directions d);

  private:

  int rows, cols;
  directions dir;
  int front[2], back[2];
  int *board[48][84];
};

#endif

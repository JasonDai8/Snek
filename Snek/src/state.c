#include "state.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);

/* Task 1 */
game_state_t* create_default_state() {
  // TODO: Implement this function.
  // allocating memory
  game_state_t* state = malloc(sizeof(game_state_t));
  state->num_snakes = 1;
  state->snakes = malloc(state->num_snakes*sizeof(snake_t));
  //setting stuff
  state->num_rows = 18;
  state->snakes->live = true;
  //snake coord setup
  state->snakes->head_row = 2;
  state->snakes->head_col = 4;
  state->snakes->tail_row = 2;
  state->snakes->tail_col = 2;
  //setup memory for board
  state->board = calloc(18, sizeof(char*));
  for (unsigned int i = 0; i < 18; i++) {
    state->board[i] = calloc(21, sizeof(char));
    for (unsigned int j = 0; j < 20; j++) {
      if (i == 0 || i == 17 || j == 0 || j == 19) {
        state->board[i][j] = '#';
      } else {
        state->board[i][j] = ' ';
      }
    }
    state->num_snakes = 1;
    state->board[i][20] = '\n';
    
  }

  state->board[2][9] = '*';
  state->board[2][4] = 'D';
  state->board[2][2] = 'd';
  state->board[2][3] = '>';
  state->snakes->live = true;
  return state;
  
}

/* Task 2 */
void free_state(game_state_t* state) {
  // TODO: Implement this function.
  for(unsigned int i = 0; i < state->num_rows; i++) {
    free(state->board[i]);
  }
  free(state->board);
  free(state->snakes); 
  free(state);
  return;
}

/* Task 3 */
void print_board(game_state_t* state, FILE* fp) {
  // TODO: Implement this function.
  for(unsigned int i = 0; i < state->num_rows; i++) {
    fprintf(fp, "%s", state->board[i]);
  }
  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  // TODO: Implement this function.
  if (c=='w' || c=='a' || c=='s' || c=='d') {
    return true;
  }
  else {
    return false;
  }
  return true;
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  // TODO: Implement this function.
  if (c=='W' || c=='A' || c=='S' || c=='D' || c=='x') {
    return true;
  }
  else {
    return false;
  }
  return true;
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implement this function.
  switch(c) {
      case 'w':
          return true;
      case 'a':
          return true;
      case 's' :
          return true;
      case 'd':
          return true;
      case '^':
          return true;
      case '<':
          return true;
      case 'v':
          return true;
      case '>':
          return true;
      case 'W':
          return true;
      case 'A': 
          return true;
      case 'S':
          return true;
      case 'D':
          return true;
      case 'x':
          return true;
      default:
          return false;
  }
  return true;
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implement this function.
  switch(c) {
    case '^':
        return 'w';
    case '<':
        return 'a';
    case 'v':
        return 's';
    case '>':
        return 'd';
    default:
        return '?';
  }
  return '?';
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
  switch(c) {
      case 'W':
          return '^';
      case 'A': 
          return '<';
      case 'S':
          return 'v';
      case 'D':
          return '>';
      default:
          return '?';
  }
  return '?';
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
   if (c == 'v' || c == 's' || c == 'S') {
    return cur_row + 1;
  } else if (c == '^' || c == 'w' || c == 'W') {
    return cur_row - 1;
  } else {
    return cur_row;
  }
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
  if (c=='>' || c=='d' || c=='D') {
      return cur_col+1;
  }
  else if (c=='<' || c=='a' || c=='A') {
      return cur_col-1;
  }
  return cur_col;
}



/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t curr_snake = state->snakes[snum];
  unsigned int xidx = curr_snake.head_row;
  unsigned int yidx = curr_snake.head_col;
  char head = state->board[xidx][yidx];
  xidx = get_next_row(xidx, head);
  yidx = get_next_col(yidx, head);
  return state->board[xidx][yidx];
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  unsigned int xidx = state->snakes[snum].head_row;
  unsigned int yidx = state->snakes[snum].head_col;
  char head = state->board[xidx][yidx];
  char body = head_to_body(head);
  state->board[xidx][yidx] = body;
  unsigned int newx = get_next_row(xidx, head);
  unsigned int newy = get_next_col(yidx, head);
  state->board[newx][newy] = head;
  state->snakes[snum].head_row = newx;  
  state->snakes[snum].head_col = newy;         
  return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  unsigned int xidx = state->snakes[snum].tail_row;
  unsigned int yidx = state->snakes[snum].tail_col;
  char tail = state->board[xidx][yidx];
  char blank = ' ';
  state->board[xidx][yidx] = blank;
  xidx = get_next_row(xidx, tail);
  yidx = get_next_col(yidx, tail);
  char body2 = state->board[xidx][yidx];
  char newTail = body_to_tail(body2);
  state->board[xidx][yidx] = newTail;
  state->snakes[snum].tail_row = xidx;  
  state->snakes[snum].tail_col = yidx;
  return;
}

/* Task 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  // TODO: Implement this function.
  for(unsigned int x = 0; x < state->num_snakes; x++){
      char next = next_square(state, x);
      if(next == ' '){
          update_head(state, x);
          update_tail(state, x);
      }
      else if(next == '*'){
        update_head(state,x);
        add_food(state);
      }
      else {
        //kill snake
        unsigned int xidx = state->snakes[x].head_row;
        unsigned int yidx = state->snakes[x].head_col;
        state->board[xidx][yidx] = 'x';
        //kill snake
        state->snakes[x].live = false;
        //update snake struct
     }
  }
}
/* Task 5 */
game_state_t* load_board(char* filename) {
  FILE* file = fopen(filename, "r");
  game_state_t* state = malloc(sizeof(game_state_t));
  state->snakes = malloc(sizeof(snake_t));
  //if what we pass in is NULL, end
  if(file==NULL) {
    return NULL; 
  }
  else if(state==NULL) {
    fclose(file);
    return NULL;
  }
   if(state->snakes == NULL) {
    free_state(state);
    fclose(file);
    return NULL;
   }
  //setting up for getline
  state->num_rows = 0;
  size_t row_length = 0;
  char* row = NULL;
  //running thru the file
  unsigned int row_content = &row;
  unsigned int row_length_content = &row;
  while(getline(&row, &row_length, file) != -1) {
    size_t row_alloc = strlen(row)+1;
    char* new_row = malloc(row_alloc*sizeof(char));
    //check if the row is empty
    if(new_row!=NULL) {
      strcpy(new_row, row);
      unsigned int row_num = state->num_rows+1;
      state->board = realloc(state->board, row_num*sizeof(char*));
      //check if the board is empty
      if(state->board==NULL) {
        if(new_row==NULL) {
          free(new_row);
          free_state(state);
          fclose(file);
          return NULL;
        }
      }
    }
    else {
      free_state(state); 
      fclose(file);
      return NULL;
    }
    unsigned int curr_row = state->num_rows;
    state->board[curr_row] = new_row;
    state->num_rows++;
  }
  //finished copying row over
  free(row);
  fclose(file);
  return state;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  int tailx = state->snakes[snum].tail_row;
  int taily = state->snakes[snum].tail_col;
  char currindx = state->board[tailx][taily];
  while(is_head(currindx) == false){
    tailx = get_next_row(tailx, currindx);
    taily = get_next_col(taily, currindx);
    currindx = state->board[tailx][taily];
  }
  state->snakes[snum].head_row = tailx;
  state->snakes[snum].head_col = taily;
  return;
}

/* Task 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  // TODO: Implement this function.
  int counter = 0;
  for(int a = 0; a < state->num_rows; a++){
      for(int b = 0; b < strlen(state->board[a]); b++) {
          if(is_tail(state->board[a][b])) {
              counter++;
          }
      }
  }
  state->snakes = malloc((counter)*sizeof(snake_t));
  state->num_snakes = counter;
  int index = 0;
  for(int x = 0; x < state->num_rows; x++) {
      for(int y = 0; y < strlen(state->board[x]); y++) {
          if(is_tail(state->board[x][y])) {
              state->snakes[index].tail_row = x;
              state->snakes[index].tail_col = y;
              state->snakes[index].live = true;
              find_head(state, index);
              index++;
          }
      }
  }
  return state;
}

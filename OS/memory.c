#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct freememory{
  struct freememory *front;
  struct freememory *back;
  int space;
};

int r;
struct freememory *ind;

void sort() {

}

void init() {
  r = rand() % 10;
  ind = malloc(sizeof(struct freememory));
  ind->front = NULL;
  ind->back = NULL;
  ind->space = rand() % 1024;
  struct freememory *temp = ind;
  for (int i = 1; i < r; i++) {
    struct freememory *new = malloc(sizeof(struct freememory));
    new->front = temp;
    temp->back = new;
    new->back = NULL;
    new->space = rand() % 1024;
  }
  sort();
}

void delete() {
  for (int i = 0; i < r; i++) {
    struct freememory *temp = ind;
    ind = ind->back;
    free(temp);
  }
}

void printStatus() {
  struct freememory *temp = ind;
  printf("---------------------");
  printf("| Number | Size(KB) |");
  for (int i = 0; i < r; i++) {
    printf("|    %d   |   %4d   |", i, temp->space);
    temp = temp->back;
  }
}

int main() {
  init();
  printStatus();
  printStatus();
  delete();
  return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//Board in Memory

struct board {
  char *pieces;
  char turn;
  char lastPawn;
  char kingW;
  char kingB;
  //Castle Booleans
  char WL,WR,BL,BR;
  char drawCount;
};

char x(char i) {
  return i%8;
}

char y(char i) {
  return 7-i/8;
}

char i(char x,char y) {
  if(x<0 || x>7 || y<0 || y>7) {
    return -1;
  }
  return x + 8*(7 - y);
}

struct board *newBoard() {
  struct board *b = malloc(sizeof(struct board));
  (*b).turn = 1;
  (*b).lastPawn = -1;
  (*b).WL = (*b).WR = (*b).BL = (*b).BR = 1;
  (*b).drawCount = 0;
  (*b).pieces = malloc(64*sizeof(char));
  char j;
  for(j = 0; j<64; j++) {
    (*b).pieces[j] = ' ';
  }
  for(j=0; j<8; j++) {
    (*b).pieces[i(j,6)] = 'p';
  }
  for(j=0; j<8; j++) {
    (*b).pieces[i(j,1)] = 'P';
  }
  (*b).pieces[0] = (*b).pieces[7] = 'r';
  (*b).pieces[56] = (*b).pieces[63] = 'R';
  (*b).pieces[1] = (*b).pieces[6] = 'n';
  (*b).pieces[57] = (*b).pieces[62] = 'N';
  (*b).pieces[2] = (*b).pieces[5] = 'b';
  (*b).pieces[58] = (*b).pieces[61] = 'B';
  (*b).pieces[3] = 'q';
  (*b).pieces[59] = 'Q';
  (*b).pieces[4] = 'k';
  (*b).kingB = 4;
  (*b).pieces[60] = 'K';
  (*b).kingW = 60;
  return b;
}

void freeBoard(struct board *b) {
  free((void *)((*b).pieces));
  free((void*)b);
}

char upper(char c) {
  if(c>='a' && c<='z') return c-'a'+'A';
  else return c;
}

void printBoard(struct board *b) {
  printf("  ---------------------------------\n");
  char j, p;
  for(j=0; j<64; j++) {
    if(j%8==0) {
      printf("%d |",8-j/8);
    }
    p = (*b).pieces[j];
    if(p==' ') {
      printf("   |");
    } else if(p=='P' || p=='N' || p=='B' || p=='R' || p=='Q' || p=='K') {
      printf(" \033[0;36m%c\033[0m |",p);
    } else {
      printf(" \033[0;31m%c\033[0m |",upper(p));
    }
    if (j%8==7) {
      printf("\n  ---------------------------------\n");
    }
  }
  printf("    a   b   c   d   e   f   g   h\n");
}

//Game Logic

struct moveSet {
  int moveCount;
  struct board *moves;
};

char color(char p) {
  return p=='P' || p=='R' || p=='N' || p=='B' || p=='Q' || p=='K';
}

char isRank(char c) {
  return c=='1' || c=='2' || c=='3' || c=='4' || c=='5' || c=='6' || c=='7' || c=='8';
}

char isFile(char c) {
  return c=='a' || c=='b' || c=='c' || c=='d' || c=='e' || c=='f' || c=='g' || c=='h';
}

void relu(float *x) {
  if((*x)<0) {
    *x = 0;
  }
}

char getPiece(struct board *b,char j) {
  if(j==-1) return 0;
  return (*b).pieces[j];
}

char checkCheck(struct board *b) {
  char kX, kY;
  if((*b).turn) {
    kX = x((*b).kingW);
    kY = y((*b).kingW);
    //Illegal King Checks lol:
    if(getPiece(b,i(kX+1,kY))=='k' || getPiece(b,i(kX+1,kY+1))=='k' || getPiece(b,i(kX,kY+1))=='k' || getPiece(b,i(kX-1,kY+1))=='k' || getPiece(b,i(kX-1,kY))=='k' || getPiece(b,i(kX-1,kY-1))=='k' || getPiece(b,i(kX,kY-1))=='k' || getPiece(b,i(kX+1,kY-1))=='k') {
      return 1;
    }
    //Pawn Checks:
    if(getPiece(b,i(kX+1,kY+1))=='p'|| getPiece(b,i(kX-1,kY+1))=='p') {
      return 1;
    }
    //Knight Checks:
    if(getPiece(b,i(kX+2,kY+1))=='n' || getPiece(b,i(kX+1,kY+2))=='n' || getPiece(b,i(kX-1,kY+2))=='n' || getPiece(b,i(kX-2,kY+1))=='n' || getPiece(b,i(kX-2,kY-1))=='n' || getPiece(b,i(kX-1,kY-2))=='n' || getPiece(b,i(kX+1,kY-2))=='n' || getPiece(b,i(kX+2,kY-1))=='n') {
      return 1;
    }
    //Horizontal Checks:
    char j,p;
    for(j=kX+1;j<8;j++) {
      p = (*b).pieces[i(j,kY)];
      if(p=='r' || p=='q') return 1;
      if(p!=' ') break;
    }
    for(j=kX-1;j>-1;j--) {
      p = (*b).pieces[i(j,kY)];
      if(p=='r' || p=='q') return 1;
      if(p!=' ') break;
    }
    for(j=kY+1;j<8;j++) {
      p = (*b).pieces[i(kX,j)];
      if(p=='r' || p=='q') return 1;
      if(p!=' ') break;
    }
    for(j=kY-1;j>-1;j--) {
      p = (*b).pieces[i(kX,j)];
      if(p=='r' || p=='q') return 1;
      if(p!=' ') break;
    }
    //Diagonal Checks:
    char j2;
    j = kX+1;
    j2 = kY+1;
    while(j<8 && j2<8) {
      p = (*b).pieces[i(j,j2)];
      if(p=='b' || p=='q') return 1;
      if(p!=' ') break;
      j++;
      j2++;
    }
    j = kX-1;
    j2 = kY+1;
    while(j>-1 && j2<8) {
      p = (*b).pieces[i(j,j2)];
      if(p=='b' || p=='q') return 1;
      if(p!=' ') break;
      j--;
      j2++;
    }
    j = kX-1;
    j2 = kY-1;
    while(j>-1 && j2>-1) {
      p = (*b).pieces[i(j,j2)];
      if(p=='b' || p=='q') return 1;
      if(p!=' ') break;
      j--;
      j2--;
    }
    j = kX+1;
    j2 = kY-1;
    while(j<8 && j2>-1) {
      p = (*b).pieces[i(j,j2)];
      if(p=='b' || p=='q') return 1;
      if(p!=' ') break;
      j++;
      j2--;
    }
  } else {
    kX = x((*b).kingB);
    kY = y((*b).kingB);
    //Illegal King Checks lol:
    if(getPiece(b,i(kX+1,kY))=='K' || getPiece(b,i(kX+1,kY+1))=='K' || getPiece(b,i(kX,kY+1))=='K' || getPiece(b,i(kX-1,kY+1))=='K' || getPiece(b,i(kX-1,kY))=='K' || getPiece(b,i(kX-1,kY-1))=='K' || getPiece(b,i(kX,kY-1))=='K' || getPiece(b,i(kX+1,kY-1))=='K') {
      return 1;
    }
    //Knight Checks:
    if(getPiece(b,i(kX+2,kY+1))=='N' || getPiece(b,i(kX+1,kY+2))=='N' || getPiece(b,i(kX-1,kY+2))=='N' || getPiece(b,i(kX-2,kY+1))=='N' || getPiece(b,i(kX-2,kY-1))=='N' || getPiece(b,i(kX-1,kY-2))=='N' || getPiece(b,i(kX+1,kY-2))=='N' || getPiece(b,i(kX+2,kY-1))=='N') {
      return 1;
    }
    //Pawn Checks:
    if(getPiece(b,i(kX+1,kY-1))=='P'|| getPiece(b,i(kX-1,kY-1))=='P') {
      return 1;
    }
    //Horizontal Checks:
    char j,p;
    for(j=kX+1;j<8;j++) {
      p = (*b).pieces[i(j,kY)];
      if(p=='R' || p=='Q') return 1;
      if(p!=' ') break;
    }
    for(j=kX-1;j>-1;j--) {
      p = (*b).pieces[i(j,kY)];
      if(p=='R' || p=='Q') return 1;
      if(p!=' ') break;
    }
    for(j=kY+1;j<8;j++) {
      p = (*b).pieces[i(kX,j)];
      if(p=='R' || p=='Q') return 1;
      if(p!=' ') break;
    }
    for(j=kY-1;j>-1;j--) {
      p = (*b).pieces[i(kX,j)];
      if(p=='R' || p=='Q') return 1;
      if(p!=' ') break;
    }
    //Diagonal Checks:
    char j2;
    j = kX+1;
    j2 = kY+1;
    while(j<8 && j2<8) {
      p = (*b).pieces[i(j,j2)];
      if(p=='B' || p=='Q') return 1;
      if(p!=' ') break;
      j++;
      j2++;
    }
    j = kX-1;
    j2 = kY+1;
    while(j>-1 && j2<8) {
      p = (*b).pieces[i(j,j2)];
      if(p=='B' || p=='Q') return 1;
      if(p!=' ') break;
      j--;
      j2++;
    }
    j = kX-1;
    j2 = kY-1;
    while(j>-1 && j2>-1) {
      p = (*b).pieces[i(j,j2)];
      if(p=='B' || p=='Q') return 1;
      if(p!=' ') break;
      j--;
      j2--;
    }
    j = kX+1;
    j2 = kY-1;
    while(j<8 && j2>-1) {
      p = (*b).pieces[i(j,j2)];
      if(p=='B' || p=='Q') return 1;
      if(p!=' ') break;
      j++;
      j2--;
    }
  }
  return 0;
}

char getPromotion(char c) {
  printf("Please input your desired promotion (N,B,R,Q)\n");
  char input[2];
  scanf("%s",input);
  if((input[0]=='Q' || input[0]=='N' || input[0]=='R' || input[0]=='B') && input[1]==0) {
      if(c) return input[0];
      return input[0]-'A'+'a';
  } else {
      printf("\nInvalid piece.");
      return getPromotion(c);
  }
}

char humanMove(struct board * b) {
  printf("Enter a move:\n");
  char pendingMove[5];
  scanf("%4s",pendingMove);
  if(pendingMove[0]=='E' && pendingMove[1]==0) {
    return 0;
  } else if(isFile(pendingMove[0])&&isRank(pendingMove[1])&&isFile(pendingMove[2])&&isRank(pendingMove[3])&&pendingMove[4]==0) {
    char originX = pendingMove[0]-'a';
    char originY = pendingMove[1]-'1';
    char origin = i(originX,originY);
    char currentPiece = (*b).pieces[origin];
    char destinationX = pendingMove[2]-'a';
    char destinationY = pendingMove[3]-'1';
    char destination = i(destinationX,destinationY);
    char destinationSpace = (*b).pieces[destination];
    //printf("\n%d %d %d %c %d %d %d %c\n",originX,originY,origin,currentPiece,destinationX,destinationY,destination,destination);
    if((*b).pieces[origin]==' ' || color(currentPiece)!=(*b).turn || origin == destination || (destinationSpace!=' ' && color(currentPiece)==color(destinationSpace))) {
      printf("\nInvalid move.1\n");
      return humanMove(b);
    }
    if(currentPiece=='P') {
      if(destinationX==originX && destinationY==originY+1 && (*b).pieces[destination]==' ') {
        (*b).pieces[destination] = 'P';
        (*b).pieces[origin] = ' ';
        if(checkCheck(b)) {
          (*b).pieces[destination] = destinationSpace;
          (*b).pieces[origin] = currentPiece;
          printf("\nInvalid move.2\n");
          return humanMove(b);
        }
        (*b).lastPawn = -1;
        (*b).turn = !(*b).turn;
        if(destinationY==7) {
          (*b).pieces[destination] = getPromotion(1);
        }
        (*b).drawCount = 0;
        return 1;
      } else if(destinationX==originX && destinationY==3 && originY==1 && (*b).pieces[destination]==' ' && (*b).pieces[i(originX,2)]==' ') {
        (*b).pieces[destination] = 'P';
        (*b).pieces[origin] = ' ';
        if(checkCheck(b)) {
          (*b).pieces[destination] = destinationSpace;
          (*b).pieces[origin] = currentPiece;
          printf("\nInvalid move.3\n");
          return humanMove(b);
        }
        (*b).lastPawn = destinationX;
        (*b).turn = !(*b).turn;
        (*b).drawCount = 0;
        return 1;
      } else if(abs(destinationX-originX)==1 && destinationY==originY+1 && destinationSpace!=' ') {
        (*b).pieces[destination] = 'P';
        (*b).pieces[origin] = ' ';
        if(checkCheck(b)) {
          (*b).pieces[destination] = destinationSpace;
          (*b).pieces[origin] = currentPiece;
          printf("\nInvalid move.4\n");
          return humanMove(b);
        }
        if(destinationY==7) {
          (*b).pieces[destination] = getPromotion(1);
        }
        (*b).lastPawn = -1;
        (*b).turn = !(*b).turn;
        (*b).drawCount = 0;
        return 1;
      } else if(abs(destinationX-originX)==1 && destinationY==5 && originY==4 && destinationX==(*b).lastPawn) {
        (*b).pieces[destination] = 'P';
        (*b).pieces[origin] = ' ';
        (*b).pieces[i(destinationX,originY)] = ' ';
        if(checkCheck(b)) {
          (*b).pieces[destination] = destinationSpace;
          (*b).pieces[origin] = currentPiece;
          (*b).pieces[i(destinationX,originY)] = 'p';
          printf("\nInvalid move.5\n");
          return humanMove(b);
        }
        (*b).lastPawn = -1;
        (*b).turn = !(*b).turn;
        (*b).drawCount = 0;
        return 1;
      } else {
        printf("\nInvalid move.6\n");
        return humanMove(b);
      }
    } else if (currentPiece=='p') {
      if(destinationX==originX && destinationY==originY-1 && (*b).pieces[destination]==' ') {
        (*b).pieces[destination] = 'p';
        (*b).pieces[origin] = ' ';
        if(checkCheck(b)) {
          (*b).pieces[destination] = destinationSpace;
          (*b).pieces[origin] = currentPiece;
          printf("\nInvalid move.7\n");
          return humanMove(b);
        }
        if(destinationY==0) {
          (*b).pieces[destination] = getPromotion(0);
        }
        (*b).lastPawn = -1;
        (*b).turn = !(*b).turn;
        (*b).drawCount = 0;
        return 1;
      } else if(destinationX==originX && destinationY==4 && originY==6 && (*b).pieces[destination]==' ' && (*b).pieces[i(originX,5)]==' ') {
        (*b).pieces[destination] = 'p';
        (*b).pieces[origin] = ' ';
        if(checkCheck(b)) {
          (*b).pieces[destination] = destinationSpace;
          (*b).pieces[origin] = currentPiece;
          printf("\nInvalid move.8\n");
          return humanMove(b);
        }
        (*b).lastPawn = destinationX;
        (*b).turn = !(*b).turn;
        (*b).drawCount = 0;
        return 1;
      } else if(abs(destinationX-originX)==1 && destinationY==originY-1 && destinationSpace!=' ') {
        (*b).pieces[destination] = 'p';
        (*b).pieces[origin] = ' ';
        if(checkCheck(b)) {
          (*b).pieces[destination] = destinationSpace;
          (*b).pieces[origin] = currentPiece;
          printf("\nInvalid move.9\n");
          return humanMove(b);
        }
        if(destinationY==0) {
          (*b).pieces[destination] = getPromotion(0);
        }
        (*b).lastPawn = -1;
        (*b).turn = !(*b).turn;
        (*b).drawCount = 0;
        return 1;
      } else if(abs(destinationX-originX)==1 && destinationY==2 && originY==3 && destinationX==(*b).lastPawn) {
        (*b).pieces[destination] = 'p';
        (*b).pieces[origin] = ' ';
        (*b).pieces[i(destinationX,originY)] = ' ';
        if(checkCheck(b)) {
          (*b).pieces[destination] = destinationSpace;
          (*b).pieces[origin] = currentPiece;
          (*b).pieces[i(destinationX,originY)] = 'P';
          printf("\nInvalid move.10\n");
          return humanMove(b);
        }
        (*b).lastPawn = -1;
        (*b).turn = !(*b).turn;
        (*b).drawCount = 0;
        return 1;
      } else {
        printf("\nInvalid move.11\n");
        return humanMove(b);
      }
    } else if(currentPiece == 'N') {
      char absX = abs(destinationX-originX);
      char absY = abs(destinationY - originY);
      if((absX==2 && absY==1) || (absX==1 && absY==2)) {
        (*b).pieces[destination] = 'N';
        (*b).pieces[origin] = ' ';
        if(checkCheck(b)) {
          (*b).pieces[destination] = destinationSpace;
          (*b).pieces[origin] = currentPiece;
          printf("\nInvalid move.12\n");
          return humanMove(b);
        }
        (*b).lastPawn = -1;
        (*b).turn = !(*b).turn;
        if(destinationSpace != ' ') {
          (*b).drawCount = 0;
        } else {
          (*b).drawCount++;
        }
        return 1;
      } else {
        printf("\nInvalid move.13\n");
        return humanMove(b);
      }
    } else if(currentPiece == 'n') {
      char absX = abs(destinationX-originX);
      char absY = abs(destinationY - originY);
      if((absX==2 && absY==1) || (absX==1 && absY==2)) {
        (*b).pieces[destination] = 'n';
        (*b).pieces[origin] = ' ';
        if(checkCheck(b)) {
          (*b).pieces[destination] = destinationSpace;
          (*b).pieces[origin] = currentPiece;
          printf("\nInvalid move.14\n");
          return humanMove(b);
        }
        (*b).lastPawn = -1;
        (*b).turn = !(*b).turn;
        if(destinationSpace != ' ') {
          (*b).drawCount = 0;
        } else {
          (*b).drawCount++;
        }
        return 1;
      } else {
        printf("\nInvalid move.15\n");
        return humanMove(b);
      }
    } else if(currentPiece == 'B') {
      if(abs(destinationX-originX) != abs(destinationY-originY)) {
        printf("\nInvalid move.16\n");
        return humanMove(b);
      }
      char jx,jy;
      switch(2*(destinationY>originY) + (destinationX>originX)) {
        case 0:
        jx = originX-1;
        jy = originY-1;
        while(i(jx,jy)!=destination) {
          if((*b).pieces[i(jx,jy)]!=' ') {
            printf("\nInvalid move.17\n");
            return humanMove(b);
          }
          jx--;
          jy--;
        }
        break;
        case 1:
        jx = originX+1;
        jy = originY-1;
        while(i(jx,jy)!=destination) {
          if((*b).pieces[i(jx,jy)]!=' ') {
            printf("\nInvalid move.18\n");
            return humanMove(b);
          }
          jx++;
          jy--;
        }
        break;
        case 2:
        jx = originX-1;
        jy = originY+1;
        while(i(jx,jy)!=destination) {
          if((*b).pieces[i(jx,jy)]!=' ') {
            printf("\nInvalid move.19\n");
            return humanMove(b);
          }
          jx--;
          jy++;
        }
        break;
        case 3:
        jx = originX+1;
        jy = originY+1;
        while(i(jx,jy)!=destination) {
          if((*b).pieces[i(jx,jy)]!=' ') {
            printf("\nInvalid move.20\n");
            return humanMove(b);
          }
          jx++;
          jy++;
        }
        break;
      }
      (*b).pieces[destination] = 'B';
      (*b).pieces[origin] = ' ';
      if(checkCheck(b)) {
        (*b).pieces[destination] = destinationSpace;
        (*b).pieces[origin] = currentPiece;
        printf("\nInvalid move.21\n");
        return humanMove(b);
      }
      (*b).lastPawn = -1;
      (*b).turn = !(*b).turn;
      if(destinationSpace != ' ') {
        (*b).drawCount = 0;
      } else {
        (*b).drawCount++;
      }
      return 1;
    } else if(currentPiece == 'b') {
      if(abs(destinationX-originX) != abs(destinationY-originY)) {
        printf("\nInvalid move.22\n");
        return humanMove(b);
      }
      char jx,jy;
      switch(2*(destinationY>originY) + (destinationX>originX)) {
        case 0:
        jx = originX-1;
        jy = originY-1;
        while(i(jx,jy)!=destination) {
          if((*b).pieces[i(jx,jy)]!=' ') {
            printf("\nInvalid move.23\n");
            return humanMove(b);
          }
          jx--;
          jy--;
        }
        break;
        case 1:
        jx = originX+1;
        jy = originY-1;
        while(i(jx,jy)!=destination) {
          if((*b).pieces[i(jx,jy)]!=' ') {
            printf("\nInvalid move.24\n");
            return humanMove(b);
          }
          jx++;
          jy--;
        }
        break;
        case 2:
        jx = originX-1;
        jy = originY+1;
        while(i(jx,jy)!=destination) {
          if((*b).pieces[i(jx,jy)]!=' ') {
            printf("\nInvalid move.25\n");
            return humanMove(b);
          }
          jx--;
          jy++;
        }
        break;
        case 3:
        jx = originX+1;
        jy = originY+1;
        while(i(jx,jy)!=destination) {
          if((*b).pieces[i(jx,jy)]!=' ') {
            printf("\nInvalid move.26\n");
            return humanMove(b);
          }
          jx++;
          jy++;
        }
        break;
      }
      (*b).pieces[destination] = 'b';
      (*b).pieces[origin] = ' ';
      if(checkCheck(b)) {
          (*b).pieces[destination] = destinationSpace;
          (*b).pieces[origin] = currentPiece;
          printf("\nInvalid move.27\n");
          return humanMove(b);
        }
        (*b).lastPawn = -1;
        (*b).turn = !(*b).turn;
        if(destinationSpace != ' ') {
          (*b).drawCount = 0;
        } else {
          (*b).drawCount++;
        }
        return 1;
    } else if(currentPiece == 'R') {
      char j;
      if(originX == destinationX) {
        if(destinationY > originY) {
          for(j = originY+1;j < destinationY;j++) {
            if((*b).pieces[i(originX,j)]!=' ') {
              printf("\nInvalid move.28\n");
              return humanMove(b);
            }
          }
        } else {
          for(j = originY-1;j > destinationY;j--) {
            if((*b).pieces[i(originX,j)]!=' ') {
              printf("\nInvalid move.29\n");
              return humanMove(b);
            }
          }
        }
      } else if(originY == destinationY) {
        if(destinationX > originX) {
          for(j = originX+1;j < destinationX;j++) {
            if((*b).pieces[i(j,originY)]!=' ') {
              printf("\nInvalid move.30\n");
              return humanMove(b);
            }
          }
        } else {
          for(j = originX-1;j > destinationX;j--) {
            if((*b).pieces[i(j,originY)]!=' ') {
              printf("\nInvalid move.31\n");
              return humanMove(b);
            }
          }
        }
      } else {
        printf("\nInvalid move.32\n");
        return humanMove(b);
      }
      (*b).pieces[destination] = 'R';
      (*b).pieces[origin] = ' ';
      if(checkCheck(b)) {
          (*b).pieces[destination] = destinationSpace;
          (*b).pieces[origin] = currentPiece;
          printf("\nInvalid move.33\n");
          return humanMove(b);
        }
        (*b).lastPawn = -1;
        (*b).turn = !(*b).turn;
        if(destinationSpace != ' ') {
          (*b).drawCount = 0;
        } else {
          (*b).drawCount++;
        }
        if(origin == i(0,0)) (*b).WL = 0;
        if(origin == i(7,0)) (*b).WR = 0;
        return 1;
    } else if(currentPiece == 'r') {
      char j;
      if(originX == destinationX) {
        if(destinationY > originY) {
          for(j = originY+1;j < destinationY;j++) {
            if((*b).pieces[i(originX,j)]!=' ') {
              printf("\nInvalid move.34\n");
              return humanMove(b);
            }
          }
        } else {
          for(j = originY-1;j > destinationY;j--) {
            if((*b).pieces[i(originX,j)]!=' ') {
              printf("\nInvalid move.35\n");
              return humanMove(b);
            }
          }
        }
      } else if(originY == destinationY) {
        if(destinationX > originX) {
          for(j = originX+1;j < destinationX;j++) {
            if((*b).pieces[i(j,originY)]!=' ') {
              printf("\nInvalid move.36\n");
              return humanMove(b);
            }
          }
        } else {
          for(j = originX-1;j > destinationX;j--) {
            if((*b).pieces[i(j,originY)]!=' ') {
              printf("\nInvalid move.37\n");
              return humanMove(b);
            }
          }
        }
      } else {
        printf("\nInvalid move.38\n");
        return humanMove(b);
      }
      (*b).pieces[destination] = 'r';
      (*b).pieces[origin] = ' ';
      if(checkCheck(b)) {
          (*b).pieces[destination] = destinationSpace;
          (*b).pieces[origin] = currentPiece;
          printf("\nInvalid move.39\n");
          return humanMove(b);
        }
        (*b).lastPawn = -1;
        (*b).turn = !(*b).turn;
        if(destinationSpace != ' ') {
          (*b).drawCount = 0;
        } else {
          (*b).drawCount++;
        }
        if(origin == i(0,7)) (*b).BL = 0;
        if(origin == i(7,7)) (*b).BR = 0;
        return 1;
    } else if(currentPiece == 'Q') {
      char j;
      if(originX == destinationX) {
        if(destinationY > originY) {
          for(j = originY+1;j < destinationY;j++) {
            if((*b).pieces[i(originX,j)]!=' ') {
              printf("\nInvalid move.40\n");
              return humanMove(b);
            }
          }
        } else {
          for(j = originY-1;j > destinationY;j--) {
            if((*b).pieces[i(originX,j)]!=' ') {
              printf("\nInvalid move.41\n");
              return humanMove(b);
            }
          }
        }
      } else if(originY == destinationY) {
        if(destinationX > originX) {
          for(j = originX+1;j < destinationX;j++) {
            if((*b).pieces[i(j,originY)]!=' ') {
              printf("\nInvalid move.42\n");
              return humanMove(b);
            }
          }
        } else {
          for(j = originX-1;j > destinationX;j--) {
            if((*b).pieces[i(j,originY)]!=' ') {
              printf("\nInvalid move.43\n");
              return humanMove(b);
            }
          }
        }
      } else if(abs(destinationX-originX) != abs(destinationY-originY)) {
        printf("\nInvalid move.44\n");
        return humanMove(b);
      } else {
      char jx,jy;
      switch(2*(destinationY>originY) + (destinationX>originX)) {
        case 0:
        jx = originX-1;
        jy = originY-1;
        while(i(jx,jy)!=destination) {
          if((*b).pieces[i(jx,jy)]!=' ') {
            printf("\nInvalid move.45\n");
            return humanMove(b);
          }
          jx--;
          jy--;
        }
        break;
        case 1:
        jx = originX+1;
        jy = originY-1;
        while(i(jx,jy)!=destination) {
          if((*b).pieces[i(jx,jy)]!=' ') {
            printf("\nInvalid move.46\n");
            return humanMove(b);
          }
          jx++;
          jy--;
        }
        break;
        case 2:
        jx = originX-1;
        jy = originY+1;
        while(i(jx,jy)!=destination) {
          if((*b).pieces[i(jx,jy)]!=' ') {
            printf("\nInvalid move.47\n");
            return humanMove(b);
          }
          jx--;
          jy++;
        }
        break;
        case 3:
        jx = originX+1;
        jy = originY+1;
        while(i(jx,jy)!=destination) {
          if((*b).pieces[i(jx,jy)]!=' ') {
            printf("\nInvalid move.48\n");
            return humanMove(b);
          }
          jx++;
          jy++;
          }
          break;
        }
      }
      (*b).pieces[destination] = 'Q';
      (*b).pieces[origin] = ' ';
      if(checkCheck(b)) {
        (*b).pieces[destination] = destinationSpace;
        (*b).pieces[origin] = currentPiece;
        printf("\nInvalid move.49\n");
        return humanMove(b);
      }
      (*b).lastPawn = -1;
      (*b).turn = !(*b).turn;
      if(destinationSpace != ' ') {
        (*b).drawCount = 0;
      } else {
        (*b).drawCount++;
      }
      return 1;
    } else if(currentPiece == 'q') {
      char j;
      if(originX == destinationX) {
        if(destinationY > originY) {
          for(j = originY+1;j < destinationY;j++) {
            if((*b).pieces[i(originX,j)]!=' ') {
              printf("\nInvalid move.50\n");
              return humanMove(b);
            }
          }
        } else {
          for(j = originY-1;j > destinationY;j--) {
            if((*b).pieces[i(originX,j)]!=' ') {
              printf("\nInvalid move.51\n");
              return humanMove(b);
            }
          }
        }
      } else if(originY == destinationY) {
        if(destinationX > originX) {
          for(j = originX+1;j < destinationX;j++) {
            if((*b).pieces[i(j,originY)]!=' ') {
              printf("\nInvalid move.52\n");
              return humanMove(b);
            }
          }
        } else {
          for(j = originX-1;j > destinationX;j--) {
            if((*b).pieces[i(j,originY)]!=' ') {
              printf("\nInvalid move.53\n");
              return humanMove(b);
            }
          }
        }
      } else if(abs(destinationX-originX) != abs(destinationY-originY)) {
        printf("\nInvalid move.54\n");
        return humanMove(b);
      } else {
      char jx,jy;
      switch(2*(destinationY>originY) + (destinationX>originX)) {
        case 0:
        jx = originX-1;
        jy = originY-1;
        while(i(jx,jy)!=destination) {
          if((*b).pieces[i(jx,jy)]!=' ') {
            printf("\nInvalid move.55\n");
            return humanMove(b);
          }
          jx--;
          jy--;
        }
        break;
        case 1:
        jx = originX+1;
        jy = originY-1;
        while(i(jx,jy)!=destination) {
          if((*b).pieces[i(jx,jy)]!=' ') {
            printf("\nInvalid move.56\n");
            return humanMove(b);
          }
          jx++;
          jy--;
        }
        break;
        case 2:
        jx = originX-1;
        jy = originY+1;
        while(i(jx,jy)!=destination) {
          if((*b).pieces[i(jx,jy)]!=' ') {
            printf("\nInvalid move.57\n");
            return humanMove(b);
          }
          jx--;
          jy++;
        }
        break;
        case 3:
        jx = originX+1;
        jy = originY+1;
        while(i(jx,jy)!=destination) {
          if((*b).pieces[i(jx,jy)]!=' ') {
            printf("\nInvalid move.58\n");
            return humanMove(b);
          }
          jx++;
          jy++;
        }
        break;
      }
    }
    (*b).pieces[destination] = 'q';
    (*b).pieces[origin] = ' ';
    if(checkCheck(b)) {
        (*b).pieces[destination] = destinationSpace;
        (*b).pieces[origin] = currentPiece;
        printf("\nInvalid move.59\n");
        return humanMove(b);
      }
      (*b).lastPawn = -1;
      (*b).turn = !(*b).turn;
      if(destinationSpace != ' ') {
        (*b).drawCount = 0;
      } else {
        (*b).drawCount++;
      }
      return 1;
    } else if(currentPiece=='K') {
      //printf("\nCheckpoint 1.\n");
      char absX = abs(destinationX-originX);
      char absY = abs(destinationY-originY);
      if(absX==2 && absY==0 && origin==i(4,0) && !checkCheck(b)) {
        if((*b).WL && destination==i(2,0) && (*b).pieces[i(3,0)]==' ' && (*b).pieces[i(2,0)]==' ' && (*b).pieces[i(1,0)]==' ') {
          (*b).pieces[i(3,0)] = 'K';
          (*b).kingW = i(3,0);
          (*b).pieces[i(4,0)] = ' ';
          if(checkCheck(b)) {
            (*b).pieces[i(3,0)] = ' ';
            (*b).pieces[i(4,0)] = 'K';
            (*b).kingW = i(4,0);
            printf("\nInvalid move60.\n");
            return humanMove(b);
          }
          (*b).pieces[i(2,0)] = 'K';
          (*b).kingW = i(2,0);
          (*b).pieces[i(3,0)] = 'R';
          (*b).pieces[i(0,0)] = ' ';
          if(checkCheck(b)) {
            (*b).pieces[i(0,0)] = 'R';
            (*b).pieces[i(2,0)] = ' ';
            (*b).pieces[i(3,0)] = ' ';
            (*b).pieces[i(4,0)] = 'K';
            (*b).kingW = i(4,0);
            printf("\nInvalid move.61\n");
            return humanMove(b);
          }
          (*b).lastPawn = -1;
          (*b).turn = !(*b).turn;
          (*b).drawCount++;
          (*b).WL = (*b).WR = 0;
          return 1;
        } else if((*b).WR && destination==i(6,0) && (*b).pieces[i(5,0)]==' ' && (*b).pieces[i(6,0)]==' ') {
          (*b).pieces[i(5,0)] = 'K';
          (*b).kingW = i(5,0);
          (*b).pieces[i(4,0)] = ' ';
          if(checkCheck(b)) {
            (*b).pieces[i(5,0)] = ' ';
            (*b).pieces[i(4,0)] = 'K';
            (*b).kingW = i(4,0);
            printf("\nInvalid move.62\n");
            return humanMove(b);
          }
          (*b).pieces[i(6,0)] = 'K';
          (*b).kingW = i(6,0);
          (*b).pieces[i(5,0)] = 'R';
          (*b).pieces[i(7,0)] = ' ';
          if(checkCheck(b)) {
            (*b).pieces[i(7,0)] = 'R';
            (*b).pieces[i(6,0)] = ' ';
            (*b).pieces[i(5,0)] = ' ';
            (*b).pieces[i(4,0)] = 'K';
            (*b).kingW = i(4,0);
            printf("\nInvalid move.63\n");
            return humanMove(b);
          }
          (*b).lastPawn = -1;
          (*b).turn = !(*b).turn;
          (*b).drawCount++;
          (*b).WL = (*b).WR = 0;
          return 1;
        } else {
          printf("\nInvalid move.64\n");
          return humanMove(b);
        }
      } else if(absX<=1 && absY<=1) {
        (*b).pieces[destination] = 'K';
        (*b).kingW = destination;
        (*b).pieces[origin] = ' ';
        if(checkCheck(b)) {
          (*b).pieces[destination] = destinationSpace;
          (*b).pieces[origin] = currentPiece;
          (*b).kingW = origin;
          printf("\nInvalid move.65\n");
          return humanMove(b);
        }
        (*b).lastPawn = -1;
        (*b).turn = !(*b).turn;
        if(destinationSpace != ' ') {
          (*b).drawCount = 0;
        } else {
          (*b).drawCount++;
        }
        (*b).WL = (*b).WR = 0;
        return 1;
      } else {
        printf("\nInvalid move.66\n");
        return humanMove(b);
      }
    } else if(currentPiece=='k') {
      char absX = abs(destinationX-originX);
      char absY = abs(destinationY-originY);
      if(absX==2 && absY==0 && origin==i(4,7) && !checkCheck(b)) {
        if((*b).BL && destination==i(2,7) && (*b).pieces[i(3,7)]==' ' && (*b).pieces[i(2,7)]==' ' && (*b).pieces[i(1,7)]==' ') {
          (*b).pieces[i(3,7)] = 'k';
          (*b).kingB = i(3,7);
          (*b).pieces[i(4,7)] = ' ';
          if(checkCheck(b)) {
            (*b).pieces[i(3,7)] = ' ';
            (*b).pieces[i(4,7)] = 'k';
            (*b).kingB = i(4,7);
            printf("\nInvalid move.67\n");
            return humanMove(b);
          }
          (*b).pieces[i(2,7)] = 'k';
          (*b).kingB = i(2,7);
          (*b).pieces[i(3,7)] = 'r';
          (*b).pieces[i(0,7)] = ' ';
          if(checkCheck(b)) {
            (*b).pieces[i(0,7)] = 'r';
            (*b).pieces[i(2,7)] = ' ';
            (*b).pieces[i(3,7)] = ' ';
            (*b).pieces[i(4,7)] = 'k';
            (*b).kingB = i(4,7);
            printf("\nInvalid move68.\n");
            return humanMove(b);
          }
          (*b).lastPawn = -1;
          (*b).turn = !(*b).turn;
          (*b).drawCount++;
          (*b).BL = (*b).BR = 0;
          return 1;
        } else if((*b).BR && destination==i(6,7) && (*b).pieces[i(5,7)]==' ' && (*b).pieces[i(6,7)]==' ') {
          (*b).pieces[i(5,7)] = 'k';
          (*b).kingB = i(5,7);
          (*b).pieces[i(4,7)] = ' ';
          if(checkCheck(b)) {
            (*b).pieces[i(5,7)] = ' ';
            (*b).pieces[i(4,7)] = 'k';
            (*b).kingB = i(4,7);
            printf("\nInvalid move.69\n");
            return humanMove(b);
          }
          (*b).pieces[i(6,7)] = 'k';
          (*b).kingB = i(6,7);
          (*b).pieces[i(5,7)] = 'r';
          (*b).pieces[i(7,7)] = ' ';
          if(checkCheck(b)) {
            (*b).pieces[i(7,7)] = 'r';
            (*b).pieces[i(6,7)] = ' ';
            (*b).pieces[i(5,7)] = ' ';
            (*b).pieces[i(4,7)] = 'k';
            (*b).kingB = i(4,7);
            printf("\nInvalid move.70\n");
            return humanMove(b);
          }
          (*b).lastPawn = -1;
          (*b).turn = !(*b).turn;
          (*b).drawCount++;
          (*b).BL = (*b).BR = 0;
          return 1;
        } else {
          printf("\nInvalid move.71\n");
          return humanMove(b);
        }
      } else if(absX<=1 && absY<=1) {
        (*b).pieces[destination] = 'k';
        (*b).kingB = destination;
        (*b).pieces[origin] = ' ';
        if(checkCheck(b)) {
          (*b).pieces[destination] = destinationSpace;
          (*b).pieces[origin] = currentPiece;
          (*b).kingB = origin;
          printf("\nInvalid move.72\n");
          return humanMove(b);
        }
        (*b).lastPawn = -1;
        (*b).turn = !(*b).turn;
        if(destinationSpace != ' ') {
          (*b).drawCount = 0;
        } else {
          (*b).drawCount++;
        }
        (*b).BL = (*b).BR = 0;
        return 1;
      } else {
        printf("\nInvalid move.73\n");
        return humanMove(b);
      }
    } else {
      printf("\nInvalid move.74\n");
      return humanMove(b);
    }
  }
}

//Main

int main() {
  struct board *b = newBoard();
  printf("\nEntering \"E\" will exit the game.\nOtherwise, enter a move as a coordinate pair (e.g. \"e2e4\").\nCastling is treated as a king move (e.g. short castleing for white will always be \"e1g1\"\nSpecific promotion will be prompted after a qualifying pawn move.\n\n");
  printBoard(b);
  while(humanMove(b)) {
    printBoard(b);
  }
  printf("\nClean Exit\n");
  return 0;
}
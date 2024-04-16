#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

//Board in Memory

#define WHITE 1
#define BLACK 0

#define INDEX char
#define PIECE char
#define XINDEX char
#define YINDEX char
#define BOOL char

#define DEBUG 0

#if DEBUG
  #define PRINT(s, ...) printf(s,##__VA_ARGS__)
#else
  #define PRINT(s, ...) (void)0
#endif

#define DEBUG2 0

#if DEBUG2
  #define PRINT2(s, ...) printf(s,##__VA_ARGS__)
#else
  #define PRINT2(s, ...) (void)0
#endif

#define DEBUGR 0

#if DEBUGR
  #define PRINTR(s, ...) printf(s,##__VA_ARGS__)
#else
  #define PRINTR(s, ...) (void)0
#endif

#define DEBUGK 1

#if DEBUGK
  #define PRINTK(s, ...) printf(s,##__VA_ARGS__)
#else
  #define PRINTK(s, ...) (void)0
#endif

struct board {
  char *pieces;
  char turn;
  char lastPawn;
  char kingW;
  char kingB;
  char WL,WR,BL,BR;//Castle Booleans
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
  PRINT2("\nIndex in Bounds\n");
  return x + 8*(7 - y);
}

struct board *newBoard() {
  struct board *b = (struct board *)malloc(sizeof(struct board));
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

void printBoardDebug(struct board *b,FILE *fptr) {
  fprintf(fptr,"\n  ---------------------------------\n");
  char j, p;
  for(j=0; j<64; j++) {
    if(j%8==0) {
      fprintf(fptr,"%d |",8-j/8);
    }
    fprintf(fptr," %c |",(*b).pieces[j]);
    if (j%8==7) {
      fprintf(fptr,"\n  ---------------------------------\n");
    }
  }
  fprintf(fptr,"    a   b   c   d   e   f   g   h\n");
}

//Game Logic

char color(char p) {
  return p=='P' || p=='R' || p=='N' || p=='B' || p=='Q' || p=='K';
}

char isRank(char c) {
  return c=='1' || c=='2' || c=='3' || c=='4' || c=='5' || c=='6' || c=='7' || c=='8';
}

char isFile(char c) {
  return c=='a' || c=='b' || c=='c' || c=='d' || c=='e' || c=='f' || c=='g' || c=='h';
}


//Returns 0 if the i return fed in was -1, else the piece at the index on the pointed board.
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
  printf("\nPlease input your desired promotion (N,B,R,Q)\n");
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
    if((*b).pieces[origin]==' ') {
      printf("\nInvalid move.1.1\n");
      return humanMove(b);
    }
    if(color(currentPiece)!=(*b).turn) {
      printf("\nInvalid move.1.2\n");
      return humanMove(b);
    }
    if(origin == destination) {
      printf("\nInvalid move.1.3\n");
      return humanMove(b);
    }
    if(destinationSpace!=' ' && color(currentPiece)==color(destinationSpace)) {
      printf("\nInvalid move.1.4\n");
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

//Artificial Intelligence

float knights[16] = {250,300,350,400,
                     200,250,300,350,
                     150,200,250,300,
                     100,150,200,250};

float bishops[16] = {270,320,370,420,
                     220,270,320,370,
                     170,220,270,320,
                     120,170,220,270};

float rooks[16] = {550,600,650,700,
                   500,550,600,650,
                   450,500,550,600,
                   400,450,500,550};

float queens[16] = {850, 900, 950, 1000,
                    800, 850, 900, 950,
                    750, 800, 850, 900,
                    700, 750, 800, 850};

float kings[16] = {10, 10, 10, 10,
                   30, 30, 30, 30,
                   110,110,80, 30,
                   110,110,100,80};

float kingE[16] ={60, 80, 100,120,
                  40, 60, 80, 100,
                  20, 40, 60, 80,
                  0,  20, 40, 60};

float pawns[6] = {100,125,150,200,250,400};

float value[5] = {1,3,3,5,9};

INDEX table(INDEX j) {
  XINDEX xCurrent;
  YINDEX yCurrent;
  xCurrent = x(j);
  yCurrent = y(j);
  if(yCurrent <= 3) yCurrent = 3-yCurrent;
  else yCurrent = yCurrent-4;
  if(xCurrent > 3) xCurrent = 7-xCurrent;
  return 4*yCurrent+xCurrent;
}

void ReLU(float *x) {
  if((*x)<0) {
    *x = 0;
  }
}

struct score {
  float MW, TW, MB, TB, Total;//Material and Tactical scores for White and Black
};

struct score newScore(float nMW, float nTW, float nMB,float nTB) {
  struct score s;
  s.MW = nMW;
  s.TW = nTW;
  s.MB = nMB;
  s.TB = nTB;
  return s;
}

struct node {
  struct score eval;
  int moveCount, movesPlayed;
  struct board *current;
  struct node *previous, *moves, *next;
};

struct node *newNode() {
  struct node *new = (struct node*)malloc(sizeof(struct node));
  (*new).eval = newScore(0,0,0,0);
  (*new).moveCount = -1;
  (*new).movesPlayed = 0;
  (*new).current = newBoard();
  (*new).previous = (*new).moves = (*new).next = NULL;
  return new;
}

struct node *nextNodeBase(struct node *n) {
  struct node *new = (struct node *)malloc(sizeof(struct node));
  (*new).eval = n->eval;
  (*new).moveCount = -1;
  (*new).movesPlayed = (*n).movesPlayed + 1;
  (*new).current = (struct board*)memcpy(malloc(sizeof(struct board)),(*n).current,sizeof(struct board));
  new->current->pieces = (char*)memcpy(malloc(64*sizeof(char)),n->current->pieces,64*sizeof(char));
  (*(*new).current).turn = !(*(*new).current).turn;
  return new;
}

void analysis(struct node *n) {
  INDEX j;
  for(j=0; j<64; j++) {
    switch((*(*n).current).pieces[j]) {
      case 'p':
        (*n).eval.MB += pawns[6-y(j)];
        break;
      case 'P':
        (*n).eval.MW += pawns[y(j)-1];
        break;
      case 'n':
        (*n).eval.MB += knights[table(j)];
        break;
      case 'N':
        (*n).eval.MW += knights[table(j)];
        break;
      case 'b':
        (*n).eval.MB += bishops[table(j)];
        break;
      case 'B':
        (*n).eval.MW += bishops[table(j)];
        break;
      case 'r':
        (*n).eval.MB += rooks[table(j)];
        break;
      case 'R':
        (*n).eval.MW += rooks[table(j)];
        break;
      case 'q':
        (*n).eval.MB += queens[table(j)];
        break;
      case 'Q':
        (*n).eval.MW += queens[table(j)];
        break;
      case 'k':
        (*n).eval.MB += kings[table(j)];
        break;
      case 'K':
        (*n).eval.MW += kings[table(j)];
        break;
      case ' ':
        break;
      default:
        printf("analysis Error");
        exit(1);
    }

  }
}

BOOL isWhite(PIECE piece) {
  return piece == 'P' || piece == 'N' || piece == 'B' || piece == 'R' || piece == 'Q' || piece == 'K';
}

BOOL isBlack(PIECE piece) {
  return piece == 'p' || piece == 'n' || piece == 'b' || piece == 'r' || piece == 'q' || piece == 'k';
}

int comparator(const void* a,const void* b) {
  struct node *a2, *b2;
  a2 = (struct node *)a;
  b2 = (struct node *)b;
  float aAnalysis, bAnalysis;
  aAnalysis = (*a2).eval.MW + (*a2).eval.TW - (*a2).eval.MB - (*a2).eval.TB;
  bAnalysis = (*b2).eval.MW + (*b2).eval.TW - (*b2).eval.MB - (*b2).eval.TB;
  if(aAnalysis > bAnalysis) {
    return 1;
  } else if(aAnalysis == bAnalysis) {
    return 0;
  } else {
    return -1;
  }
}

void deepCopyNode(struct node *dest, struct node *copy) {
  memcpy((void *)dest,(void *)copy,sizeof(struct node));
  dest->current = (struct board*)memcpy(malloc(sizeof(struct board)),(void *)copy->current,sizeof(struct board));
  dest->current->pieces = (char*)memcpy(malloc(64*sizeof(char)),(void *)copy->current->pieces,64*sizeof(char));
}

XINDEX knightMovesX[8] = {2,1,-1,-2,-2,-1,1,2};
YINDEX knightMovesY[8] = {1,2,2,1,-1,-2,-2,-1};

XINDEX kingMovesX[8] = {1,1,0,-1,-1,-1,0,1};
YINDEX kingMovesY[8] = {0,1,1,1,0,-1,-1,-1};

//Lists game states after legal moves at the moves pointer within n.
void generateMoves(struct node *n) {
  INDEX j;
  (*n).moveCount = 0;
  (*n).moves = (struct node *)malloc(100*sizeof(struct node));
  struct node *base = nextNodeBase(n);
  int moveIndex = 0;
  int currentAlloc = 100;
  struct node *end = n->moves + moveIndex + currentAlloc;

  XINDEX X;
  YINDEX Y;
  XINDEX X2;
  YINDEX Y2;
  INDEX j2;
  PIECE dest;
  PRINT("\n1\n");
  for(j=0; j<64; j++) {
    X = x(j);
    Y = y(j);
    PRINT("\n2\n");
    switch((*(*n).current).pieces[j]) {
      case 'p':
        PRINT("\n3\n");
        if((*(*n).current).pieces[i(X,Y-1)] == ' ') {
          PRINT("\na\n");
          deepCopyNode(n->moves + moveIndex,base);
          PRINT("\nb %c %c\n",(*(*((*n).moves + moveIndex)).current).pieces[j],(*(*((*n).moves + moveIndex)).current).pieces[i(X,Y-1)]);
          n->moves[moveIndex].current->pieces[j] = ' ';
          PRINT("test2");
          n->moves[moveIndex].current->pieces[i(X,Y-1)] = 'p';
          PRINT("\nc\n");
          n->moves[moveIndex].current->lastPawn = -1;
          n->moves[moveIndex].current->drawCount = 0;
          moveIndex++;
          n->moveCount++;
          PRINT("\n4\n");
          if(moveIndex == currentAlloc) {
            currentAlloc += 100;
            (*n).moves = (struct node *)realloc((void*)(*n).moves,currentAlloc*sizeof(struct node));
            end += 100;
          }
          PRINT("\n5\n");
          if(getPiece((*n).current,i(X,Y-2)) == ' ' && Y==6) {
            deepCopyNode(n->moves + moveIndex,base);
            PRINT(" 5.1 ");
            n->moves[moveIndex].current->pieces[j] = ' ';
            n->moves[moveIndex].current->pieces[i(X,Y-2)] = 'p';
            n->moves[moveIndex].current->lastPawn = X;
            n->moves[moveIndex].current->drawCount = 0;
            PRINT(" 5.2 ");
            moveIndex++;
            n->moveCount++;
            if(moveIndex == currentAlloc) {
              currentAlloc += 100;
              (*n).moves = (struct node *)realloc((void*)(*n).moves,currentAlloc*sizeof(struct node));
              end += 100;
            }
          }
          PRINT("\n6\n");
        }
        if(isWhite(getPiece((*n).current,i(X+1,Y-1)))) {
          deepCopyNode(n->moves + moveIndex,base);
          n->moves[moveIndex].current->pieces[j] = ' ';
          n->moves[moveIndex].current->pieces[i(X+1,Y-1)] = 'p';
          n->moves[moveIndex].current->lastPawn = -1;
          n->moves[moveIndex].current->drawCount = 0;
          moveIndex++;
          n->moveCount++;
          if(moveIndex == currentAlloc) {
            currentAlloc += 100;
            (*n).moves = (struct node *)realloc((void*)(*n).moves,currentAlloc*sizeof(struct node));
            end += 100;
          }
        }
        if(isWhite(getPiece((*n).current,i(X-1,Y-1)))) {
          deepCopyNode(n->moves + moveIndex,base);
          n->moves[moveIndex].current->pieces[j] = ' ';
          n->moves[moveIndex].current->pieces[i(X-1,Y-1)] = 'p';
          n->moves[moveIndex].current->lastPawn = -1;
          n->moves[moveIndex].current->drawCount = 0;
          moveIndex++;
          (*n).moveCount++;
          if(moveIndex == currentAlloc) {
            currentAlloc += 100;
            (*n).moves = (struct node *)realloc((void*)(*n).moves,currentAlloc*sizeof(struct node));
            end += 100;
          }
        }
        if(n->current->lastPawn-X == 1 && Y==3) {
          deepCopyNode(n->moves + moveIndex,base);
          n->moves[moveIndex].current->pieces[j] = ' ';
          n->moves[moveIndex].current->pieces[i(n->current->lastPawn,2)] = 'p';
          n->moves[moveIndex].current->pieces[i(n->current->lastPawn,3)] = ' ';
          n->moves[moveIndex].current->lastPawn = -1;
          n->moves[moveIndex].current->drawCount = 0;
          moveIndex++;
          (*n).moveCount++;
          if(moveIndex == currentAlloc) {
            currentAlloc += 100;
            (*n).moves = (struct node *)realloc((void*)(*n).moves,currentAlloc*sizeof(struct node));
            end += 100;
          }
        }
        if(X-n->current->lastPawn == 1 && Y==3) {
          deepCopyNode(n->moves + moveIndex,base);
          n->moves[moveIndex].current->pieces[j] = ' ';
          n->moves[moveIndex].current->pieces[i(n->current->lastPawn,2)] = 'p';
          n->moves[moveIndex].current->pieces[i(n->current->lastPawn,3)] = ' ';
          n->moves[moveIndex].current->lastPawn = -1;
          n->moves[moveIndex].current->drawCount = 0;
          moveIndex++;
          (*n).moveCount++;
          if(moveIndex == currentAlloc) {
            currentAlloc += 100;
            (*n).moves = (struct node *)realloc((void*)(*n).moves,currentAlloc*sizeof(struct node));
            end += 100;
          }
        }
        break;
      case 'n':
      PRINT2("\nn\n");
        for(j2=0; j2<8; j2++) {
          PRINT2("\n1 j2 = %d\n",j2);
          dest = getPiece(n->current,i(X+knightMovesX[j2],Y+knightMovesY[j2]));
          if(dest == ' ' || isWhite(dest)) {
            PRINT2("\n2\n");
            deepCopyNode(n->moves + moveIndex,base);
            n->moves[moveIndex].current->pieces[j] = ' ';
            n->moves[moveIndex].current->pieces[i(X+knightMovesX[j2],Y+knightMovesY[j2])] = 'n';
            n->moves[moveIndex].current->lastPawn = -1;
            PRINT2("\n3\n");
            if(dest == ' ') {
              PRINT2("\n4a\n");
              n->moves[moveIndex].current->drawCount++;
            } else {
              PRINT2("\n4b\n");
              n->moves[moveIndex].current->drawCount = 0;
            }
            moveIndex++;
            (*n).moveCount++;
            PRINT2("\n5\n");
            if(moveIndex == currentAlloc) {
              currentAlloc += 100;
              (*n).moves = (struct node *)realloc((void*)(*n).moves,currentAlloc*sizeof(struct node));
              end += 100;
            }
            PRINT2("\n6\n");
          }
        }
        break;
      case 'b':
      PRINT2("\nb\n");
        X2 = X+1;
        Y2 = Y+1;
        while(i(X2,Y2)!=-1) {
          dest = getPiece(n->current,i(X2,Y2));
          if(isBlack(dest)) break;
          deepCopyNode(n->moves + moveIndex,base);
          n->moves[moveIndex].current->pieces[i(X,Y)] = ' ';
          n->moves[moveIndex].current->pieces[i(X2,Y2)] = 'b';
          n->moves[moveIndex].current->lastPawn = -1;
          if(dest == ' ')
            n->moves[moveIndex].current->drawCount++;
          else {
            n->moves[moveIndex].current->drawCount = 0;
          }
          moveIndex++;
          (*n).moveCount++;
          if(moveIndex == currentAlloc) {
              currentAlloc += 100;
              (*n).moves = (struct node *)realloc((void*)(*n).moves,currentAlloc*sizeof(struct node));
              end += 100;
          }
          if(dest != ' ') break;
          X2++;
          Y2++;
        }
        X2 = X-1;
        Y2 = Y+1;
        while(i(X2,Y2)!=-1) {
          dest = getPiece(n->current,i(X2,Y2));
          if(isBlack(dest)) break;
          deepCopyNode(n->moves + moveIndex,base);
          n->moves[moveIndex].current->pieces[i(X,Y)] = ' ';
          n->moves[moveIndex].current->pieces[i(X2,Y2)] = 'b';
          n->moves[moveIndex].current->lastPawn = -1;
          if(dest == ' ')
            n->moves[moveIndex].current->drawCount++;
          else {
            n->moves[moveIndex].current->drawCount = 0;
          }
          moveIndex++;
          (*n).moveCount++;
          if(moveIndex == currentAlloc) {
              currentAlloc += 100;
              (*n).moves = (struct node *)realloc((void*)(*n).moves,currentAlloc*sizeof(struct node));
              end += 100;
          }
          if(dest != ' ') break;
            X2--;
            Y2++;
        }
        X2 = X-1;
        Y2 = Y-1;
        while(i(X2,Y2)!=-1) {
          dest = getPiece(n->current,i(X2,Y2));
          if(isBlack(dest)) break;
          deepCopyNode(n->moves + moveIndex,base);
          n->moves[moveIndex].current->pieces[i(X,Y)] = ' ';
          n->moves[moveIndex].current->pieces[i(X2,Y2)] = 'b';
          n->moves[moveIndex].current->lastPawn = -1;
          if(dest == ' ')
            n->moves[moveIndex].current->drawCount++;
          else {
            n->moves[moveIndex].current->drawCount = 0;
          }
          moveIndex++;
          (*n).moveCount++;
          if(moveIndex == currentAlloc) {
              currentAlloc += 100;
              (*n).moves = (struct node *)realloc((void*)(*n).moves,currentAlloc*sizeof(struct node));
              end += 100;
          }
          if(dest != ' ') break;
            X2--;
            Y2--;
        }
        X2 = X+1;
        Y2 = Y-1;
        while(i(X2,Y2)!=-1) {
          dest = getPiece(n->current,i(X2,Y2));
          if(isBlack(dest)) break;
          deepCopyNode(n->moves + moveIndex,base);
          n->moves[moveIndex].current->pieces[i(X,Y)] = ' ';
          n->moves[moveIndex].current->pieces[i(X2,Y2)] = 'b';
          n->moves[moveIndex].current->lastPawn = -1;
          if(dest == ' ')
            n->moves[moveIndex].current->drawCount++;
          else {
            n->moves[moveIndex].current->drawCount = 0;
          }
          moveIndex++;
          (*n).moveCount++;
          if(moveIndex == currentAlloc) {
              currentAlloc += 100;
              (*n).moves = (struct node *)realloc((void*)(*n).moves,currentAlloc*sizeof(struct node));
              end += 100;
          }
          if(dest != ' ') break;
            X2++;
            Y2--;
        }
        break;
      case 'r':
        PRINTR("\nPreL\n");
        for(j2 = X+1;i(j2,Y)!=-1;j2++) {
          dest = getPiece(n->current,i(j2,Y));
          if(isBlack(dest)) break;
          deepCopyNode(n->moves + moveIndex,base);
          n->moves[moveIndex].current->pieces[i(X,Y)] = ' ';
          n->moves[moveIndex].current->pieces[i(j2,Y)] = 'r';
          n->moves[moveIndex].current->lastPawn = -1;
          if(dest == ' ')
            n->moves[moveIndex].current->drawCount++;
          else {
            n->moves[moveIndex].current->drawCount = 0;
          }
          moveIndex++;
          (*n).moveCount++;
          if(n->moves[moveIndex].current->BL && X==0) n->moves[moveIndex].current->BL = 0;
          if(n->moves[moveIndex].current->BR && X==7) n->moves[moveIndex].current->BR = 0;
          if(moveIndex == currentAlloc) {
              currentAlloc += 100;
              (*n).moves = (struct node *)realloc((void*)(*n).moves,currentAlloc*sizeof(struct node));
              end += 100;
          }
          if(dest != ' ') break;
        }
        PRINTR("\n1L\n");
        for(j2 = Y+1;i(X,j2)!=-1;j2++) {
          dest = getPiece(n->current,i(X,j2));
          if(isBlack(dest)) break;
          deepCopyNode(n->moves + moveIndex,base);
          n->moves[moveIndex].current->pieces[i(X,Y)] = ' ';
          n->moves[moveIndex].current->pieces[i(X,j2)] = 'r';
          n->moves[moveIndex].current->lastPawn = -1;
          if(dest == ' ')
            n->moves[moveIndex].current->drawCount++;
          else {
            n->moves[moveIndex].current->drawCount = 0;
          }
          moveIndex++;
          (*n).moveCount++;
          if(n->moves[moveIndex].current->BL && X==0) n->moves[moveIndex].current->BL = 0;
          if(n->moves[moveIndex].current->BR && X==7) n->moves[moveIndex].current->BR = 0;
          if(moveIndex == currentAlloc) {
              currentAlloc += 100;
              (*n).moves = (struct node *)realloc((void*)(*n).moves,currentAlloc*sizeof(struct node));
              end += 100;
          }
          if(dest != ' ') break;
        }
        PRINTR("\n2L\n");
        for(j2 = X-1;i(j2,Y)!=-1;j2--) {
          dest = getPiece(n->current,i(j2,Y));
          if(isBlack(dest)) break;
          PRINTR("\n0\n");
          deepCopyNode(n->moves + moveIndex,base);
          n->moves[moveIndex].current->pieces[i(X,Y)] = ' ';
          n->moves[moveIndex].current->pieces[i(j2,Y)] = 'r';
          n->moves[moveIndex].current->lastPawn = -1;
          PRINTR("\n1\n");
          if(dest == ' ')
            n->moves[moveIndex].current->drawCount++;
          else {
            n->moves[moveIndex].current->drawCount = 0;
          }
          moveIndex++;
          (*n).moveCount++;
          if(n->moves[moveIndex].current->BL && X==0) n->moves[moveIndex].current->BL = 0;
          if(n->moves[moveIndex].current->BR && X==7) n->moves[moveIndex].current->BR = 0;
          if(moveIndex == currentAlloc) {
              currentAlloc += 100;
              (*n).moves = (struct node *)realloc((void*)(*n).moves,currentAlloc*sizeof(struct node));
              end += 100;
          }
          if(dest != ' ') break;
        }
        PRINTR("\n3L\n");
        for(j2 = Y-1;i(X,j2)!=-1;j2--) {
          dest = getPiece(n->current,i(X,j2));
          if(isBlack(dest)) break;
          deepCopyNode(n->moves + moveIndex,base);
          n->moves[moveIndex].current->pieces[i(X,Y)] = ' ';
          n->moves[moveIndex].current->pieces[i(X,j2)] = 'r';
          n->moves[moveIndex].current->lastPawn = -1;
          if(dest == ' ')
            n->moves[moveIndex].current->drawCount++;
          else {
            n->moves[moveIndex].current->drawCount = 0;
          }
          moveIndex++;
          (*n).moveCount++;
          if(n->moves[moveIndex].current->BL && X==0) n->moves[moveIndex].current->BL = 0;
          if(n->moves[moveIndex].current->BR && X==7) n->moves[moveIndex].current->BR = 0;
          if(moveIndex == currentAlloc) {
              currentAlloc += 100;
              (*n).moves = (struct node *)realloc((void*)(*n).moves,currentAlloc*sizeof(struct node));
              end += 100;
          }
          if(dest != ' ') break;
        }
        PRINTR("\n4L\n");
        break;
      case 'q':
        PRINT2("\nb\n");
        X2 = X+1;
        Y2 = Y+1;
        while(i(X2,Y2)!=-1) {
          dest = getPiece(n->current,i(X2,Y2));
          if(isBlack(dest)) break;
          deepCopyNode(n->moves + moveIndex,base);
          n->moves[moveIndex].current->pieces[i(X,Y)] = ' ';
          n->moves[moveIndex].current->pieces[i(X2,Y2)] = 'q';
          n->moves[moveIndex].current->lastPawn = -1;
          if(dest == ' ')
            n->moves[moveIndex].current->drawCount++;
          else {
            n->moves[moveIndex].current->drawCount = 0;
          }
          moveIndex++;
          (*n).moveCount++;
          if(moveIndex == currentAlloc) {
              currentAlloc += 100;
              (*n).moves = (struct node *)realloc((void*)(*n).moves,currentAlloc*sizeof(struct node));
              end += 100;
          }
          if(dest != ' ') break;
          X2++;
          Y2++;
        }
        X2 = X-1;
        Y2 = Y+1;
        while(i(X2,Y2)!=-1) {
          dest = getPiece(n->current,i(X2,Y2));
          if(isBlack(dest)) break;
          deepCopyNode(n->moves + moveIndex,base);
          n->moves[moveIndex].current->pieces[i(X,Y)] = ' ';
          n->moves[moveIndex].current->pieces[i(X2,Y2)] = 'q';
          n->moves[moveIndex].current->lastPawn = -1;
          if(dest == ' ')
            n->moves[moveIndex].current->drawCount++;
          else {
            n->moves[moveIndex].current->drawCount = 0;
          }
          moveIndex++;
          (*n).moveCount++;
          if(moveIndex == currentAlloc) {
              currentAlloc += 100;
              (*n).moves = (struct node *)realloc((void*)(*n).moves,currentAlloc*sizeof(struct node));
              end += 100;
          }
          if(dest != ' ') break;
            X2--;
            Y2++;
        }
        X2 = X-1;
        Y2 = Y-1;
        while(i(X2,Y2)!=-1) {
          dest = getPiece(n->current,i(X2,Y2));
          if(isBlack(dest)) break;
          deepCopyNode(n->moves + moveIndex,base);
          n->moves[moveIndex].current->pieces[i(X,Y)] = ' ';
          n->moves[moveIndex].current->pieces[i(X2,Y2)] = 'q';
          n->moves[moveIndex].current->lastPawn = -1;
          if(dest == ' ')
            n->moves[moveIndex].current->drawCount++;
          else {
            n->moves[moveIndex].current->drawCount = 0;
          }
          moveIndex++;
          (*n).moveCount++;
          if(moveIndex == currentAlloc) {
              currentAlloc += 100;
              (*n).moves = (struct node *)realloc((void*)(*n).moves,currentAlloc*sizeof(struct node));
              end += 100;
          }
          if(dest != ' ') break;
            X2--;
            Y2--;
        }
        X2 = X+1;
        Y2 = Y-1;
        while(i(X2,Y2)!=-1) {
          dest = getPiece(n->current,i(X2,Y2));
          if(isBlack(dest)) break;
          deepCopyNode(n->moves + moveIndex,base);
          n->moves[moveIndex].current->pieces[i(X,Y)] = ' ';
          n->moves[moveIndex].current->pieces[i(X2,Y2)] = 'q';
          n->moves[moveIndex].current->lastPawn = -1;
          if(dest == ' ')
            n->moves[moveIndex].current->drawCount++;
          else {
            n->moves[moveIndex].current->drawCount = 0;
          }
          moveIndex++;
          (*n).moveCount++;
          if(moveIndex == currentAlloc) {
              currentAlloc += 100;
              (*n).moves = (struct node *)realloc((void*)(*n).moves,currentAlloc*sizeof(struct node));
              end += 100;
          }
          if(dest != ' ') break;
            X2++;
            Y2--;
        }
        PRINTR("\nPreL\n");
        for(j2 = X+1;i(j2,Y)!=-1;j2++) {
          dest = getPiece(n->current,i(j2,Y));
          if(isBlack(dest)) break;
          deepCopyNode(n->moves + moveIndex,base);
          n->moves[moveIndex].current->pieces[i(X,Y)] = ' ';
          n->moves[moveIndex].current->pieces[i(j2,Y)] = 'q';
          n->moves[moveIndex].current->lastPawn = -1;
          if(dest == ' ')
            n->moves[moveIndex].current->drawCount++;
          else {
            n->moves[moveIndex].current->drawCount = 0;
          }
          moveIndex++;
          (*n).moveCount++;
          if(moveIndex == currentAlloc) {
              currentAlloc += 100;
              (*n).moves = (struct node *)realloc((void*)(*n).moves,currentAlloc*sizeof(struct node));
              end += 100;
          }
          if(dest != ' ') break;
        }
        PRINTR("\n1L\n");
        for(j2 = Y+1;i(X,j2)!=-1;j2++) {
          dest = getPiece(n->current,i(X,j2));
          if(isBlack(dest)) break;
          deepCopyNode(n->moves + moveIndex,base);
          n->moves[moveIndex].current->pieces[i(X,Y)] = ' ';
          n->moves[moveIndex].current->pieces[i(X,j2)] = 'q';
          n->moves[moveIndex].current->lastPawn = -1;
          if(dest == ' ')
            n->moves[moveIndex].current->drawCount++;
          else {
            n->moves[moveIndex].current->drawCount = 0;
          }
          moveIndex++;
          (*n).moveCount++;
          if(moveIndex == currentAlloc) {
              currentAlloc += 100;
              (*n).moves = (struct node *)realloc((void*)(*n).moves,currentAlloc*sizeof(struct node));
              end += 100;
          }
          if(dest != ' ') break;
        }
        PRINTR("\n2L\n");
        for(j2 = X-1;i(j2,Y)!=-1;j2--) {
          dest = getPiece(n->current,i(j2,Y));
          if(isBlack(dest)) break;
          PRINTR("\n0\n");
          deepCopyNode(n->moves + moveIndex,base);
          n->moves[moveIndex].current->pieces[i(X,Y)] = ' ';
          n->moves[moveIndex].current->pieces[i(j2,Y)] = 'q';
          n->moves[moveIndex].current->lastPawn = -1;
          PRINTR("\n1\n");
          if(dest == ' ')
            n->moves[moveIndex].current->drawCount++;
          else {
            n->moves[moveIndex].current->drawCount = 0;
          }
          moveIndex++;
          (*n).moveCount++;
          if(moveIndex == currentAlloc) {
              currentAlloc += 100;
              (*n).moves = (struct node *)realloc((void*)(*n).moves,currentAlloc*sizeof(struct node));
              end += 100;
          }
          if(dest != ' ') break;
        }
        PRINTR("\n3L\n");
        for(j2 = Y-1;i(X,j2)!=-1;j2--) {
          dest = getPiece(n->current,i(X,j2));
          if(isBlack(dest)) break;
          deepCopyNode(n->moves + moveIndex,base);
          n->moves[moveIndex].current->pieces[i(X,Y)] = ' ';
          n->moves[moveIndex].current->pieces[i(X,j2)] = 'q';
          n->moves[moveIndex].current->lastPawn = -1;
          if(dest == ' ')
            n->moves[moveIndex].current->drawCount++;
          else {
            n->moves[moveIndex].current->drawCount = 0;
          }
          moveIndex++;
          (*n).moveCount++;
          if(moveIndex == currentAlloc) {
              currentAlloc += 100;
              (*n).moves = (struct node *)realloc((void*)(*n).moves,currentAlloc*sizeof(struct node));
              end += 100;
          }
          if(dest != ' ') break;
        }
        PRINTR("\n4L\n");
        break;
      case 'k':
        for(j2=0; j2<8; j2++) {
          PRINT2("\n1 j2 = %d\n",j2);
          dest = getPiece(n->current,i(X+knightMovesX[j2],Y+knightMovesY[j2]));
          if(dest == ' ' || isWhite(dest)) {
            PRINT2("\n2\n");
            deepCopyNode(n->moves + moveIndex,base);
            n->moves[moveIndex].current->pieces[j] = ' ';
            n->moves[moveIndex].current->pieces[i(X+knightMovesX[j2],Y+knightMovesY[j2])] = 'n';
            n->moves[moveIndex].current->lastPawn = -1;
            PRINT2("\n3\n");
            if(dest == ' ') {
              PRINT2("\n4a\n");
              n->moves[moveIndex].current->drawCount++;
            } else {
              PRINT2("\n4b\n");
              n->moves[moveIndex].current->drawCount = 0;
            }
            moveIndex++;
            (*n).moveCount++;
            PRINT2("\n5\n");
            if(moveIndex == currentAlloc) {
              currentAlloc += 100;
              (*n).moves = (struct node *)realloc((void*)(*n).moves,currentAlloc*sizeof(struct node));
              end += 100;
            }
            PRINT2("\n6\n");
          }

        }
        break;
  }
  for(j=0; j<(*n).moveCount; j++) {
    analysis((*n).moves+j);
  }
  qsort((void *)(*n).moves,(*n).moveCount,sizeof(struct node),comparator);
}

//Main

int main() {
  struct node *n = newNode();
  struct node *temp;
  printf("\nEntering \"E\" will exit the game.\nOtherwise, enter a move as a coordinate pair (e.g. \"e2e4\").\nCastling is treated as a king move (e.g. short castleing for white will always be \"e1g1\"\nSpecific promotion will be prompted after a qualifying pawn move.\n\n");
  printBoard((*n).current);
  while(humanMove((*n).current)) {
    printBoard((*n).current);
    generateMoves(n);
    n = (*n).moves;
    printBoard((*n).current);
  }
  printf("Clean exit");
  return 0;
}
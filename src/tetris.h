#ifndef _TETRIS_H_
#define _TETRIS_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <signal.h>
#include <string.h>

#define VISIBLE_BLOCKS 3

#define WIDTH 10
#define HEIGHT 22
#define NOTHING 0
#define QUIT 'q'
#define NUM_OF_SHAPE 7
#define NUM_OF_ROTATE 4
#define BLOCK_HEIGHT 4
#define BLOCK_WIDTH 4
#define BLOCK_NUM 3

// menu number
#define MENU_PLAY '1'
#define MENU_RANK '2'
#define MENU_REC '3'
#define MENU_EXIT '4'

// 사용자 이름의 길이
#define NAMELEN 16

#define CHILDREN_MAX 36

typedef struct _RecNode
{
	int level;					  // tree의  depth
	int accumulatedScore;		  // 누적된 점수
	char recField[HEIGHT][WIDTH]; // 추천된 블록의 위치와 회전 수를 고려해서 블록을 테트리스 필드에 놓았을 때의 필드 상태
	struct _RecNode **child;	  // : tree에서 children을 가리키는 node pointer이고, child 수만큼 동적 할당

	// optional element
	int curBlockID;							  // tree에서 고려되는 block의 ID
	int recBlockX, recBlockY, recBlockRotate; // 추천된 위치와 회전 수
	int rec_flag;

} RecNode;

typedef struct _Node
{
	char name[NAMELEN];
	int score;
	struct _Node *next;
} RankNode;

/* [blockShapeID][# of rotate][][]*/
const char block[NUM_OF_SHAPE][NUM_OF_ROTATE][BLOCK_HEIGHT][BLOCK_WIDTH] = {
	{ /*[0][][][]					▩▩▩▩*/
	 {/*[][0][][]*/
	  {0, 0, 0, 0},
	  {1, 1, 1, 1},
	  {0, 0, 0, 0},
	  {0, 0, 0, 0}},
	 {/*[][1][][]*/
	  {0, 1, 0, 0},
	  {0, 1, 0, 0},
	  {0, 1, 0, 0},
	  {0, 1, 0, 0}},
	 {/*[][2][][]*/
	  {0, 0, 0, 0},
	  {1, 1, 1, 1},
	  {0, 0, 0, 0},
	  {0, 0, 0, 0}},
	 {/*[][3][][]*/
	  {0, 1, 0, 0},
	  {0, 1, 0, 0},
	  {0, 1, 0, 0},
	  {0, 1, 0, 0}}},
	{ /*[1][][][];					  ▩▩▩*/
	 {/*[][0][][]				      ▩*/
	  {0, 0, 0, 0},
	  {0, 0, 0, 0},
	  {0, 1, 1, 1},
	  {0, 0, 0, 1}},
	 {/*[][1][][]*/
	  {0, 0, 0, 0},
	  {0, 0, 1, 1},
	  {0, 0, 1, 0},
	  {0, 0, 1, 0}},
	 {/*[][2][][]*/
	  {0, 0, 0, 0},
	  {0, 1, 0, 0},
	  {0, 1, 1, 1},
	  {0, 0, 0, 0}},
	 {/*[][3][][]*/
	  {0, 0, 0, 0},
	  {0, 0, 1, 0},
	  {0, 0, 1, 0},
	  {0, 1, 1, 0}}},
	{ /*[2][][][];					  ▩▩▩*/
	 {/*[][0][][]				  ▩*/
	  {0, 0, 0, 0},
	  {0, 0, 0, 0},
	  {0, 1, 1, 1},
	  {0, 1, 0, 0}},
	 {/*[][1][][]*/
	  {0, 0, 0, 0},
	  {0, 0, 1, 0},
	  {0, 0, 1, 0},
	  {0, 0, 1, 1}},
	 {/*[][2][][]*/
	  {0, 0, 0, 0},
	  {0, 0, 0, 1},
	  {0, 1, 1, 1},
	  {0, 0, 0, 0}},
	 {/*[][3][][]*/
	  {0, 0, 0, 0},
	  {0, 1, 1, 0},
	  {0, 0, 1, 0},
	  {0, 0, 1, 0}}},
	{ /*[3][][][];					  ▩▩▩*/
	 {/*[][0][][]				    ▩*/
	  {0, 0, 0, 0},
	  {0, 1, 0, 0},
	  {1, 1, 1, 0},
	  {0, 0, 0, 0}},
	 {/*[][1][][]*/
	  {0, 0, 0, 0},
	  {0, 1, 0, 0},
	  {1, 1, 0, 0},
	  {0, 1, 0, 0}},
	 {/*[][2][][]*/
	  {0, 0, 0, 0},
	  {0, 0, 0, 0},
	  {1, 1, 1, 0},
	  {0, 1, 0, 0}},
	 {/*[][3][][]*/
	  {0, 0, 0, 0},
	  {0, 1, 0, 0},
	  {0, 1, 1, 0},
	  {0, 1, 0, 0}}},
	{ /*[4][][][];					  ▩▩*/
	 {/*[][0][][]				  ▩▩*/
	  {0, 0, 0, 0},
	  {0, 0, 0, 0},
	  {0, 1, 1, 0},
	  {0, 1, 1, 0}},
	 {/*[][1][][]*/
	  {0, 0, 0, 0},
	  {0, 0, 0, 0},
	  {0, 1, 1, 0},
	  {0, 1, 1, 0}},
	 {/*[][2][][]*/
	  {0, 0, 0, 0},
	  {0, 0, 0, 0},
	  {0, 1, 1, 0},
	  {0, 1, 1, 0}},
	 {/*[][3][][]*/
	  {0, 0, 0, 0},
	  {0, 0, 0, 0},
	  {0, 1, 1, 0},
	  {0, 1, 1, 0}}},
	{ /*[5][][][];					  ▩▩*/
	 {/*[][0][][]				▩▩*/
	  {0, 0, 0, 0},
	  {0, 0, 1, 1},
	  {0, 1, 1, 0},
	  {0, 0, 0, 0}},
	 {/*[][1][][]*/
	  {0, 0, 0, 0},
	  {0, 1, 0, 0},
	  {0, 1, 1, 0},
	  {0, 0, 1, 0}},
	 {/*[][2][][]*/
	  {0, 0, 0, 0},
	  {0, 0, 1, 1},
	  {0, 1, 1, 0},
	  {0, 0, 0, 0}},
	 {/*[][3][][]*/
	  {0, 0, 0, 0},
	  {0, 1, 0, 0},
	  {0, 1, 1, 0},
	  {0, 0, 1, 0}}},
	{ /*[6][][][];					▩▩*/
	 {/*[][0][][]				  ▩▩*/
	  {0, 0, 0, 0},
	  {0, 0, 0, 0},
	  {0, 1, 1, 0},
	  {0, 0, 1, 1}},
	 {/*[][1][][]*/
	  {0, 0, 0, 0},
	  {0, 0, 1, 0},
	  {0, 1, 1, 0},
	  {0, 1, 0, 0}},
	 {/*[][2][][]*/
	  {0, 0, 0, 0},
	  {0, 0, 0, 0},
	  {0, 1, 1, 0},
	  {0, 0, 1, 1}},
	 {/*[][3][][]*/
	  {0, 0, 0, 0},
	  {0, 0, 1, 0},
	  {0, 1, 1, 0},
	  {0, 1, 0, 0}}}};

char field[HEIGHT][WIDTH];		 /* 테트리스의 메인 게임 화면 */
int nextBlock[BLOCK_NUM];		 /* 현재 블럭의 ID와 다음 블럭의 ID들을 저장; [0]: 현재 블럭; [1]: 다음 블럭 */
int blockRotate, blockY, blockX; /* 현재 블럭의 회전, 블럭의 Y 좌표, 블럭의 X 좌표*/
int score;						 /* 점수가 저장*/
int gameOver = 0;				 /* 게임이 종료되면 1로 setting된다.*/
int timed_out;
int recommendR, recommendY, recommendX; // 추천 블럭 배치 정보. 차례대로 회전, Y 좌표, X 좌표
RecNode *recRoot;

// 테트리스의 모든  global 변수를 초기화 해준다.
void InitTetris();

// 테트리스의 모든  interface를 그려준다.
void DrawOutline();

// 테트리스와 관련된 키입력을 받는다.
int GetCommand();

// GetCommand로 입력받은 command에 대한 동작을 수행한다.
int ProcessCommand(int command);

// 블럭이 일정 시간(1초)마다 내려가도록 호출되는 함수
void BlockDown(int sig);

// 입력된 움직임이 가능한지를 판단해주는 함수.
int CheckToMove(char f[HEIGHT][WIDTH], int currentBlock, int blockRotate, int blockY, int blockX);

// 테트리스에서 command에 의해 바뀐 부분만 다시 그려준다.
void DrawChange(char f[HEIGHT][WIDTH], int command, int currentBlock, int blockRotate, int blockY, int blockX);

// 테트리스의 블럭이 쌓이는 field를 그려준다.
void DrawField();

// 떨어지는 블럭을 field에 더해준다.
int AddBlockToField(char f[HEIGHT][WIDTH], int currentBlock, int blockRotate, int blockY, int blockX);

// 완전히 채워진 Line을 삭제하고 점수를 매겨준다.
int DeleteLine(char f[HEIGHT][WIDTH]);

// 커서의 위치를 입력된 x, y의 위치로 옮겨주는 역할을 한다.
void gotoyx(int y, int x);

// 테트리스의 화면 오른쪽상단에 다음 나올 블럭을 그려준다..
void DrawNextBlock(int *nextBlock);

// 테트리스의 화면 오른쪽 하단에 Score를 출력한다.
void PrintScore(int score);

// 해당 좌표(y,x)에 원하는 크기(height,width)의 box를 그린다.
void DrawBox(int y, int x, int height, int width);

// 해당 좌표(y,x)에 원하는 모양의 블록을 그린다.
void DrawBlock(int y, int x, int blockID, int blockRotate, char tile);

// 블록이 떨어질 위치를 미리 보여준다.
void DrawShadow(int y, int x, int blockID, int blockRotate);

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate);

// 테트리스 게임을 시작한다.
void play();

// 메뉴를 보여준다.
char menu();

// rank file로부터 랭킹 정보를 읽어와 랭킹 목록을 구성한다.
void createRankList();

// 화면에 랭킹 기록들을 보여준다.
void rank();

// rank file을 생성한다.
void writeRankFile();

// 새로운 랭킹 정보를 추가한다.
void newRank(int score);

// 추천 블럭 배치를 구한다.
int recommend(RecNode *root);
int modified_recommend(RecNode *t_root);
// 추천 기능에 따라 블럭을 배치하여 진행하는 게임을 시작한다.
void recommendedPlay();
void Rec_BlockDown(int sig);
void autoMove();

#endif

#include "tetris.h"
#include <string.h>
#include <unistd.h>

static struct sigaction act, oact;

RankNode *head_Node = NULL;
RecNode *root = NULL;
int rec_autoMove = 0;

int main()
{
	int exit = 0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);

	srand((unsigned int)time(NULL));

	createRankList();
	while (!exit)
	{
		clear();
		switch (menu())
		{
		case MENU_PLAY:
			play();
			break;
		case MENU_RANK:
			rank();
			break;
		case MENU_REC:
			recommendedPlay();
			break;
		case MENU_EXIT:
			exit = 1;
			break;
		default:
			break;
		}
	}

	endwin();
	system("clear");
	return 0;
}
void InitTetris()
{
	int i, j;

	for (j = 0; j < HEIGHT; j++)
		for (i = 0; i < WIDTH; i++)
			field[j][i] = 0;

	nextBlock[0] = rand() % 7;
	nextBlock[1] = rand() % 7;
	nextBlock[2] = rand() % 7;

	blockRotate = 0;
	blockY = -1;
	blockX = WIDTH / 2 - 2;
	score = 0;
	gameOver = 0;
	timed_out = 0;

	DrawOutline();
	DrawField();

	DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);

	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline()
{
	int i, j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0, 0, HEIGHT, WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2, WIDTH + 10);
	printw("NEXT BLOCK");
	DrawBox(3, WIDTH + 10, 4, 8);

	move(9, WIDTH + 10);
	DrawBox(10, WIDTH + 10, 4, 8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(16, WIDTH + 10);
	printw("SCORE");
	DrawBox(17, WIDTH + 10, 1, 8);
}

int GetCommand()
{
	int command;
	command = wgetch(stdscr);
	switch (command)
	{
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ': /* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command)
{
	int ret = 1;
	int drawFlag = 0;
	switch (command)
	{
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if ((drawFlag = CheckToMove(field, nextBlock[0], (blockRotate + 1) % 4, blockY, blockX)))
			blockRotate = (blockRotate + 1) % 4;
		break;
	case KEY_DOWN:
		if ((drawFlag = CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if ((drawFlag = CheckToMove(field, nextBlock[0], blockRotate, blockY, blockX + 1)))
			blockX++;
		break;
	case KEY_LEFT:
		if ((drawFlag = CheckToMove(field, nextBlock[0], blockRotate, blockY, blockX - 1)))
			blockX--;
		break;
	default:
		break;
	}
	if (drawFlag)
		DrawChange(field, command, nextBlock[0], blockRotate, blockY, blockX);
	return ret;
}

void DrawField()
{
	int i, j;
	for (j = 0; j < HEIGHT; j++)
	{
		move(j + 1, 1);
		for (i = 0; i < WIDTH; i++)
		{
			if (field[j][i] == 1)
			{
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else
				printw(".");
		}
	}
}

void PrintScore(int score)
{
	move(18, WIDTH + 11);
	printw("%8d", score);
}

void DrawNextBlock(int *nextBlock)
{
	int i, j;
	for (i = 0; i < 4; i++)
	{
		move(4 + i, WIDTH + 13);

		for (j = 0; j < 4; j++)
		{
			if (block[nextBlock[1]][0][i][j] == 1)
			{
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else
				printw(" ");
		}
	}

	for (i = 0; i < 4; i++)
	{
		move(11 + i, WIDTH + 13);

		for (j = 0; j < 4; j++)
		{
			if (block[nextBlock[2]][0][i][j] == 1)
			{
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else
				printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID, int blockRotate, char tile)
{
	int i, j;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
		{
			if (block[blockID][blockRotate][i][j] == 1 && i + y >= 0)
			{
				move(i + y + 1, j + x + 1);
				attron(A_REVERSE);
				printw("%c", tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT, WIDTH + 10);
}

void DrawBox(int y, int x, int height, int width)
{
	int i, j;
	move(y, x);
	addch(ACS_ULCORNER);
	for (i = 0; i < width; i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for (j = 0; j < height; j++)
	{
		move(y + j + 1, x);
		addch(ACS_VLINE);
		move(y + j + 1, x + width + 1);
		addch(ACS_VLINE);
	}
	move(y + j + 1, x);
	addch(ACS_LLCORNER);
	for (i = 0; i < width; i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play()
{
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM, &act, &oact);
	InitTetris();
	do
	{
		if (timed_out == 0)
		{
			alarm(1);
			timed_out = 1;
		}

		command = GetCommand();
		if (ProcessCommand(command) == QUIT)
		{
			alarm(0);
			DrawBox(HEIGHT / 2 - 1, WIDTH / 2 - 5, 1, 10);
			move(HEIGHT / 2, WIDTH / 2 - 4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	} while (!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT / 2 - 1, WIDTH / 2 - 5, 1, 10);
	move(HEIGHT / 2, WIDTH / 2 - 4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu()
{
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

int CheckToMove(char f[HEIGHT][WIDTH], int currentBlock, int blockRotate, int blockY, int blockX)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			int new_Y = blockY + i;
			int new_X = blockX + j;

			if (block[currentBlock][blockRotate][i][j] == 1)
			{
				// field 벗어남
				if (new_Y < 0 || new_Y >= HEIGHT || new_X < 0 || new_X >= WIDTH)
					return 0;

				// 블록과 겹침
				if (f[new_Y][new_X] == 1)
					return 0;
			}
		}
	}

	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH], int command, int currentBlock, int blockRotate, int blockY, int blockX)
{
	// 입력 명령을 역으로 적용해 이전 블록 위치와 회전을 구한다.
	int prev_Y = blockY;
	int prev_X = blockX;
	int prev_Rotate = blockRotate;

	switch (command)
	{
	case KEY_UP:
		prev_Rotate = (prev_Rotate + 3) % 4; // 0 1 2 3
		break;
	case KEY_DOWN:
		prev_Y -= 1;
		break;
	case KEY_RIGHT:
		prev_X -= 1;
		break;
	case KEY_LEFT:
		prev_X += 1;
		break;
	default:
		break;
	}

	// 이전에 그려진 블록을 화면에서 지운다.
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{

			if (block[currentBlock][blockRotate][i][j] == 1)
			{
				// 이전 블록을 화면에서 지운다
				f[prev_Y + i][prev_X + j] = 0;
				DrawField();
			}
		}
	}

	// 새로운 블록 정보를 그린다.
	DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);

	root = (RecNode *)malloc(sizeof(RecNode));
	root->level = 0;
	root->accumulatedScore = 0;
	root->curBlockID = nextBlock[0];
	root->recBlockX = 0;
	root->recBlockY = 0;
	root->recBlockRotate = 0;
	for (int i = 0; i < HEIGHT; i++)
		for (int j = 0; j < WIDTH; j++)
			root->recField[i][j] = 0;
	int max = modified_recommend(root);
	DrawBlock(root->recBlockY, root->recBlockX, root->curBlockID, root->recBlockRotate, 'R');

	// move로 커서를 필드 밖으로
	move(HEIGHT + 10, WIDTH + 10);
}

void BlockDown(int sig)
{
	// 블록이 한칸 내려갈수 있으면 함수 종료
	if (CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX))
	{
		blockY++;
		DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
		timed_out = 0;
		return;
	}

	// 블록이 더 이상 내려갈수없는경우
	// blockY가 초기값인 -1일경우 게임오버
	if (blockY == -1)
	{
		gameOver = 1;
		return;
	}

	// 블록을 필드에 쌓는다
	score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);

	// 완전한 line을 지우고 점수갱신
	score += DeleteLine(field); // 여기서 스코어 갱신
	DrawField();
	PrintScore(score);

	nextBlock[0] = nextBlock[1];
	nextBlock[1] = nextBlock[2];
	nextBlock[2] = rand() % 7;

	DrawNextBlock(nextBlock);

	// 블록위치초기화 & 종료
	blockX = 0;
	blockY = -1;
	timed_out = 0;
	return;
}

int AddBlockToField(char f[HEIGHT][WIDTH], int currentBlock, int blockRotate, int blockY, int blockX)
{
	// Block이 추가된 영역의 필드값을 바꾼다.
	int touched = 0; // 닿은 면적

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (block[currentBlock][blockRotate][i][j] == 1)
			{
				f[blockY + i][blockX + j] = 1;

				// 더 이상 블록이 내려갈 수 없을 때, 블록을 필드에 추가하는 과정에서 필드에 추가되는 위치의 바로 아래에 필드가 채워져 있는지를 검사한다. 만약 채워져 있다면(1이라면), touched++

				if (f[blockY + i + 1][blockX + j] == 1 || (blockY + i + 1 == HEIGHT))
					touched++;
			}
		}
	}

	return 10 * touched;
}

int DeleteLine(char f[HEIGHT][WIDTH])
{
	int count_delete = 0;

	// 채워진 line을 찾을 때는 필드에서 한 줄의 element가 모두 1이어야 한다.
	for (int r = 0; r < HEIGHT; r++)
	{
		int flag_isFull = 1; // 임시로 한줄이 채워졌는지 확인하는 플래그

		for (int c = 0; c < WIDTH; c++)
		{
			if (f[r][c] == 0)
			{
				flag_isFull = 0;
				break;
			}
		}

		// 꽉 찬 구간이 있으면 해당 구간을 지운다. 즉, 해당 구간으로 필드값을 한칸씩 내린다.
		// 찾으면(flag가 check된 경우) 지워진 line 바로 위에서부터, 필드에 쌓인 블록의 정보를 한 줄씩 내려준다.
		if (flag_isFull)
		{
			count_delete++;

			for (int i = r - 1; i >= 0; i--)
			{
				for (int j = 0; j < WIDTH; j++)
				{
					f[i + 1][j] = f[i][j];
				}
			}

			for (int j = 0; j < WIDTH; j++)
			{
				f[0][j] = 0;
			}
		}
	}

	return (count_delete * count_delete) * 100;
}

void DrawShadow(int y, int x, int blockID, int blockRotate)
{
	int max_row = y;

	while (CheckToMove(field, nextBlock[0], blockRotate, max_row + 1, blockX))
	{
		max_row++;
	}

	DrawBlock(max_row, x, blockID, blockRotate, '/');
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate)
{
	DrawShadow(y, x, blockID, blockRotate);
	DrawBlock(y, x, blockID, blockRotate, ' ');

}

void createRankList()
{
	// 저장된 순위를 연결 리스트로 불러온다.
	FILE *fp;
	int i, j;
	int sn;

	char name[NAMELEN];
	int score;

	// 파일 열기
	fp = fopen("rank.txt", "r");
	if (fp == NULL)
	{
		printw("File open Error");
		return;
	}
	if (fscanf(fp, "%d", &sn) != 1) {
		fclose(fp);
		return;
	}

	// 정보읽어오기

	while (fscanf(fp, "%15s %d", name, &score) == 2)
	{
		// LinkedList로 저장
		RankNode *newNode = (RankNode *)malloc(sizeof(RankNode));
		if (newNode == NULL)
		{
			printw("Memory Alloc Error");
			fclose(fp);
			return;
		}
		strcpy(newNode->name, name);
		newNode->score = score;
		newNode->next = NULL;

		if (head_Node == NULL ||
			head_Node->score <= newNode->score)
		{
			newNode->next = head_Node;
			head_Node = newNode;
		}
		else
		{
			RankNode *currentNode = head_Node;
			while (currentNode->next != NULL && currentNode->next->score >= newNode->score)
				currentNode = currentNode->next;

			newNode->next = currentNode->next;
			currentNode->next = newNode;
		}
	}

	// 파일닫기
	fclose(fp);
}

void rank()
{
	// 순위 조회와 삭제 메뉴를 표시한다.
	FILE *fp = fopen("rank.txt", "r");
	if (fp == NULL)
	{
		printw("File open Error");
		return;
	}

	int X = 1, Y = 0, ch;
	fscanf(fp, "%d", &Y); // 총 개수
	fclose(fp);
	clear();

	// printw()로 3개의 메뉴출력
	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");

	// wgetch()를 사용하여 변수 ch에 입력받은 메뉴번호 저장
	ch = wgetch(stdscr);

	// 각 메뉴에 따라 입력받을 값을 변수에 저장
	// 메뉴1: X, Y를 입력받고 적절한 input인지 확인 후(X<=Y), X와 Y사이의 rank 출력
	RankNode *currentNode = head_Node;
	if (ch == '1')
	{
		printw("X:");
		echo();
		scanw("%d", &X);
		printw("Y:");
		scanw("%d", &Y);
		noecho();

		if (X > Y)
		{
			echo();
			printw("search failure: no rank in the list");
			noecho();
		}
		else
		{
			int count = 0;

			printw("          name          |     score          \n");
			printw("------------------------------------------\n");

			echo();
			// X~Y 범위의 랭킹 출력
			while (currentNode != NULL)
			{
				count++;
				if (count >= X && count <= Y)
				{
					printw("%-24s| %d\n", currentNode->name, currentNode->score);
				}
				currentNode = currentNode->next;
			}
			noecho();
		}
	}

	// 메뉴2: 문자열을 받아 저장된 이름과 비교하고 이름에 해당하는 리스트를 출력
	else if (ch == '2')
	{
		char str[NAMELEN + 1] = "";
		int check = 0;

		printw("input the name: ");
		echo();
		scanw("%15s", str);

		printw("          name          |     score          \n");
		printw("------------------------------------------\n");

		while (currentNode != NULL)
		{
			if (strcmp(currentNode->name, str) == 0)
			{
				printw("%-24s| %d\n", currentNode->name, currentNode->score);
				check = 1;
			}
			currentNode = currentNode->next;
		}

		if (!check)
			printw("\nsearch failure: no name in the list");

		noecho();
	}

	// 메뉴3: rank번호를 입력받아 리스트에서 삭제
	else if (ch == '3')
	{
		int num = 0;

		printw("input the rank: ");
		echo();
		scanw("%d", &num);

		RankNode *previousNode = NULL;
		while (currentNode != NULL && num > 1)
		{
			previousNode = currentNode;
			currentNode = currentNode->next;
			num--;
		}
		if (currentNode == NULL || num < 1)
		{
			printw("\nsearch failure: the rank not in the list");
		}
		else
		{
			if (previousNode == NULL)
				head_Node = currentNode->next;
			else
				previousNode->next = currentNode->next;
			free(currentNode);
			printw("\nresult: the rank deleted");
		}
		noecho();

		writeRankFile();
	}
	getch();
}

void writeRankFile()
{
	int count = 0;
	RankNode *currentNode = head_Node;
	while (currentNode != NULL)
	{
		count++;
		currentNode = currentNode->next;
	}

	FILE *fp = fopen("rank.txt", "w");
	if (fp == NULL)
	{
		printw("File open Error");
		return;
	}
	fprintf(fp, "%d\n", count);
	for (currentNode = head_Node; currentNode != NULL; currentNode = currentNode->next)
		fprintf(fp, "%s %d\n", currentNode->name, currentNode->score);
	fclose(fp);
}

void newRank(int score)
{
	// 게임 종료 후 이름과 점수를 순위 목록에 추가한다.
	char str[NAMELEN + 1] = "";
	int i, j;
	clear();

	echo();
	// 사용자 이름을 입력받음
	printw("your name: ");
	scanw("%15s", str);
	noecho();

	if (str[0] == '\0') return;

	// 이름과 점수를 저장할 노드를 생성한다.
	RankNode *newNode = (RankNode *)malloc(sizeof(RankNode));
	if (newNode == NULL)
	{
		printw("Memory Alloc Error");
		return;
	}
	strcpy(newNode->name, str);
	newNode->score = score;
	newNode->next = NULL;

	RankNode *currentNode = head_Node;
	RankNode *previousNode = NULL;

	// 빈 LL
	while (currentNode != NULL && currentNode->score >= score)
	{
		previousNode = currentNode;
		currentNode = currentNode->next;
	}

	if (previousNode == NULL)
	{
		newNode->next = head_Node;
		head_Node = newNode;
	}
	else
	{
		previousNode->next = newNode;
		newNode->next = currentNode;
	}

	writeRankFile();
}

int recommend(RecNode *t_root)
{
	int max = 0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수
	int totalScore = t_root->accumulatedScore;
	int currentBlock = nextBlock[t_root->level];

	// 블록마다 다른 회전값
	int R;
	if (currentBlock == 0 || currentBlock == 5 || currentBlock == 6)
		R = 2;
	else if (currentBlock == 4)
		R = 1;
	else
		R = 4;

	// 현재 블록의 모든 회전 상태 탐색
	for (int r = 0; r < R; r++)
	{
		// 현재 상태에서 가능한 모든 위치 탐색
		for (int x = -1; x < WIDTH; x++)
		{
			int y = -1;
			// 블록이 해당 위치에 놓일수 있는지
			if (!CheckToMove(field, currentBlock, r, y, x))
				continue;

			// 노드에 저장할 임시 필드 생성
			char tempField[HEIGHT][WIDTH];
			for (int i = 0; i < HEIGHT; i++)
				for (int j = 0; j < WIDTH; j++)
					tempField[i][j] = field[i][j];

			// 놓을수 있다면 가장 아래 좌표를 찾기 -> checkToMove 참고
			while (CheckToMove(tempField, currentBlock, r, y, x))
				y++;
			y--;

			totalScore += AddBlockToField(tempField, currentBlock, r, y, x);
			totalScore += DeleteLine(tempField);

			if (totalScore >= max)
			{
				if (VISIBLE_BLOCKS == 1)
				{
					if (totalScore > max || (totalScore == max && root->recBlockY < y))
					{
						root->recBlockX = x;
						root->recBlockY = y;
						root->recBlockRotate = r;
					}
				}
				max = totalScore;
			}

			// 정보를 저장할 자식 노드 생성
			RecNode *childNode = (RecNode *)malloc(sizeof(RecNode));
			childNode->level = t_root->level + 1;
			childNode->accumulatedScore = totalScore;

			// 해당 경로의 필드 저장
			for (int i = 0; i < HEIGHT; i++)
			{
				for (int j = 0; j < WIDTH; j++)
				{
					childNode->recField[i][j] = tempField[i][j];
				}
			}

			if (childNode->level < VISIBLE_BLOCKS)
			{
				int childMax = recommend(childNode);
				if (childMax >= max)
				{
					if (t_root->level == 0 && (childMax > max || (childMax == max && root->recBlockY < y)))
					{
						root->recBlockX = x;
						root->recBlockY = y;
						root->recBlockRotate = r;
					}
					max = childMax;
				}
			}
			totalScore = t_root->accumulatedScore;
		}
	}
	return max;
}

int modified_recommend(RecNode *t_root)
{
	int max = 0;
	int totalScore = t_root->accumulatedScore;

	int currentBlock = nextBlock[t_root->level];
	// 블록마다 다른 회전값
	int R;
	if (currentBlock == 0 || currentBlock == 5 || currentBlock == 6)
		R = 2;
	else if (currentBlock == 4)
		R = 1;
	else
		R = 4;

	// Pruning 기준: 이미 탐색 중 최댓값보다 낮은 경로
	int pruning = t_root->accumulatedScore;

	// 현재 블록의 모든 회전 상태 탐색
	for (int r = 0; r < R; r++)
	{
		// 현재 상태에서 가능한 모든 위치 탐색
		for (int x = -1; x < WIDTH; x++)
		{
			int y = -1;
			// 블록이 해당 위치에 놓일수 있는지
			if (!CheckToMove(field, currentBlock, r, y, x))
				continue;

			// 노드에 저장할 임시 필드 생성
			char tempField[HEIGHT][WIDTH];
			for (int i = 0; i < HEIGHT; i++)
				for (int j = 0; j < WIDTH; j++)
					tempField[i][j] = field[i][j];

			// 놓을수 있다면 가장 아래 좌표를 찾기 -> checkToMove 참고
			while (CheckToMove(field, currentBlock, r, y, x))
				y++;
			y--;

			totalScore += AddBlockToField(tempField, currentBlock, r, y, x);
			totalScore += DeleteLine(tempField);

			// 현재 점수가 기준 이하이면 중단
			if (totalScore <= pruning)
			{
				totalScore = t_root->accumulatedScore;
				continue;
			}

			if (totalScore >= max)
			{
				if (VISIBLE_BLOCKS == 1)
				{
					if (totalScore > max || (totalScore == max && root->recBlockY < y))
					{
						root->recBlockX = x;
						root->recBlockY = y;
						root->recBlockRotate = r;
					}
				}
				max = totalScore;
			}

			// 정보를 저장할 자식 노드 생성
			RecNode *childNode = (RecNode *)malloc(sizeof(RecNode));
			childNode->level = t_root->level + 1;
			childNode->accumulatedScore = totalScore;

			// 해당 경로의 필드 저장
			for (int i = 0; i < HEIGHT; i++)
			{
				for (int j = 0; j < WIDTH; j++)
				{
					childNode->recField[i][j] = tempField[i][j];
				}
			}

			if (childNode->level < VISIBLE_BLOCKS)
			{
				int childMax = recommend(childNode);
				if (childMax >= max)
				{
					if (t_root->level == 0 && (childMax > max || (childMax == max && root->recBlockY < y)))
					{
						root->recBlockX = x;
						root->recBlockY = y;
						root->recBlockRotate = r;
					}
					max = childMax;
				}
			}
			totalScore = t_root->accumulatedScore;
		}
	}
	return max;
}

int ProcessOnlyQuit(int command)
{
	int ret = 1;
	int drawFlag = 0;
	switch (command)
	{
	case QUIT:
		ret = QUIT;
		break;
	default:
		break;
	}
	return ret;
}

void recommendedPlay()
{
	int command;
	clear();
	act.sa_handler = Rec_BlockDown;

	sigaction(SIGALRM, &act, &oact);
	InitTetris();

	do
	{
		if (timed_out == 0)
		{
			alarm(1);
			timed_out = 1;
		}

		command = GetCommand();
		if (ProcessOnlyQuit(command) == QUIT) // 수행가능 commend는 오직 Quit
		{
			alarm(0);
			DrawBox(HEIGHT / 2 - 1, WIDTH / 2 - 5, 1, 10);
			move(HEIGHT / 2, WIDTH / 2 - 4);
			printw("Good-bye!!");
			refresh();
			getch();
			rec_autoMove = 0;

			return;
		}
	} while (!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT / 2 - 1, WIDTH / 2 - 5, 1, 10);
	move(HEIGHT / 2, WIDTH / 2 - 4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

void autoMove()
{
	if (rec_autoMove == 1)
		return;

	int max = 0;

	// 이전 블록을 화면에서 지운다
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (block[nextBlock[0]][blockRotate][i][j] == 1)
			{
				field[blockY + i][blockX + j] = 0;
				DrawField();
			}
		}
	}

	root = (RecNode *)malloc(sizeof(RecNode));
	if (!root)
	{
		printf("Memory allocation failed!\n");
		return;
	}
	root->level = 0;
	root->accumulatedScore = 0;
	root->curBlockID = nextBlock[0];
	root->recBlockX = 0;
	root->recBlockY = 0;
	root->recBlockRotate = 0;

	// 필드 복사
	for (int i = 0; i < HEIGHT; i++)
		for (int j = 0; j < WIDTH; j++)
			root->recField[i][j] = field[i][j];

	// 추천 블록 계산
	max = modified_recommend(root);

	blockX = root->recBlockX;
	blockRotate = root->recBlockRotate;

	// Y값과 X값이 유효한지 확인
	DrawBlock(blockY, blockX, root->curBlockID, root->recBlockRotate, ' ');
	DrawBlock(root->recBlockY, root->recBlockX, root->curBlockID, root->recBlockRotate, 'R');

	rec_autoMove = 1;
}

void Rec_BlockDown(int sig)
{
	autoMove();

	// 블록이 한 칸 내려갈 수 있으면
	if (CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX))
	{
		blockY++;
		DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
		timed_out = 0;
		return;
	}

	// 블록이 더 이상 내려갈 수 없는 경우
	if (blockY == -1)
	{
		gameOver = 1;
		return;
	}

	// 블록을 필드에 추가
	score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);

	// 완전한 line 지우기 및 점수 갱신
	score += DeleteLine(field);
	DrawField();
	PrintScore(score);

	// 다음 블록 설정
	nextBlock[0] = nextBlock[1];
	nextBlock[1] = nextBlock[2];
	nextBlock[2] = rand() % 7;

	DrawNextBlock(nextBlock);

	// 블록 위치 초기화
	blockX = 0;
	blockY = -1;
	timed_out = 0;
	rec_autoMove = 0;
}

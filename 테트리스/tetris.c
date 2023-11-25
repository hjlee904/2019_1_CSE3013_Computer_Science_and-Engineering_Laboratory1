#include "tetris.h"

static struct sigaction act, oact;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));

	createRankList();

	recRoot = (RecNode *)malloc(sizeof(RecNode));
	recRoot->lv = -1;
	recRoot->score = 0;
	recRoot->f = field;
	constructRecTree(recRoot);

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
		case MENU_RECO: recommendedPlay(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}

	writeRankFile();

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
	nextBlock[2]=rand()%7;
	recommend(recRoot);
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();
	DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);
	DrawBox(9,WIDTH+10,4,8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(15,WIDTH+10);
	printw("SCORE");
	DrawBox(16,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
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

int recGetCommand() {
	int command;
	command = wgetch(stdscr);
	switch (command) {
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

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(17,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j, k;
	for( k = 1; k < 3; k++){
		for( i = 0; i < 4; i++ ){
			move(4+i+(k-1)*6,WIDTH+13);
			for( j = 0; j < 4; j++ ){
				if( block[nextBlock[k]][0][i][j] == 1 ){
					attron(A_REVERSE);
					printw(" ");
					attroff(A_REVERSE);
				}
				else printw(" ");
			}
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch('o');
	for(i=0;i<width;i++)
		addch('-');
	addch('o');
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch('|');
		move(y+j+1,x+width+1);
		addch('|');
	}
	move(y+j+1,x);
	addch('o');
	for(i=0;i<width;i++)
		addch('-');
	addch('o');
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	int i, j;
	for(i=0; i<4; i++){
		for(j=0; j<4; j++){
			if(block[currentBlock][blockRotate][i][j]==1){
				if(f[blockY+i][blockX+j]){
					return 0;
				}
				if(blockY+i >= HEIGHT){
					return 0;
				}
				if(blockX+j < 0){
					return 0;
				}
				if(blockX+j >= WIDTH){
					return 0;
				}
			}
		}
	}
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	int i, j;
	int blk = currentBlock, rot = blockRotate, y = blockY, x = blockX;
	int ys = y, xs = x;
	switch(command){
		case KEY_UP:
				rot = (rot+3)%4;
				break;
		case KEY_DOWN:
				y--;
				break;
		case KEY_LEFT:
				x++;
				break;
		case KEY_RIGHT:
				x--;
				break;
	}
	ys = y;
	xs = x;
	while(1){
		if(CheckToMove(field, nextBlock[0], rot, ys, xs)==1){
			ys++;
		}
		else break;
	}
	for(i=0; i<4; i++){
		for(j=0; j<4; j++){
			if(block[blk][rot][i][j]==1){
				move(i+ys, j+xs+1);
				printw(".");
			}
		}
	}
	for(i=0; i<4; i++){
		for(j=0; j<4; j++){
			if(block[blk][rot][i][j]==1){
				if(i+y>=0){
					move(i+y+1, j+x+1);
					printw(".");
				}
			}
		}
	}
	DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);
	move(HEIGHT, WIDTH+10);
}

void BlockDown(int sig){
	if(CheckToMove(field, nextBlock[0], blockRotate, blockY+1, blockX)){
		blockY++;
		DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
	}
	else {
		score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
		if(blockY==-1){
			gameOver = 1;
		}
		else {
			score += DeleteLine(field);
			blockRotate = 0;
			blockY = -1;
			blockX = WIDTH/2-2;
			nextBlock[0] = nextBlock[1];
			nextBlock[1] = nextBlock[2];
			nextBlock[2] = rand()%7;
			recommend(recRoot);

			DrawNextBlock(nextBlock);
			DrawBlock(blockY, blockX, nextBlock[0], blockRotate, ' ');
			PrintScore(score);
		}
		DrawField();
	}
	timed_out = 0;
}

void recBlockDown(int sig) {
	int i;
	if (CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX)) {
		blockY++;
		DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
	}
	else {
		score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
		if (blockY == -1) {
			gameOver = 1;
		}
		else {
			score += DeleteLine(field);
			blockRotate = 0;
			blockY = -1;
			blockX = (WIDTH / 2) - 2;
			nextBlock[0] = nextBlock[1];
			nextBlock[1] = nextBlock[2];
			nextBlock[2] = rand() % 7;
			recommend(recRoot);

			blockX = recommendX;
			blockRotate = recommendR;
			DrawNextBlock(nextBlock);
			//recommendFree();
			PrintScore(score);
		}
		DrawField();
	}
	timed_out = 0;
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	int i, j;
	int touched = 0;
	for(i=0; i<4; i++){
		for(j=0; j<4; j++){
			if(block[currentBlock][blockRotate][i][j]==1){
				if(0 <= blockY+i && blockY+i < HEIGHT && 0 <= blockX+j && blockX+j < WIDTH){
					f[blockY+i][blockX+j] = 1;
					if(blockY+i == HEIGHT-1){
						touched++;
					}
				}
			}
		}
	}
	return touched * 10;
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	int i, j, k, fillFlag;
	int count = 0;
	for(i=0; i<HEIGHT; i++){
		fillFlag = 1;
		for(j=0; j<WIDTH; j++){
			if(f[i][j]==0){
				fillFlag = 0;
				break;
			}
		}
		if(fillFlag){
			count++;
			for(k=i-1; k>=0; k--){
				for(j=WIDTH; j>0; j--){
					f[k+1][j] = f[k][j];
				}
			}
		}
	}
	return count*count*100;
}

void DrawShadow(int y, int x, int blockID,int blockRotate){
	char tile = '/';
	while(CheckToMove(field, nextBlock[0], blockRotate, y, x)){
			y++;
	}
	DrawBlock(y-1, x, blockID, blockRotate, tile);
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate){
	DrawShadow(y, x, blockID, blockRotate);
	DrawRecommend();
	DrawBlock(y, x, blockID, blockRotate, ' ');
}

void createRankList(){
	rPointer temp = NULL;
	int i;

	FILE* fp;
	if((fp = fopen("rank.txt", "r"))==NULL){
		return;
	}
	else{
		head = (rPointer)malloc(sizeof(rP));
		head->link = NULL;
		if(fscanf(fp, "%d", &n)==EOF){
			return;
		}
		else{
			temp = head;
			for( i=0; i<n; i++ ){
				temp->link = (rPointer)malloc(sizeof(rP));
				temp = temp->link;
				fscanf(fp, "%s", temp->userName);
				fscanf(fp, "%d", &(temp->userScore));
				temp->link = NULL;
			}
		}
	}
	head = head->link;
	fclose(fp);
}

void rank(){
	int from = 1, to = n;
	char name1[NAMELEN];
	int del, count;

	rPointer temp = NULL;
	temp = head;

	clear();
	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");
	switch(wgetch(stdscr)){
		case '1':
			echo();
			printw("X: ");
			scanw("%d", &from);
			printw("Y: ");
			scanw("%d", &to);
			noecho();
			printw("      name      |    score    \n");
			printw("------------------------------\n");
			if( from>0 && to>0 && from<=to && from<=n && to<=n ){
				for( count=1; count<from; count++ ){
					temp = temp->link;
				}
				for( count=from; count<=to; count++ ){
					printw("  %-14s|  %d\n", temp->userName, temp->userScore);
					temp = temp->link;
				}
			}
			else printw("\nsearch failure: no rank in the list\n");
			break;
		case '2':
			printw("input the name: ");
			echo();
			scanw("%s", name1);
			noecho();
			printw("      name      |    score    \n");
			printw("------------------------------\n");
			count = 0;
			for( del=0; del<n; del++ ){
					if( strcmp(temp->userName, name1)==0 ){
					printw("%  -14s|  %d\n", temp->userName, temp->userScore);
					count = 1;
				}
				temp = temp->link;
			}
			if ( count==0 ){
				printw("\nsearch failure: no name in the list\n");
			}
			break;
		case '3':
			printw("input the rank: ");
			echo();
			scanw("%d", &del);
			noecho();
			if( !(del>=1 && del<=n)){
				printw("\nsearch failure: the rank not in the list\n");
			}
			else{
				if( del==1 ){ 
					head = head->link;
				}
				else{
					for( count=2; count<del; count++ ){
						temp = temp->link;
					}
					if( del==n ){
						temp->link = NULL;
					}
					else{
						temp->link = temp->link->link;
					}
				}
				n-=1;
				printw("\nresult: the rank deleted\n");
			}
			break;
		default: break;
	}
	getch();
}

void writeRankFile(){
	rPointer temp = head;
	FILE* fp = fopen("rank.txt", "w");
	fprintf(fp, "%d\n", n);
	while(temp){
		fprintf(fp, "%s %d\n", temp->userName, temp->userScore);
		temp = temp->link;
	}
	free(temp);
	head = NULL;
	fclose(fp);
}

void newRank(int score){
	rPointer temp = NULL, temp1 = NULL, temp2 = NULL;
	temp = (rPointer)malloc(sizeof(rP));

	char nameTemp[NAMELEN];
	clear();
	printw("your name: ");
	echo();
	scanw("%s", nameTemp);
	noecho();
	
	temp1 = head;
	temp->link = NULL;
	temp->userScore = score;
	strcpy(temp->userName, nameTemp);
	if(temp1==NULL){
		head = temp;
		n = 1;
	}
	if( score>(temp1->userScore) ){
		temp->link = temp1;
		head = temp;
		n += 1;
	}
	else{
		while(1){
		if( (temp1->link)==NULL ){
			temp1->link = temp;
			break;
		}
		if( score>(temp1->userScore) ){
			temp->link = temp1;
			temp2->link = temp;
			break;
		}
		temp2 = temp1;
		temp1 = temp1->link;
		}
	n += 1;
	}
}

void constructRecTree(RecNode *root){
	int i, h;
	RecNode **c = root->c;
	for( i=0; i<CHILDREN_MAX; ++i){
		c[i] = (RecNode *)malloc(sizeof(RecNode));
		c[i]->lv = root->lv+1;
		c[i]->f = (char (*)[WIDTH])malloc(sizeof(char)*HEIGHT*WIDTH);
		if( c[i]->lv < VISIBLE_BLOCKS ){
			constructRecTree(c[i]);
		}
	}
}

void DrawRecommend(){
	char tile = 'R';
	if( CheckToMove(field, nextBlock[0], recommendR, recommendY, recommendX)==1 ){
		DrawBlock(recommendY, recommendX, nextBlock[0], recommendR, tile);
	}
}

int evalState(int lv, char f[HEIGHT][WIDTH], int r, int y, int x){
	return AddBlockToField(f, nextBlock[lv], r, y, x) + DeleteLine(f);
}

int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수
	int r, x, y, rBoundary, lBoundary;
	int h, w;
	int eval;
	int solR, solY, solX;
	int recommended = 0;
	int i = 0;
	int lv = root->lv+1;
	RecNode **c = root->c;

	for( r=0; r<NUM_OF_ROTATE; ++r){
		lBoundary = 3;
		for( h=0; h<BLOCK_HEIGHT; ++h ){
			for( w=0; w<BLOCK_WIDTH; ++w ){
				if( block[nextBlock[lv]][r][h][w] ){
					break;
				}
			}
			if( w<lBoundary ){
				lBoundary = w;
			}
		}
		lBoundary = 0-lBoundary;

		rBoundary = 0;
		for( h=0; h<BLOCK_HEIGHT; ++h ){
			for( w=BLOCK_WIDTH-1; w>=0; --w ){
				if( block[nextBlock[lv]][r][h][w] ){
					break;
				}
			}
			if( w>rBoundary ){
				rBoundary = w;
			}
		}
		rBoundary = WIDTH-1-rBoundary;

		for( x=lBoundary; x<=rBoundary; ++x, ++i ){
			 for( h=0; h<HEIGHT; ++h ){
				 for( w=0; w<WIDTH; ++w ){
					 c[i]->f[h][w] = root->f[h][w];
				 }
			 }
			 y = 0;
			 if( CheckToMove(c[i]->f, nextBlock[lv], r, y, x )){
				 while(CheckToMove(c[i]->f, nextBlock[lv], r, ++y, x));
				 --y;
			 }
			 else{
				 continue;
			 }

			 c[i]->score = root->score+evalState(lv, c[i]->f, r, y, x);
			 if( lv<VISIBLE_BLOCKS-1 ){
				 eval = recommend(c[i]);
			 }
			 else{
				 eval = c[i]->score;
			 }
			 if( max<eval ){
				 recommended = 1;
				 max = eval;
				 solR = r;
				 solY = y;
				 solX = x;
			 }
		}
	}
	if( lv==0 && recommended ){
		recommendR = solR;
		recommendY = solY;
		recommendX = solX;
	}
	return max;
}

void recommendedPlay() {
	int command, i, j;
	clear();
	act.sa_handler = recBlockDown;
	sigaction(SIGALRM, &act, &oact);

	for (j = 0; j<HEIGHT; j++)
		for (i = 0; i<WIDTH; i++)
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
	DrawBlock(blockY, blockX, nextBlock[0], blockRotate, ' ');
	recommend(recRoot);
	blockRotate = recommendR;
	blockY = recommendY;
	blockX = recommendX;
	//recommendFree();
	DrawNextBlock(nextBlock);
	PrintScore(score);

	do {
		if (timed_out == 0) {
			alarm(1);
			timed_out = 1;
		}

		command = recGetCommand();
		if (ProcessCommand(command) == QUIT) {
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

void modified_recommend(ModNode* root, int sp) {
	static int max;
	int max2;
	int max3;
	int i, j, k, l = 0;

	if ( modRoot == NULL ) {
		max = 0;
		modRoot = (ModNode*)malloc(sizeof(ModNode));
		modRoot->recScore = 0;
		for ( i=0; i<HEIGHT; i++ ) {
			for ( j=0; j<WIDTH; j++ ) {
				modRoot->recField[i][j] = field[i][j];
			}
		}
		recommendX = 0;
		recommendY = 0;
		recommendR = 0;
		root = modRoot;
	}
	if ( sp == VISIBLE_BLOCKS ) {
		if  (recommendX == 0 && recommendY == 0 && recommendR == 0 ) {
			if (CheckToMove(field, nextBlock[0], root->parent->parent->recBlockRotate, root->parent->parent->recBlockY, root->parent->parent->recBlockX)) {
				recommendX = root->parent->parent->recBlockX;
				recommendY = root->parent->parent->recBlockY;
				recommendR = root->parent->parent->recBlockRotate;
				max = root->recScore;
			}
		}
		if ( (root->recScore > max) || (root->recScore == max && root->parent->parent->recBlockY < recommendY) ) {
			if ( CheckToMove(field, nextBlock[0], root->parent->parent->recBlockRotate, root->parent->parent->recBlockY, root->parent->parent->recBlockX) ) {
				recommendX = root->parent->parent->recBlockX;
				recommendY = root->parent->parent->recBlockY;
				recommendR = root->parent->parent->recBlockRotate;
				max = root->recScore;
			}
		}
		return;
	}
	else {
		for ( l=0; l<100; l++ ) {
			root->child[l] = (ModNode*)malloc(sizeof(ModNode));
			root->child[l]->parent = root;
		}
		for ( l=0; l<100; l++ ) {
			for ( i = 0; i<HEIGHT; i++ ) {
				for ( j = 0; j<WIDTH; j++ ) {
					root->child[l]->recField[i][j] = root->recField[i][j];
				}
			}
		}
		l = 0;
		max3 = 0;
		max2 = 0;
		while ( l<100 ) {
			for ( i=0; i<4; i++ ) {
				for ( j = -2; j<13; j++ ) {
					root->child[l]->recBlockRotate = i;
					root->child[l]->recBlockX = j;
					k = blockY;
					if ( CheckToMove(root->child[l]->recField, nextBlock[sp + 1], i, k, j) == 0 ) {
						l++;
						continue;
					}
					while (CheckToMove(root->child[l]->recField, nextBlock[sp + 1], i, k + 1, j)) {
						k++;
					}
					root->child[l]->recBlockY = k;
					root->child[l]->recScore = root->recScore;
					root->child[l]->recScore += AddBlockToField(root->child[l]->recField, nextBlock[sp + 1], root->child[l]->recBlockRotate, root->child[l]->recBlockY, root->child[l]->recBlockX);
					root->child[l]->recScore += 10 * DeleteLine(root->child[l]->recField);
					if (max2<root->child[l]->recScore) {
						max2 = root->child[l]->recScore;
						max3 = l;
					}
					l++;
				}
			}
		}
		step[sp] = max3;
		modified_recommend(root->child[max3], sp + 1);
	}
}


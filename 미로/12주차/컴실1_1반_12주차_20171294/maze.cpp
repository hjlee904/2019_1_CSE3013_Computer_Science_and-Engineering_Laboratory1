#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>

typedef struct Cell {
	int hor;
	int ver;
};
Cell **maze = NULL;

void maze_print(); // vWall 정보를 가지고, 한 줄을 그린다. ( 세로 벽 )
void makeNewHorizontal(int); // 이전 세로벽의 정보를 가지고, 가로 벽을 만든다.
void makeNewVertical(int); // 첫 줄 이후, 세로벽을 랜덤하게 만든다. 

int WIDTH;
int HIGHT;
int* hWall;
int* vWall;
int* cell;
int id = 0;
FILE* file;

int main() {

	int i, j, k, u, Min, rh, rw, rr;
	int prevWall = -1;
	int prevId;

	file = fopen("mazeGen.maz", "wt");

	printf("Width : ");
	scanf("%d", &WIDTH);
	printf("Hight : ");
	scanf("%d", &HIGHT);

	hWall = (int*)malloc(sizeof(int)*WIDTH);        // 가로 벽. 빈칸의 갯수와 같다.
	vWall = (int*)malloc(sizeof(int)*(WIDTH - 1));	// 세로 벽. 빈칸의 갯수보다 1개 작다.
	cell = (int*)malloc(sizeof(int)*WIDTH);			// 빈칸들. ( id를 저장해서 집합을 구분하는 데에 쓰임 )
	memset(cell, 0, sizeof(cell));					// 초기화.

	srand(time(NULL)); // 랜덤한 값을 내기 위한 seed 설정.

		/*
			프로그램 순서.
			1. 첫 줄을 그린다.
			2. 2 ~ (Height - 1)줄까지 그린다.
			3. 마지막 줄을 그린다.
		*/

		/*
			첫 줄.
		*/
	// 가장 상단은 다 막혔으므로, 가로벽을 만들어준다.
	for (i = 0; i < WIDTH; i++) {
		fprintf(file, "+-");
		printf("+-");
	}
	fprintf(file, "+\n");
	printf("+\n");

	maze = (Cell**)malloc(sizeof(Cell*)*HIGHT);
	for (int i = 0; i < HIGHT; i++) {
		maze[i] = (Cell*)malloc(sizeof(Cell)*WIDTH);
		for (int j = 0; j < WIDTH; j++) {
			maze[i][j].ver = 0;
			maze[i][j].hor = 0;
		}
	}


	// 뚜껑을 만들었다면, 세로 벽을 쳐서 첫 번째로 집합을 나눠준다.
	for (i = 0; i < WIDTH - 1; i++) {
		vWall[i] = rand() % 2; // 1이 나오면 벽을 치는 거고, 0이 나오면 벽을 안치는 거다.
		if (vWall[i] == 1) { // 벽을 치는 경우.
			maze[0][i].ver = 1;
			for (j = (prevWall + 1); j <= i; j++) {
				// prevWall는 이전 세로 벽의 위치라고 보면 된다. 
				// 초기화가 -1로 되어 있었으므로, 그냥 맨 왼쪽 벽이었다고 생각하자.
				// 그 벽 다음부터 i번째 벽까지는 벽이 없는 상태.
				/*
					 |     .      .      |      |      |
						-1    0      1      2      3    맨오른쪽
					빈 칸이 5개라면
					벽은 4개, 0 ~ 3번.
					만약 vWall[2]에서 처음 1이 나왔다면 ( rand%2 )
					prevWall = -1 이었으므로, for문은
					0 ~ 2까지 돌게 될 것이다.
					빈칸을 기준으로 본다면 2번 직전 까지가 되고, 해당 빈칸들은 모두 같은 id를 가져야 함.
				*/
				cell[j] = id;
			}
			prevWall = i; // 만약 위와같은 경우였다면, i=2, prevWall = 2가 된다.
			id++; // 같은 집합은 모두 같은 id를 가졌으므로, id를 하나 올려줌.
		}
	}
	for (i = prevWall + 1; i < WIDTH; i++) {
		/*
			제일 마지막 벽 이후로 vWall[i]가 1이 나온 적이 없다면,
			가장 오른쪽 벽에 도달한 상태이나,
			id가 할당이 안되어있음.
		*/
		cell[i] = id;
	}
	id++;

	for (k = 1; k < HIGHT - 1; k++) {
		makeNewHorizontal(k);
		makeNewVertical(k);
	}
	makeNewHorizontal(k);

	for (i = 0; i < WIDTH - 1; i++) {
		/*
			+-+-+-+-+-+-+-+-+-+-+
			|0 0 0|1 1 1 1 1|2|3|
			+-+-+ +-+-+-+ +-+ + +
			 4 5   6 7 8   9
			였다면, 우리가 원하는 결과는

			+-+-+-+-+-+-+-+-+-+-+
			|0 0 0|1 1 1 1 1|2|3|
			+-+-+ +-+-+-+ +-+ + +  가 된다. 열어야만 연결되므로,
			|0 0 0 0 0 0 0 0 0 0|
			+-+-+-+-+-+-+-+-+-+-+ // 이건 나중에 따로 해주면 됨.
		*/

		/*
			두 가지 경우로 나뉨.

			같을 때, 다를 때.

			* 만약 두 cell이 다르다면, 두 cell 모두 바로 위의 가로벽이 막혀있는 상태

			따라서 세로벽을 열어주지 않으면 고립되게 된다.



			* 만약 두 cell이 같다면 이미 이전 미로에서 연결이 되어있다는 뜻이 된다.

			따라서 닫아야만 cycle이 생기지 않는다.
		*/

		if (cell[i] != cell[i + 1]) {
			vWall[i] = 0;
			prevId = cell[i + 1];
			cell[i + 1] = cell[i];
			for (j = 0; j < WIDTH; j++) { // for문을 돌면서 해당 id에 속하는 놈들을 모두 cell[i]로.
				if (cell[j] == prevId) cell[j] = cell[i];
			}
		}
		else {
			vWall[i] = 1;
			maze[HIGHT - 1][i].ver = 1;
		}
	}
	
	if (HIGHT <= WIDTH) {
		Min = HIGHT;
	}
	else Min = WIDTH;
	Min = Min / 2;

	u = 0;
	while (1) {
		rh = rand() % HIGHT;
		rw = rand() % WIDTH;
		rr = rand() % 2;
		if (rr == 0) {
			if (maze[rh][rw].hor == 1) {
				maze[rh][rw].hor = 0;
				u++;
			}
		}
		if (rr == 1) {
			if (maze[rh][rw].ver== 1) {
				maze[rh][rw].ver = 0;
				u++;
			}
		}
		if (u == Min) {
			break;
		}
	}
	
	maze_print();

	//마지막 가로벽. 그냥 벽만 존재.
	for (i = 0; i < WIDTH; i++) {
		fprintf(file, "+-");
		printf("+-");
	}
	fprintf(file, "+\n");
	printf("+\n");

	return 0;
}

void maze_print() {

	int i, j;

	for (i = 0; i < HIGHT-1; i++) {
		fprintf(file, "|");
		printf("|");
		fprintf(file, " ");
		printf(" ");
		for (j = 0; j < WIDTH - 1; j++) {
			if (maze[i][j].ver == 1) {
				fprintf(file, "|");
				printf("|");
			}
			else {
				fprintf(file, " ");
				printf(" ");
			}
			fprintf(file, " ");
			printf(" ");
		}
		fprintf(file, "|");
		printf("|");
		fprintf(file, "\n");
		printf("\n");

		fprintf(file, "+");
		printf("+");
		for (j = 0; j < WIDTH; j++) {
			if (maze[i][j].hor == 1) {
				fprintf(file, "-+");
				printf("-+");
			}
			else {
				fprintf(file, " +");
				printf(" +");
			}
		}
		fprintf(file, "\n");
		printf("\n");
	}
	fprintf(file, "|");
	printf("|");
	fprintf(file, " ");
	printf(" ");
	for (j = 0; j < WIDTH - 1; j++) {
		if (maze[HIGHT-1][j].ver == 1) {
			fprintf(file, "|");
			printf("|");
		}
		else {
			fprintf(file, " ");
			printf(" ");
		}
		fprintf(file, " ");
		printf(" ");
	}
	fprintf(file, "|");
	printf("|");
	fprintf(file, "\n");
	printf("\n");
}

void makeNewHorizontal(int k) {
	/*
		새로운 가로 벽을 만드는 함수.
		가로 벽을 만듦의 여부는 당연히 id에 영향을 준다.
		가로벽이 있다면, 윗칸과 다른 id를 받게 될 것이고,
		가로벽이 없다면, 윗칸과 같은 id를 받게 된다.
	*/

	int prevId; // 윗 칸의 id.
	int i;
	int alreadyDone = 0; // id 내에서, 이미 뚫려있는 지의 여부, 

	prevId = cell[0]; // 윗 칸의 id를 받아옴.
					  /*
						  숫자는 cell을 의미. 맨 마지막 줄의 숫자만 cell에 저장되어있다.
						  +-+-+-+-+
						  |0 0 0|1|
					  */
	for (i = 0; i < WIDTH; i++) {
		/*
			WIDTH만큼 돌면서 가로벽 생성 여부를 결정.
		*/
		hWall[i] = rand() % 2;
		// 1이면 가로벽 만들고, 0이면 안만든다.

		/*
			+-+-+-+-+
			|0 0 0|1| 에서 만약 hWall[0] = 1이면

			+-+-+-+-+
			|0 0 0|1| 가 되고, 0이면,
			+-+

			+-+-+-+-+
			|0 0 0|1| 가 되는 상황임.
			+ +
		*/
		maze[k - 1][i].hor = 1;

		if (hWall[i] == 0) { // 가로벽을 안 만들게 된 경우,
							 /*
								 +-+-+-+-+
								 |0 0 0|1| 인 상황.
								 + +
							 */
			alreadyDone = 1; // 이미 뚫려있다고 체크해준다.
							 // 용도는 아래 설명.
			maze[k - 1][i].hor = 0;
		}

		if (i < WIDTH - 1) {
			// 마지막 가로벽이 아니면,
			/*
				+-+-+-+-+
				|0 0 0|1| 였다면 i가 3보다 작을 때, 0, 1, 2 의 세 경우를 의미함.
				 ^ ^ ^     ^ 위치에 올 벽들.
			*/

			if (prevId != cell[i + 1]) {
				// 윗쪽 오른쪽 칸의 id를 본다. 
				/*
					prevId는 cell[0]으로 초기화 되어있다. 즉, 0일 것임.
					+-+-+-+-+
			cell    |0 0 0|1|

					따라서 이 if문에 걸리려면
					+-+-+-+-+
					|0 0 0|1|  인 경우에만 가능.
					 	   ^
				*/
				if (alreadyDone == 0) {
					// alreadyDone이 0이라면
					// 아직 벽이 뚫린적이 없단 얘기가 되므로,
					/*
						+-+-+-+-+
						|0 0 0|1|  의 꼴일 것임.
						+-+-+-+
						그런데 이러면 위 세 칸이 고립된다.
						따라서 벽을 강제로라도 뚫어야됨.
					*/

					hWall[i] = 0;
					maze[k - 1][i].hor = 0;
					/*
						+-+-+-+-+
						|0 0 0|1| 의 모양이 될 것이다.
						+-+-+ +
					*/
				}

				else {
					//벽이 뚫린 적이 있으므로, 초기화만 해준다.
					/*
						+-+-+-+-+
						|0 0 0|1|  의 꼴임. 왜냐면, hWall[i] = 0이면 alreadyDone 은 1임.
						+ +-+-+
					*/

					alreadyDone = 0;
					/*
						다음 작업을 위해 설정을 하는 것.
						+-+-+-+-+-+-+-+-+-+-+-+-+
						|0 0 0|1 1 1 1 1 1 1|2|3| 의 꼴이었다면, 1들에 대해 alreadyDone은 0이어야 하니까.
						+ +-+-+
					*/
				}

				/*
					if 에 걸렸다는 말은 다음 cell부터는 다른 id니까, id를 바꿔줘야 함.
					+-+-+-+-+-+-+-+-+-+-+-+-+
					|0 0 0|1 1 1 1 1 1 1|2|3|
					+ +-+-+
				*/
				prevId = cell[i + 1]; // prevId를 바꿔준다.
			}
		}

		if ((i == WIDTH - 1) && (alreadyDone == 0)) {
			// 마지막 칸, 아직 뚫린적이 없으면 뚫어줌.
			/*
				+-+-+-+-+
				|0 0 0|1| 일 수는 없으니,
				+ +-+-+-+
				+-+-+-+-+
				|0 0 0|1| 로 만들어 주는 것.
				+ +-+-+ +
			*/
			hWall[i] = 0;
			maze[k - 1][i].hor = 0;
		}

		if (hWall[i]) {
			// 막힌 벽이라면, id를 새로 할당.
			/*
				+-+-+-+-+-+-+-+-+-+-+-+-+
				|0 0 0|1 1 1 1 1 1 1|2|3| 에서

				+-+-+-+-+-+-+-+-+-+-+-+-+
				|0 0 0|1 1 1 1 1 1 1|2|3| 까지 왔다면,
				+-+-+ +

				각 시행마다,

				+-+-+-+-+-+-+-+-+-+-+-+-+
				|0 0 0|1 1 1 1 1 1 1|2|3|
				+-+
				 4

				+-+-+-+-+-+-+-+-+-+-+-+-+
				|0 0 0|1 1 1 1 1 1 1|2|3|
				+-+-+
				 4 5

				+-+-+-+-+-+-+-+-+-+-+-+-+
				|0 0 0|1 1 1 1 1 1 1|2|3| 이 된 상황이라고 보면 된다.
				+-+-+ +
				 4 5 0

				0은 아무 일도 일어나지 않아서 0.

				이 때의 cell은 4 5 0 1 1 1 1 1 1 1 2 3 이 저장되어 있을 것이다.

			*/
			cell[i] = id;
			id++;
		}
	}
}

void makeNewVertical(int k) {
	/*
		새로운 세로 벽을 만드는 함수.
	*/

	/*
		이 함수 직전 상태가
		+-+-+-+-+-+-+-+-+-+-+
		|0 0 0|1 1 1 1 1|2|3|
		+-+-+ +-+-+-+ +-+ + + 라고 하자.
		 4 5   6 7 8   9

		이 떄 Cell은 4 5 0 6 7 8 1 9 2 3 일 것임.
		아랫줄에 써있는 것은 막힌벽이어서 cell이 update 되었을 때의 상태.

	*/
	int i, j;
	int Join_flag = 0;
	int prev_Id;

	for (i = 0; i < WIDTH - 1; i++) {
		if (cell[i] != cell[i + 1]) {
			// cell이 다르면, 벽을 칠지 말지 정한다.
			/*
				+-+-+-+-+-+-+-+-+-+-+
				|0 0 0|1 1 1 1 1|2|3|
				+-+-+ +-+-+-+ +-+ + + 이런 상황이면 4!=5 이므로 바로 들어온다.
				 4 5   6 7 8   9

			*/
			Join_flag = rand() % 2;

			if (Join_flag) { // 벽을 안치기로 정함.

							 /*
								 +-+-+-+-+-+-+-+-+-+-+
								 |0 0 0|1 1 1 1 1|2|3|
								 +-+-+ +-+-+-+ +-+ + +
								  4 5   6 7 8   9
							 */
				prev_Id = cell[i + 1]; // 따로 저장.
				cell[i + 1] = cell[i];

				/*
					+-+-+-+-+-+-+-+-+-+-+
					|0 0 0|1 1 1 1 1|2|3|
					+-+-+ +-+-+-+ +-+ + + 가 되고,
					|4 4   6 7 8   9
				*/

				// 여기서  cell이 5인 모든 놈들은 죄다 4가 된다.
				for (j = 0; j < WIDTH; j++)
					if (cell[j] == prev_Id) cell[j] = cell[i];
				vWall[i] = 0;
			}
			else {
				/*
					+-+-+-+-+-+-+-+-+-+-+
					|0 0 0|1 1 1 1 1|2|3|
					+-+-+ +-+-+-+ +-+ + + 이렇게 하겠다는 의미가 된다.
					|4|5   6 7 8   9
				*/
				vWall[i] = 1;
				maze[k][i].ver = 1;
			}
		}
		else {
			//cell이 같으면 무조건 벽을 쳐준다.

			/*
				+-+-+-+-+-+-+-+-+-+-+
				|0 0 0|1 1 1 1 1|2|3|
				+-+-+ + + + + +-+ + +    초기에 이런 상태였다고 가정하자.
			 	 4 5           6

			이 때,

				+-+-+-+-+-+-+-+-+-+-+
				|0 0 0|1 1 1 1 1|2|3|
				+-+-+ + + + + +-+ + + 까지 왔다고 하자. 그러면, 이후로는 1과 1을 비교하므로, 벽을 무조건 쳐야 한다.
				|4|5| |        6
	
				+-+-+-+-+-+-+-+-+-+-+
				|0 0 0|1 1 1 1 1|2|3|
				+-+-+ + + + + +-+ + +  이런 식으로 해줘야함. 안그러면 cycle 발생.
				|4|5| | | | |  6

			*/

			vWall[i] = 1;
			maze[k][i].ver = 1;
		}
	}
}

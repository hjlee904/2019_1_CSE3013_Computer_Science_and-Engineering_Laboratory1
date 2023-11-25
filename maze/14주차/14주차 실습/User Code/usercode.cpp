#include <stdlib.h>
#include "..\ExternDoc.h"
#include "..\UI\Zoom.h"
#include "..\UI\MsgView.h"
#include "..\Graphics\DrawFunc.h"
#include "..\Example\Example.h"

#define ERROR_NUMBER -1

//function prototype

static void drawDirect(CDC *pDC);
static void drawBuffered();

//Start of user code
#include <float.h>


typedef struct vertex VTX;
typedef struct vtx_linked_list VLL;

struct vertex{
	int row;
	int col;
	bool right;
	bool down;
};

struct vtx_linked_list{
	VTX *node;
	VLL *next;
	bool connected; //새로 추가된 구조체 멤버 변수. DFS 탐색시 탐색했던 경로 저장을 위해 사용하는 변수.
};

int rows, cols;
bool fileflag = false;
bool DFSflag = false;
short drawmode = 0; //0 : just draw maze, 1: draw DFS path, 2: draw BFS path

VTX **maze = NULL;
VTX *V_ptr = NULL;
VLL *VLL_pool = NULL; //VLL storage pool

VLL *VC = NULL; //Vertex linked list pointer for using Queue
VLL *last_ptr = NULL; //last pointer

VLL *VDFS = NULL; //solution root by DFS search
VLL *D_visit_root = NULL; //visited root by DFS search
int VLL_cnt = 0; // number of VLL used
int Total_Used_Memory = 0;

//VLL management routine
void pushQ_VLL (VTX* node);
void pushS_VLL (VTX* node, VLL** vll);
VTX* popQ_VLL( VLL **Q);
VLL *Alloc_VLL(void);
void Free_VLL( VLL *vll);
void Free_VLL_pool( void);


void Error_Exit(char *s){//For debugging only

	showMessage(s);
	exit(-1); //you may break here for possible error
}
/*****************************************************************
* function	: bool readFile(const char* filename)
* argument	: cons char* filename - filename to be opened
* return	: true if success, otherwise flase
* remark	: After read data file, phisycal view must be set;
*			  otherwise drawing will not be executed correctly.
*			  The window will be invalidated after readFile()
*			  returns true.
******************************************************************/

bool readFile(const char* filename){

	//start of the user code
	
	
	char tmpChar;
	FILE *fp;
	bool flag, nlineflag;
	VTX *tmprow;
	VTX *nxtrow;
	int i;

	//initate variables;
	fileflag = false;
	DFSflag = false;
	drawmode = 0;

	fp = fopen( filename, "r");
	if(fp ==NULL){
		flag = false;
		return flag;
	}

	flag = true;
	
	rows = 0;
	cols = 0;

	fscanf( fp, "%c", &tmpChar); // ignore first "+"
	while(1){//read fist line to count number of rows
	fscanf( fp,"%c", &tmpChar);
	if( tmpChar != 45){
		if(tmpChar != 10)
		Error_Exit("input is wrong!");
		else
			break;
	}
	fscanf( fp, "%c", &tmpChar);
	if( tmpChar != 43)
		Error_Exit("input is wrong!");
	else
		cols++;
	}

	tmprow = NULL;
	nxtrow = NULL;
	tmprow = (VTX *)malloc(sizeof(VTX) * cols);
	pushQ_VLL(tmprow);

	while(1){//read 1 text line and save data in data structure
		nlineflag = false;
		fscanf( fp, "%c", &tmpChar);//ignore 1st "|"

		for(i=0;i<cols;i++){
			tmprow[i].col = i;
			tmprow[i].row = rows;
			fscanf( fp, "%c", &tmpChar);
			if(tmpChar != 32)
				Error_Exit("Input Error!");
			fscanf( fp, "%c", &tmpChar);
			if(tmpChar == 32){
				tmprow[i].right = true;
			}
			else if(tmpChar != 124)
				Error_Exit("Input Error!");
			else{
				tmprow[i].right = false;
			}
			
		}
		tmprow[cols-1].right = false;

		fscanf( fp, "%c", &tmpChar); //ignore last enter(ASCII 10)
		fscanf( fp, "%c", &tmpChar); //ignore 1st "+"

		for(i=0;i<cols;i++){
			fscanf( fp, "%c", &tmpChar);
			if(tmpChar == 32){
				if(!nlineflag){
					nlineflag = true;
					nxtrow = (VTX *)malloc(sizeof(VTX)*cols);
					pushQ_VLL(nxtrow);
					rows++;
				}
				tmprow[i].down = true;
			}
			else if(tmpChar != 45)
				Error_Exit("Input Error!");
			else{
				tmprow[i].down = false;		
			}
			fscanf( fp, "%c",&tmpChar);
		}
		if(!nlineflag){
			for(i=0;i<cols;i++)
				tmprow[i].down = false;
			break;
		}
		tmprow = nxtrow;
		nxtrow = NULL;
		fscanf( fp, "%c",&tmpChar); //ignore stringend(ASCII 10)

	}
	fileflag = true;
	rows++;
	tmprow = NULL;
	nxtrow = NULL;
	last_ptr = NULL;
	fclose(fp);

	maze = (VTX **)malloc(sizeof(VTX *)*rows);
	for(i=0;i<rows;i++){
		maze[i] = popQ_VLL(&VC);
	}
	setWindow(0,0,4*cols+5,4*rows+5,0);

	return flag;
}

/******************************************************************
* function	: bool FreeMemory()
*
* remark	: Save user data to a file
*******************************************************************/
void freeMemory(){

	int i;
	if(maze != NULL)
	for(i=0;i<rows;i++)
		free(maze[i]);
	free(maze);

	if(fileflag){
		while(1){
			if(VC==NULL)
				break;
			popQ_VLL(&VC);
		}
	}
	if(DFSflag){
		while(1){
			if(VDFS==NULL)
				break;
			popQ_VLL(&VDFS);
		}
		while(1){
			if(D_visit_root==NULL)
				break;
			popQ_VLL(&D_visit_root);
		}
	}

	if(VLL_pool != NULL)
	Free_VLL_pool();

	if( VLL_cnt != 0)
		Error_Exit("Storage does not cleared!"); //revival this sentence when all draw

}

/**************************************************************
* function	: bool writeFile(const char* filename)
*
* argument	: const char* filename - filename to be written
* return	: true if success, otherwise false
* remark	: Save user data to a file
****************************************************************/
bool writeFile(const char* filename){
	//start of the user code
	bool flag;
	flag = 0;

	return flag;
	//end of usercode
}

/************************************************************************
* fucntion	: void drawMain(CDC* pDC)
*
* argument	: CDC* pDC - device context object pointer
* remark	: Main drawing function. Called by CMFC_MainView::OnDraw()
*************************************************************************/
void drawMain(CDC *pDC){
	//if direct drawing is defined
#if defined(GRAPHICS_DIRECT)
	drawDirect(pDC);
	//if buffered drawing is defined
#elif defined(GRAPHICS_BUFFERED)
	drawBuffered();
#endif
}

/************************************************************************
* function	: static void drawDirect(CDC *pDC
*
* argument	: CDC* pDC - device context object pointer
* remark	: Direct drawing routines here.
*************************************************************************/
static void drawDirect(CDC *pDC){
	//begin of user code
	//Nothing to write currently.
	//end of user code
}

/***********************************************************************
* function	: static void drawBuffered()
*
* argument	: CDC* pDC -0 device object pointer
* remark	: Buffered drawing routines here.
************************************************************************/
static void drawBuffered(){
	//start of the user code
	VLL *ptr;
	VTX *V;
	double lineWidth = 0.01;
	int i,j;
	int x,y;
	//draw upper border
	DrawSolidBox_I(0,4*rows,1,4*rows+1,lineWidth,RGB(0,0,255),RGB(0,0,255));
	for(i=0;i<cols;i++){
		DrawSolidBox_I(4*i+1,4*rows,4*i+4,4*rows+1,lineWidth,RGB(0,0,255),RGB(0,0,255));
		DrawSolidBox_I(4*i+4,4*rows,4*i+5,4*rows+1,lineWidth,RGB(0,0,255),RGB(0,0,255));
	}

	for(i=0;i<rows;i++){
		DrawSolidBox_I(0,4*(rows-i)-3,1,4*(rows-i),lineWidth,RGB(0,0,255),RGB(0,0,255));
		DrawSolidBox_I(0,4*(rows-i-1),1,4*(rows-i)-3,lineWidth,RGB(0,0,255),RGB(0,0,255));

		for(j=0;j<cols;j++){
			if(maze[i][j].right ==false)
				DrawSolidBox_I(4*(j+1),4*(rows-i)-3,4*j+5,4*(rows-i),lineWidth,RGB(0,0,255),RGB(0,0,255));
			if(maze[i][j].down == false)
				DrawSolidBox_I(4*j+1,4*(rows-i-1),4*(j+1),4*(rows-i)-3,lineWidth,RGB(0,0,255),RGB(0,0,255));
			DrawSolidBox_I(4*(j+1),4*(rows-i-1),4*j+5,4*(rows-i)-3,lineWidth,RGB(0,0,255),RGB(0,0,255));
		}
	}
	

	if(drawmode==1){
		if(D_visit_root!=NULL){
			ptr = D_visit_root;
			V=ptr->node;
			i=V->col;
			j=rows-1-V->row;
			DrawSolidBox_I(4*i+2, 4*j+2, 4*i+3, 4*j+3, lineWidth, RGB(150, 150, 150), RGB(150, 150, 150));
			while(ptr->next!=NULL){
				ptr=ptr->next;
				V=ptr->node;
				y=rows-V->row-1;
				x=V->col;
				if(ptr->connected){
					if(i==x){
						if(y<j){
							j=y;
						}
						DrawSolidBox_I(4*i+2, 4*j+3, 4*i+3, 4*j+6, lineWidth, RGB(150, 150, 150), RGB(150, 150, 150));
					}
					else{
						if(x<i){
							i=x;
						}
						DrawSolidBox_I(4*i+3, 4*j+2, 4*i+6, 4*j+3, lineWidth, RGB(150, 150, 150), RGB(150, 150, 150));
					}
				}
				i=x;
				j=y;
				DrawSolidBox_I(4*i+2, 4*j+2, 4*i+3, 4*j+3, lineWidth, RGB(150, 150, 150), RGB(150, 150, 150));
			}
		}
		ptr=VDFS;
		V=ptr->node;
		i=V->col;
		j=rows-1-V->row;
		DrawSolidBox_I(4*i+2, 4*j+2, 4*i+3, 4*j+3, lineWidth, RGB(10, 10, 10), RGB(10, 10, 10));
		while(ptr->next!=NULL){
			ptr=ptr->next;
			V=ptr->node;
			y=rows-V->row-1;
			x=V->col;
			if(i==x){
				if(y<j){
					j=y;
				}
				DrawSolidBox_I(4*i+2, 4*j+3, 4*i+3, 4*j+6, lineWidth, RGB(10, 10, 10), RGB(10, 10, 10));
			}
			else{
				if(x<i){
					i=x;
				}
				DrawSolidBox_I(4*i+3, 4*j+2, 4*i+6, 4*j+3, lineWidth, RGB(10, 10, 10), RGB(10, 10, 10));
			}
			i=x;
			j=y;
			DrawSolidBox_I(4*i+2, 4*j+2, 4*i+3, 4*j+3, lineWidth, RGB(10, 10, 10), RGB(10, 10, 10));
		}
	}
}

void DFS(void){
	//start of user code
	//Search By iterative ver DFS

	VTX *V;
	bool **visited;
	bool **noway;
	bool is_last_pop = false;
	int i;

	if(!DFSflag){ //이전 버튼 클릭으로 이미 DFS경로를 탐색했다면 재탐색하지 않고 바로 그린다.
		
		visited = (bool**)malloc(rows*2*sizeof(bool*));
		noway = visited + rows;
		visited[0] = (bool*)calloc(2*rows*cols, sizeof(bool));
		for(i=1; i<rows; i++){
			visited[i] = visited[i-1] + cols;
		}
		noway[0] = visited[rows-1] + cols;
		for(i=1; i<rows; i++){
			noway[i] = noway[i-1] + cols;
		}
		V = &maze[0][0];

		while(1){
			visited[V->row][V->col] = true;
			if((V->row==rows-1)&&(V->col==cols-1)){
				pushS_VLL(V, &VDFS);
				DFSflag = true;
				break;
			}
			if(!noway[V->row][V->col]){
				if((V->right)&&!visited[V->row][V->col+1]){
					if(is_last_pop){
						pushS_VLL(V, &D_visit_root);
						D_visit_root->connected = false;
					}
					pushS_VLL(V, &VDFS);
					V = &maze[V->row][V->col+1];
					is_last_pop = false;
					continue;
				}
				if((V->down)&&!visited[V->row+1][V->col]){
					if(is_last_pop){
						pushS_VLL(V, &D_visit_root);
						D_visit_root->connected = false;
					}
					pushS_VLL(V, &VDFS);
					V = &maze[V->row+1][V->col];
					is_last_pop = false;
					continue;
				}
				if(V->col>0){
					if(maze[V->row][V->col-1].right&&!visited[V->row][V->col-1]){
						if(is_last_pop){
							pushS_VLL(V, &D_visit_root);
							D_visit_root->connected = false;
						}
						pushS_VLL(V, &VDFS);
						V = &maze[V->row][V->col-1];
						is_last_pop = false;
						continue;
					}
				}
				if(V->row>0){
					if(maze[V->row-1][V->col].down&&!visited[V->row-1][V->col]){
						if(is_last_pop){
							pushS_VLL(V, &D_visit_root);
							D_visit_root->connected = false;
						}
						pushS_VLL(V, &VDFS);
						V = &maze[V->row-1][V->col];
						is_last_pop = false;
						continue;
					}
				}
				noway[V->row][V->col] = true;
				pushS_VLL(V, &D_visit_root);
				V = popQ_VLL(&VDFS);
				is_last_pop = true;
			}
			else{
				pushS_VLL(V, &D_visit_root);
				V = popQ_VLL(&VDFS);
				is_last_pop = true;
			}
		}
		setWindow(0, 0, 4*cols+5, 4*rows+5, 0);
		for(i=1; i<rows; i++){
			visited[i] = NULL;
		}
		for(i=1; i<rows; i++){
			noway[i] = NULL;
		}
		free(visited[0]);
		noway  = NULL;
		free(visited);

		DFSflag = true;
	}
	//end of user code
}
void BFS(void){
	//start of user code
	return;
}

void pushQ_VLL (VTX* node){
	VLL *L;

	if(last_ptr == NULL){
		VC = Alloc_VLL();
		last_ptr = VC;
		VC->node = node;
	}
	else{
	L = Alloc_VLL();
	L->node = node;
	last_ptr->next = L;
	last_ptr = L;
	}
	return;
}

void pushS_VLL (VTX* node, VLL** vll){
	VLL *L;
	if(*vll == NULL){
		*vll = Alloc_VLL();
		(*vll)->node = node;
		(*vll)->next = NULL;
	}
	else{
		L = Alloc_VLL();
		L->node = node;
		L->next = *vll;
		*vll = L;
	}
	return;
}

VLL *Alloc_VLL(void){
	VLL *ptr;

	if( VLL_pool ==NULL){
		ptr = (VLL *)malloc(sizeof(VLL));
		if(ptr == NULL)
			Error_Exit("No memory in Alloc_ptr_L");
		Total_Used_Memory += sizeof(VLL);
	}
	else {
		ptr = VLL_pool;
		VLL_pool = ptr->next;
	}
	ptr->next = NULL;
	ptr->connected = true;
	++VLL_cnt;
	return(ptr);
}

VTX* popQ_VLL( VLL **Q){
	VTX *i;
	VLL *tQ;

	i = (*Q)->node;
	tQ = *Q;
	if((*Q)->next == NULL){
		*Q = NULL;
		last_ptr = NULL;
	}
	else
		*Q = (*Q)->next;
	Free_VLL(tQ);
	return(i);
}

void Free_VLL( VLL *vll){
	vll->node = NULL;
	vll->next = VLL_pool;
	VLL_pool = vll;
	--VLL_cnt;
}

void Free_VLL_pool( void){

	VLL *p;
	last_ptr = NULL;
	p = VLL_pool;
	while(p!=NULL){
		free(VLL_pool ->node);
		VLL_pool = p->next;
		free(p);
		p = VLL_pool;
		Total_Used_Memory -= sizeof(VLL);
	}

	if(VLL_cnt != 0)//just for debugging
		Error_Exit("Non-zero VLL_cnt after deallocate VLL_pool.");
	VLL_pool = NULL;

}


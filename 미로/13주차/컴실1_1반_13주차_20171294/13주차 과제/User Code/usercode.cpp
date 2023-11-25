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
};

int rows, cols;
bool fileflag = false;
VTX **maze = NULL;
VLL *VLL_pool = NULL;

VLL *VC = NULL;
VLL *last_ptr = NULL;

int VLL_cnt = 0;
int Total_Used_Memory = 0;

void pushQ_VLL(VTX *node);
VTX *popQ_VLL(VLL **Q);
VLL *Alloc_VLL(void);
void Free_VLL(VLL *vll);
void Free_VLL_pool(void);

void Error_Exit(char *s){
	showMessage(s);
	exit(-1);
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
	
	char tmpChar;
	FILE *fp;
	bool flag, nlineflag;
	VTX *tmprow;
	VTX *nxtrow;
	int i, j;

	fileflag = false;
	fp = fopen(filename, "r");
	if(fp==NULL){
		flag = false;
		return flag;
	}
	flag = true;

	rows = 0;
	cols = 0;

	fscanf(fp, "%c", &tmpChar);
	while(1){
		fscanf(fp, "%c", &tmpChar);
		if(tmpChar != 45){
			if(tmpChar != 10){
				Error_Exit("input is wrong!");
			}
			else{
				break;
			}
		}
		fscanf(fp, "%c", &tmpChar);
		if(tmpChar != 43){
			Error_Exit("Input is wrong!");
		}
		else{
			cols++;
		}
	}

	tmprow = NULL;
	nxtrow = NULL;
	tmprow = (VTX*)malloc(sizeof(VTX)*cols);
	pushQ_VLL(tmprow);

	while(1){
		nlineflag = false;
		fscanf(fp, "%c", &tmpChar);
		
		for(i=0;i<cols;i++){
			tmprow[i].col = i;
			tmprow[i].row = rows;
			fscanf(fp, "%c", &tmpChar);
			if(tmpChar != 32){
				Error_Exit("Input Error!");
			}
			fscanf(fp, "%c", &tmpChar);
			if(tmpChar == 32){
				tmprow[i].right = true;
			}
			else if(tmpChar != 124){
				Error_Exit("Input Error!");
			}
			else{
				tmprow[i].right = false;
			}
		}
		tmprow[cols-1].right = false;

		fscanf(fp, "%c", &tmpChar);
		fscanf(fp, "%c", &tmpChar);

		for(i=0;i<cols;i++){
			fscanf(fp, "%c", &tmpChar);
			if(tmpChar == 32){
				if(!nlineflag){
					nlineflag = true;
					nxtrow = (VTX*)malloc(sizeof(VTX)*cols);
					pushQ_VLL(nxtrow);
					rows++;
				}
				tmprow[i].down = true;
			}
			else if(tmpChar != 45){
				Error_Exit("Input Error!");
			}
			else{
				tmprow[i].down = false;
			}
			fscanf(fp, "%c", &tmpChar);
		}
		if(!nlineflag){
			for(i=0;i<cols;i++){
				tmprow[i].down = false;
			}
			break;
		}
		tmprow = nxtrow;
		nxtrow = NULL;
		fscanf(fp, "%c", &tmpChar);
	}
	fileflag = true;
	rows++;
	tmprow = NULL;
	nxtrow = NULL;
	last_ptr = NULL;
	fclose(fp);

	maze = (VTX**)malloc(sizeof(VTX*)*rows);
	for(i=0;i<rows;i++){
		maze[i] = popQ_VLL(&VC);
	}
	setWindow(0, 0, 4*cols+5, 4*rows+5, 0);

	return flag;
}

/******************************************************************
* function	: bool FreeMemory()
*
* remark	: Save user data to a file
*******************************************************************/
void freeMemory(){
	int i, j;
	if(maze != NULL){
		for(i=0;i<rows;i++){
			free(maze[i]);
		}
		free(maze);
		maze=NULL;
	}
	if(fileflag){
		while(1){
			if(VC==NULL){
				break;
			}
			popQ_VLL(&VC);
		}
	}
	if(VLL_pool != NULL){
		Free_VLL_pool();
	}
	if(VLL_cnt !=0){
		Error_Exit("Storage does not cleared!");
	}
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
	VLL *ptr;
	VTX *V;
	int lineWidth = 0.01;
	int i, j;
	int x, y;
	DrawSolidBox_I(0, 4*rows, 1, 4*rows+1, lineWidth, RGB(0, 0, 255), RGB(0, 0, 255));
	for(i=0;i<cols;i++){
		DrawSolidBox_I(4*i+1, 4*rows, 4*i+4, 4*rows+1, lineWidth, RGB(0, 0, 255), RGB(0, 0, 255));
		DrawSolidBox_I(4*i+4, 4*rows, 4*i+5, 4*rows+1, lineWidth, RGB(0, 0, 255), RGB(0, 0, 255));
	}
	for(i=0;i<rows;i++){
		DrawSolidBox_I(0, 4*(rows-i)-3, 1, 4*(rows-i), lineWidth, RGB(0, 0, 255), RGB(0, 0, 255));
		DrawSolidBox_I(0, 4*(rows-i-1), 1, 4*(rows-i)-3, lineWidth, RGB(0, 0, 255), RGB(0, 0, 255));
		for(j=0;j<cols;j++){
			if(maze[i][j].right==false){
				DrawSolidBox_I(4*(j+1), 4*(rows-i)-3, 4*j+5, 4*(rows-i), lineWidth, RGB(0, 0, 255), RGB(0, 0, 255));
			}
			if(maze[i][j].down==false){
				DrawSolidBox_I(4*j+1, 4*(rows-i-1), 4*(j+1), 4*(rows-i)-3, lineWidth, RGB(0, 0, 255), RGB(0, 0, 255));
			}
			DrawSolidBox_I(4*(j+1), 4*(rows-i-1), 4*j+5, 4*(rows-i)-3, lineWidth, RGB(0, 0, 255), RGB(0, 0, 255));
		}
	}
}

void pushQ_VLL(VTX *node){
	VLL *L;
	
	if(last_ptr==NULL){
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

VLL *Alloc_VLL(void){
	VLL *ptr;

	if(VLL_pool==NULL){
		ptr = (VLL*)malloc(sizeof(VLL));
		if(ptr==NULL){
			Error_Exit("No memory in Alloc_ptr_L");
		}
		Total_Used_Memory += sizeof(VLL);
	}
	else{
		ptr = VLL_pool;
		VLL_pool = ptr->next;
	}
	ptr->next = NULL;
	++VLL_cnt;
	return(ptr);
}

VTX *popQ_VLL(VLL **Q){
	VTX *i;
	VLL *tQ;

	i = (*Q)->node;
	tQ = *Q;
	if((*Q)->next==NULL){
		*Q = NULL;
		last_ptr = NULL;
	}
	else{
		*Q = (*Q)->next;
	}
	Free_VLL(tQ);
	return(i);
}

void Free_VLL(VLL *vll){
	vll->node = NULL;
	vll->next = VLL_pool;
	VLL_pool = vll;
	--VLL_cnt;
}

void Free_VLL_pool(void){
	VLL *p;
	last_ptr = NULL;
	p = VLL_pool;
	while(p!=NULL){
		free(VLL_pool->node);
		VLL_pool = p->next;
		free(p);
		p = VLL_pool;
		Total_Used_Memory -= sizeof(VLL);
	}
	if(VLL_cnt != 0){
		Error_Exit("Non-zero VLL_cnt after deallocate VLL_pool.");
	}
	VLL_pool = NULL;
}


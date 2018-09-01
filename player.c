#include <stdio.h>
#include <stdlib.h>


/*Defines*/
	#ifndef ROW_SIZE
	#define ROW_SIZE 200
	#endif
	#ifndef COL_SIZE
	#define COL_SIZE 200
	#endif

	#ifndef WHITE
	#define WHITE 0
	#endif

	#ifndef BLACK
	#define BLACK 1
	#endif

	#ifndef PENDING
	#define PENDING 2
	#endif

	#ifndef SEARCH_LVL
	#define SEARCH_LVL 6
	#endif
/*/Defines*/

/*Enums*/
	enum ARROW
	{
		Left, Right, Top, Bottom
	};
/*Enums*/

/*Structs*/
	struct Tile
	{
		int left, right, top, bottom;
	};

	struct PathNode {
		int y, x;
		enum ARROW arrow;
	};

	struct Path
	{
		struct PathNode nodes[2];
		struct Path* next;
	};

	struct Queue
	{
		struct Path* first;
	};
/*/Structs*/

/*Prototypes*/
	int addTile(char* move);
	int* parseMove(char* move);
	struct Tile* getCorrectInsert(int y, int x,int type);
	struct Tile* initialTile(int type);
	struct Tile* initDefTile(int type);
	void reverseTile(struct Tile*);
	int hasNeighbour(int y, int x);
	int getTileType(struct Tile*);
	void printBoard();
	int* getAvailableTileTypes(int x, int y);
	int setForceMoves(int y, int x);
	struct Queue* initialQueue();
	int QueueIsEmpty(struct Queue* q);
	void QueueInsert(struct Queue* q,struct Path* path);
	struct Path* QueueGet(struct Queue* q, int i);
	void QueueDelete(struct Queue* q, int i);
	void reOrderPathes(struct Queue* queue, int y, int x, struct Tile* tile, int color);
	struct Path* initialPath();
	int getBestMove(struct Queue* queue, int color, int* move);
	int getRecBestMove(int bef_y, int bef_x, int y,int x,int type,int* tempMove,int search_lvl, struct PathNode * destination,int color);
	void reParsemove(int y, int x, int type, char ret[]);
	void AI(int lenW,int y_w,int x_w,int m_w,int lenB,int y_b,int x_b,int m_b,int* res_mov, int color);

/*/Prototypes*/

/*Global Variables*/
	unsigned int Y_INDEX = ROW_SIZE/2;
	unsigned int X_INDEX = COL_SIZE/2;
	int hasMoved = 0;
	struct Tile * board[ROW_SIZE][COL_SIZE];
	int turn = WHITE;
	struct Queue* whitePathes;
	struct Queue* blackPathes;
	short int status = PENDING;
	struct Tile* default_tiles[6];
	int me;
	int started = 0;
/*Functions*/
	int main(int argc, char const *argv[])
	{
	    char current_move[6];
	    int i, j, *m_w,* m_b,*res_mov, lenW, lenB;
	    whitePathes = initialQueue();
	    blackPathes = initialQueue();
	    m_w = malloc(3 * sizeof(int));
	    m_b = malloc(3 * sizeof(int));
	    res_mov = malloc(3 * sizeof(int));

	    for (i = 0; i < 6; ++i)
	    {
	    	default_tiles[i] = initDefTile(i+1);
	    }

		for (i = 0; i < ROW_SIZE; ++i)
		{
			for (j = 0; j < COL_SIZE; ++j)
			{
				board[i][j] = NULL;
			}
		}

		while (status == PENDING)
		{
			if(!started)
			{
				printf("Choose your color: W[hite] / B[lack]\n");
				scanf("%s",current_move); // from RS232
				//cin(current_move);
				if (current_move[0] == 'W')
				{
					me = WHITE;
				}
				else {
					me = BLACK;
				}
				started = 1;
				continue;
			}
			if (turn != me)
			{
				scanf("%s",current_move); // from RS232
				//cin(current_move);
				if (addTile(current_move))
				{
					turn = !turn;
				}
			}
			else {
				lenW = getBestMove(whitePathes, WHITE, m_w);
				lenB = getBestMove(blackPathes, BLACK, m_b);

				AI(lenW, m_w[0], m_w[1], m_w[2], lenB, m_b[0], m_b[1], m_b[2], res_mov, turn);
				reParsemove(res_mov[0], res_mov[1], res_mov[2], current_move);
				if (addTile(current_move))
				{
					printf("%s", current_move);
					//cout(current_move);
					turn = !turn;
				}
			}
		}
		free(whitePathes);
		free(blackPathes);
		return 0;
	}


	int addTile(char* move)
	{
		int* m = parseMove(move);
		int y, x;
		struct Tile* tmp;
		if (m == NULL)
		{
			return 0;
		}

		y = m[0];
		x = m[1];

		if (y < 0 || y >= ROW_SIZE || x < 0 || x >= COL_SIZE)
		{
			return 0;
		}
		if (board[y][x] != NULL)
		{
			return 0;
		}

		if (hasMoved == 0)
		{
			if (y != Y_INDEX -1 && x != X_INDEX -1)
			{
				return 0;
			}

			if (m[2] == 3)
			{
				return 0;
			}

			tmp = initialTile(m[2]);
			board[y][x] = tmp;
			reOrderPathes(whitePathes, y, x, tmp, WHITE);
			reOrderPathes(blackPathes, y, x, tmp, BLACK);
			if (y < Y_INDEX) Y_INDEX = y;
			if (x < X_INDEX) X_INDEX = x;
			hasMoved = 1;
			return 1;
		}

		tmp = getCorrectInsert(y,x, m[2]);
		if (tmp == NULL)
		{
			return 0;
		} else {
			board[y][x] = tmp;
			reOrderPathes(whitePathes, y, x, tmp, WHITE);
			reOrderPathes(blackPathes, y, x, tmp, BLACK);
			setForceMoves(y + 1, x);
			setForceMoves(y - 1, x);
			setForceMoves(y, x + 1);
			setForceMoves(y, x - 1);
			if (y < Y_INDEX) Y_INDEX = y;
			if (x < X_INDEX) X_INDEX = x;
			return 1;
		}
	}

	int* parseMove(char* move)
	{
		int* m = malloc(3 * sizeof(int));
		int i = 0;
		int z = 1;
		m[1] = 0;
		m[0] = 0;
		if (move[0] == '@')
		{
			m[1] = -0;
			i++;
		} else {
			while((move[i] && move[i] >= 'A' && move[i] <= 'Z') ||(move[i] && move[i] >= 'a' && move[i] <= 'z'))
			{
				if (move[i] && move[i] >= 'a' && move[i] <= 'z') move[i] = move[i] - 'a' + 'A';

				m[1] = m[1] * z + (move[i] - 'A');
				m[1] += 1;
				i++;
				z*= 26;
			}
		}
		m[1]--;
		m[1] += X_INDEX;
		if ( i == 0) return NULL;
		z = 1;

		while(move[i] && move[i] >= '0' && move[i] <= '9')
		{
			m[0] = m[0] * z + move[i] - '0';
			i++;
			z*= 10;
		}
		m[0]--;
		m[0] += Y_INDEX;
		if (z == 1) return NULL;

		if (!move[i])
		{
			return NULL;
		}

		if (move[i] == '+' )
		{
			m[2] = 1;
		} else if(move[i] == '/')
		{
			m[2] = 2;
		} else if(move[i] == '\\')
		{
			m[2] = 3;
		} else return NULL;
		return m;
	}

	struct Tile* getCorrectInsert(int y, int x,int type)
	{
		struct Tile* tile = NULL;
		int* types;
		if(!hasNeighbour(y, x)) return NULL;

		types = getAvailableTileTypes(y, x);
		if (types[0] == 0) return NULL;

		if (type <= 6 && type >= 1 && types[type])
		{
			tile = initialTile(type);
		} else if (type + 3 <= 6 && type + 3 >= 1 && types[type+3])
		{
			tile = initialTile(type+3);
		}

		return tile;
	}

	struct Tile* initialTile(int type)
	{
		struct Tile* tile = NULL;
		if (type == 1)
		{
			tile = default_tiles[0];
		}
		else if (type == 2)
		{
			tile = default_tiles[1];
		}
		else if (type == 3) {
			tile = default_tiles[2];
		}
		else if (type == 4)
		{
			tile = default_tiles[3];
		}
		else if (type == 5)
		{
			tile = default_tiles[4];
		}
		else if ( type == 6)
		{
			tile = default_tiles[5];
		} else
		{
			free(tile);
			return NULL;
		}

		return tile;
	}

	struct Tile* initDefTile(int type)
	{
		struct Tile* tile = malloc(sizeof(struct Tile));
		if (type == 1)
		{
			tile->top = WHITE;
			tile->bottom = WHITE;
			tile->left = BLACK;
			tile->right = BLACK;
		}
		else if (type == 2)
		{
			tile->left = WHITE;
			tile->top = WHITE;
			tile->bottom = BLACK;
			tile->right = BLACK;
		}
		else if (type == 3) {
			tile->top = WHITE;
			tile->bottom = BLACK;
			tile->left = BLACK;
			tile->right = WHITE;
		}
		else if (type == 4)
		{
			tile->top = BLACK;
			tile->bottom = BLACK;
			tile->left = WHITE;
			tile->right = WHITE;
		}
		else if (type == 5)
		{
			tile->left = BLACK;
			tile->top = BLACK;
			tile->bottom = WHITE;
			tile->right = WHITE;
		}
		else if ( type == 6)
		{
			tile->top = BLACK;
			tile->bottom = WHITE;
			tile->left = WHITE;
			tile->right = BLACK;
		} else
		{
			free(tile);
			return NULL;
		}

		return tile;
	}


	void reverseTile(struct Tile* tile)
	{
		tile->left = !tile->left;
		tile->right = !tile->right;
		tile->top = !tile->top;
		tile->bottom = !tile->bottom;
	}

	int hasNeighbour(int y, int x)
	{
		if(y-1 >= 0 && y-1 < ROW_SIZE && x >= 0 && x < COL_SIZE && board[y-1][x] != NULL)
		{
			return 1;
		}

		if(y+1 >= 0 && y+1 < ROW_SIZE && x >= 0 && x < COL_SIZE && board[y+1][x] != NULL)
		{
			return 1;
		}

		if(y >= 0 && y < ROW_SIZE && x-1 >= 0 && x-1 < COL_SIZE && board[y][x-1] != NULL)
		{
			return 1;
		}

		if(y >= 0 && y < ROW_SIZE && x+1 >= 0 && x+1 < COL_SIZE && board[y][x+1] != NULL)
		{
			return 1;
		}
		return 0;
	}

	int getTileType(struct Tile* tile)
	{

		if(tile->top == WHITE &&
			tile->bottom == WHITE &&
			tile->left == BLACK &&
			tile->right == BLACK) return 1;

		if(tile->left == WHITE &&
			tile->top == WHITE &&
			tile->bottom == BLACK &&
			tile->right == BLACK) return 2;

		if(tile->top == WHITE &&
			tile->bottom == BLACK &&
			tile->left == BLACK &&
			tile->right == WHITE) return 3;

		if(tile->top == BLACK &&
			tile->bottom == BLACK &&
			tile->left == WHITE &&
			tile->right == WHITE) return 4;

		if(tile->left == BLACK &&
			tile->top == BLACK &&
			tile->bottom == WHITE &&
			tile->right == WHITE) return 5;


		if(tile->top == BLACK &&
			tile->bottom == WHITE &&
			tile->left == WHITE &&
			tile->right == BLACK) return 6;

		return 0;
	}
	/*void printBoard()
	{
		int i, j;
		for (i = Y_INDEX - 10; i < Y_INDEX + 10; ++i)
		{
			for (j = X_INDEX - 10; j < X_INDEX + 10; ++j)
			{
				if (board[i][j] == NULL)
				{
					cout<<"  ";
				} else if (board[i][j]->top == board[i][j]->bottom)
				{
					cout<<" +";
				} else if (board[i][j]->top == board[i][j]->right)
				{
					cout<<" \\";
				} else {
					cout<<" /";
				}

			}
			cout<<endl;
		}
	}*/


	int* getAvailableTileTypes(int y, int x)
	{
		int* a = malloc(8 * sizeof(int));
		int i, count;
		for (i = 0; i < 7; ++i)
		{
			a[i] = 1;
		}
		a[0] = 0;
		a[7] = 0;
		if(y-1 >= 0 && y-1 < ROW_SIZE && x >= 0 && x < COL_SIZE && board[y-1][x] != NULL)
		{
			if (BLACK == board[y-1][x]->bottom)
			{
				a[1] = a[2] = a[3] = 0;
			}
			if (WHITE == board[y-1][x]->bottom)
			{

				a[4] = a[5] = a[6] = 0;
			}
		}

		if(y+1 >= 0 && y+1 < ROW_SIZE && x >= 0 && x < COL_SIZE && board[y+1][x] != NULL)
		{
			if (BLACK == board[y+1][x]->top)
			{
				a[1] = a[5] = a[6] = 0;
			}
			if (WHITE == board[y+1][x]->top)
			{
				a[2] = a[3] = a[4] = 0;
			}
		}

		if(y >= 0 && y < ROW_SIZE && x-1 >= 0 && x-1 < COL_SIZE && board[y][x-1] != NULL)
		{
			if (BLACK == board[y][x-1]->right)
			{
				a[2] = a[4] = a[6] = 0;
			}
			if (WHITE == board[y][x-1]->right)
			{
				a[1] = a[3] = a[5] = 0;
			}
		}

		if(y >= 0 && y < ROW_SIZE && x+1 >= 0 && x+1 < COL_SIZE && board[y][x+1] != NULL)
		{
			if (BLACK == board[y][x+1]->left)
			{
				a[3] = a[4] = a[5] = 0;
			}
			if (WHITE == board[y][x+1]->left)
			{
				a[1] = a[2] = a[6] = 0;
			}
		}
		for (i = 1; i < 7; ++i)
		{
			if (a[i] == 1)
			{
				a[0] = 1;
				break;
			}
		}
		if (a[0])
		{
			count = 0;
			for (i = 1; i < 7; ++i)
			{
				if (a[i])
				{
					count++;
				}
			}
			if (count == 1)
			{
				a[7] = 1;
			}
		}

		return a;
	}


	int setForceMoves(int y, int x)
	{
		int* a, count, i;
		if (board[y][x] == NULL)
		{
			a = getAvailableTileTypes(y, x);
			if (a[0] == 0) return 0;
			else {
				count = 0;
				for (i = 1; i < 7; ++i)
				{
					if (a[i])
					{
						count++;
					}
				}
				if (count == 1)
				{
					for (i = 1; i < 7; ++i)
					{
						if (a[i])
						{
							board[y][x] = initialTile(i);
							reOrderPathes(whitePathes, y, x, board[y][x], WHITE);
							reOrderPathes(blackPathes, y, x, board[y][x], BLACK);
							setForceMoves(y + 1, x);
							setForceMoves(y - 1, x);
							setForceMoves(y, x + 1);
							setForceMoves(y, x - 1);
							return 1;
						}
					}
				}
			}
		}
	}

	struct Queue* initialQueue()
	{
		struct Queue* ret = malloc(sizeof(struct Queue));
		ret->first = NULL;
		return ret;
	}

	int QueueIsEmpty(struct Queue* q)
	{
		return q->first == NULL;
	}

	void QueueInsert(struct Queue* q,struct Path* path)
	{
		struct Path* x;
		if (QueueIsEmpty(q))
		{
			q->first = path;
		} else {
			x = q->first;
			while( x ->next != NULL)
			{
				x = x->next;
			}
			x->next = path;
		}
	}

	struct Path* QueueGet(struct Queue* q, int i)
	{
		struct Path* x = q->first;
		int n;
		for (n = 0; n <= i; ++n)
		{
			if(x != NULL)
			{
				x = x->next;
			}
		}
		return x;
	}

	void QueueDelete(struct Queue* q, int i)
	{

		struct Path* tmp, *x;
		int n;
		if (QueueIsEmpty(q))
		{
			return;
		}

		if (i == 0)
		{
			tmp = q->first->next;
			free(q->first);
			q->first = tmp;
		} else {
			x = q->first;
			for (n = 1; n < i; ++n)
			{
				if (x->next != NULL)
				{
					x = x->next;
				}
			}
			if (x->next == NULL)
			{
				return;
			} else {
				tmp = x->next->next;
				free(x->next);
				x->next = tmp;
			}
		}


	}

	void reOrderPathes(struct Queue* q, int y, int x, struct Tile* tile, int color)
	{
	    struct Path* tmp = NULL,* a, * b;
	    int ai, bi;
		int bd = -1, ad = -1, d = 0;
		int leftC = 0, rightC = 0, topC = 0, bottomC = 0, i;

		if(tile->right == color)
			if (y >= 0 && y < ROW_SIZE && x+1 >= 0 && x+1 < COL_SIZE && board[y][x+1] != NULL)
			{
					rightC = 1;
			}
		if(tile->left == color)
			if (y >= 0 && y < ROW_SIZE && x-1 >= 0 && x-1 < COL_SIZE && board[y][x-1] != NULL)
			{
					leftC = 1;
			}
		if(tile->top == color)
			if (y-1 >= 0 && y-1 < ROW_SIZE && x >= 0 && x < COL_SIZE && board[y-1][x] != NULL)
			{
					topC = 1;
			}
		if(tile->bottom == color)
			if (y+1 >= 0 && y+1 < ROW_SIZE && x >= 0 && x < COL_SIZE && board[y+1][x] != NULL)
			{
					bottomC = 1;
			}

		if (!bottomC && !topC && !leftC && !rightC)
		{
			a = initialPath();
			i = 0;
			if (tile->left == color)
			{
				a->nodes[i].arrow = Left;
				i = 1;
			}
			if (tile->right == color)
			{
				a->nodes[i].arrow = Right;
				i = 1;
			}
			if (tile->top == color)
			{
				a->nodes[i].arrow = Top;
				i = 1;
			}
			if (tile->bottom == color)
			{
				a->nodes[i].arrow = Bottom;
				i = 1;
			}
			a->nodes[0].x = x;
			a->nodes[1].x = x;
			a->nodes[0].y = y;
			a->nodes[1].y = y;
			QueueInsert(q, a);
		}else if (bottomC && !topC && !leftC && !rightC)
		{
			tmp = q->first;
			while(tmp != NULL)
			{
				if (tmp->nodes[0].y == y+1 && tmp->nodes[0].x == x && tmp->nodes[0].arrow == Top)
				{
					tmp->nodes[0].y = y;
					if (tile->left == color) tmp->nodes[0].arrow = Left;
					if (tile->right == color) tmp->nodes[0].arrow = Right;
					if (tile->top == color) tmp->nodes[0].arrow = Top;
					break;
				} else if (tmp->nodes[1].y == y+1 && tmp->nodes[1].x == x && tmp->nodes[1].arrow == Top)
				{
					tmp->nodes[1].y = y;
					if (tile->left == color) tmp->nodes[1].arrow = Left;
					if (tile->right == color) tmp->nodes[1].arrow = Right;
					if (tile->top == color) tmp->nodes[1].arrow = Top;
					break;
				} else
				{
					tmp = tmp->next;
				}

			}
		}else if (!bottomC && topC && !leftC && !rightC)
		{
			tmp = q->first;
			while(tmp != NULL)
			{
				if (tmp->nodes[0].y == y-1 && tmp->nodes[0].x == x && tmp->nodes[0].arrow == Bottom)
				{
					tmp->nodes[0].y = y;
					if (tile->left == color) tmp->nodes[0].arrow = Left;
					if (tile->right == color) tmp->nodes[0].arrow = Right;
					if (tile->bottom == color) tmp->nodes[0].arrow = Bottom;
					break;
				} else if (tmp->nodes[1].y == y-1 && tmp->nodes[1].x == x && tmp->nodes[1].arrow == Bottom)
				{
					tmp->nodes[1].y = y;
					if (tile->left == color) tmp->nodes[1].arrow = Left;
					if (tile->right == color) tmp->nodes[1].arrow = Right;
					if (tile->bottom == color) tmp->nodes[1].arrow = Bottom;
					break;
				} else
				{
					tmp = tmp->next;
				}

			}
		}else if (!bottomC && !topC && leftC && !rightC)
		{
			tmp = q->first;
			while(tmp != NULL)
			{
				if (tmp->nodes[0].y == y && tmp->nodes[0].x == x-1 && tmp->nodes[0].arrow == Right)
				{
					tmp->nodes[0].x = x;
					if (tile->right == color) tmp->nodes[0].arrow = Right;
					if (tile->top == color) tmp->nodes[0].arrow = Top;
					if (tile->bottom == color) tmp->nodes[0].arrow = Bottom;
					break;
				} else if (tmp->nodes[1].y == y && tmp->nodes[1].x == x-1 && tmp->nodes[1].arrow == Right)
				{
					tmp->nodes[1].x = x;
					if (tile->right == color) tmp->nodes[1].arrow = Right;
					if (tile->top == color) tmp->nodes[1].arrow = Top;
					if (tile->bottom == color) tmp->nodes[1].arrow = Bottom;
					break;
				} else
				{
					tmp = tmp->next;
				}

			}
		}else if (!bottomC && !topC && !leftC && rightC)
		{
			tmp = q->first;
			while(tmp != NULL)
			{
				if (tmp->nodes[0].y == y && tmp->nodes[0].x == x+1 && tmp->nodes[0].arrow == Left)
				{
					tmp->nodes[0].x = x;
					if (tile->left == color) tmp->nodes[0].arrow = Left;
					if (tile->top == color) tmp->nodes[0].arrow = Top;
					if (tile->bottom == color) tmp->nodes[0].arrow = Bottom;
					break;
				} else if (tmp->nodes[1].y == y && tmp->nodes[1].x == x+1 && tmp->nodes[1].arrow == Left)
				{
					tmp->nodes[1].x = x;
					if (tile->left == color) tmp->nodes[1].arrow = Left;
					if (tile->top == color) tmp->nodes[1].arrow = Top;
					if (tile->bottom == color) tmp->nodes[1].arrow = Bottom;
					break;
				} else
				{
					tmp = tmp->next;
				}

			}
		}else if (bottomC && topC && !leftC && !rightC)
		{
			a = NULL;
			b = NULL;
			bd = -1;
			ad = -1;
			d = 0;
			tmp = q->first;
			while(tmp != NULL)
			{
				if (tmp->nodes[0].y == y+1 && tmp->nodes[0].x == x && tmp->nodes[0].arrow == Top)
				{
					a = tmp;
					ai = 0;
					ad = d;
				}else if (tmp->nodes[1].y == y+1 && tmp->nodes[1].x == x && tmp->nodes[1].arrow == Top)
				{
					a = tmp;
					ai = 1;
					ad = d;
				}

				if (tmp->nodes[0].y == y-1 && tmp->nodes[0].x == x && tmp->nodes[0].arrow == Bottom)
				{
					bd = d;
					b = tmp;
				} else if (tmp->nodes[1].y == y-1 && tmp->nodes[1].x == x && tmp->nodes[1].arrow == Bottom)
				{
					bd = d;
					b = tmp;
					bi = 1;
				}
				if (a != NULL && b != NULL) break;
				d++;
				tmp = tmp->next;
			}
			if (a == b)
			{
				status = color;
			} else {
				a->nodes[ai].x = b->nodes[!bi].x;
				a->nodes[ai].y = b->nodes[!bi].y;
				a->nodes[ai].arrow = b->nodes[!bi].arrow;
				QueueDelete(q, bd);

			}

		}else if (bottomC && !topC && leftC && !rightC)
		{
			a = NULL;
			b = NULL;
			bd = -1;
			ad = -1;
			d = 0;
			tmp = q->first;
			while(tmp != NULL)
			{
				if (tmp->nodes[0].y == y+1 && tmp->nodes[0].x == x && tmp->nodes[0].arrow == Top)
				{
					a = tmp;
					ai = 0;
					ad = d;
				} else if (tmp->nodes[1].y == y+1 && tmp->nodes[1].x == x && tmp->nodes[1].arrow == Top)
				{
					a = tmp;
					ai = 1;
					ad = d;
				}

				if (tmp->nodes[0].y == y && tmp->nodes[0].x == x-1 && tmp->nodes[0].arrow == Right)
				{
					b = tmp;
					bd = d;
					bi = 0;
				} else if (tmp->nodes[1].y == y && tmp->nodes[1].x == x-1 && tmp->nodes[1].arrow == Right)
				{
					bd = d;
					b = tmp;
					bi = 1;
				}
				if (a != NULL && b != NULL) break;
				d++;
				tmp = tmp->next;
			}
			if (a == b)
			{
				status = color;
			} else {
				a->nodes[ai].x = b->nodes[!bi].x;
				a->nodes[ai].y = b->nodes[!bi].y;
				a->nodes[ai].arrow = b->nodes[!bi].arrow;
				QueueDelete(q, bd);

			}

		}else if (bottomC && !topC && !leftC && rightC)
		{
			a = NULL;
			b = NULL;
			bd = -1;
			ad = -1;
			d = 0;
			tmp = q->first;
			while(tmp != NULL)
			{
				if (tmp->nodes[0].y == y+1 && tmp->nodes[0].x == x && tmp->nodes[0].arrow == Top)
				{
					a = tmp;
					ai = 0;
					ad = d;
				} else if (tmp->nodes[1].y == y+1 && tmp->nodes[1].x == x && tmp->nodes[1].arrow == Top)
				{
					a = tmp;
					ai = 1;
					ad = d;
				}

				if (tmp->nodes[0].y == y && tmp->nodes[0].x == x+1 && tmp->nodes[0].arrow == Left)
				{
					b = tmp;
					bd = d;
					bi = 0;
				} else if (tmp->nodes[1].y == y && tmp->nodes[1].x == x+1 && tmp->nodes[1].arrow == Left)
				{
					bd = d;
					b = tmp;
					bi = 1;
				}
				if (a != NULL && b != NULL) break;
				d++;
				tmp = tmp->next;
			}
			if (a == b)
			{
				status = color;
			} else {
				a->nodes[ai].x = b->nodes[!bi].x;
				a->nodes[ai].y = b->nodes[!bi].y;
				a->nodes[ai].arrow = b->nodes[!bi].arrow;
				QueueDelete(q, bd);

			}

		}else if (!bottomC && topC && leftC && !rightC)
		{
			a = NULL;
			b = NULL;
			bd = -1;
			ad = -1;
			d = 0;
			tmp = q->first;
			while(tmp != NULL)
			{
				if (tmp->nodes[0].y == y-1 && tmp->nodes[0].x == x && tmp->nodes[0].arrow == Bottom)
				{
					a = tmp;
					ai = 0;
					ad = d;
				} else if (tmp->nodes[1].y == y-1 && tmp->nodes[1].x == x && tmp->nodes[1].arrow == Bottom)
				{
					a = tmp;
					ai = 1;
					ad = d;
				}

				if (tmp->nodes[0].y == y && tmp->nodes[0].x == x-1 && tmp->nodes[0].arrow == Right)
				{
					b = tmp;
					bd = d;
					bi = 0;
				} else if (tmp->nodes[1].y == y && tmp->nodes[1].x == x-1 && tmp->nodes[1].arrow == Right)
				{
					bd = d;
					b = tmp;
					bi = 1;
				}
				if (a != NULL && b != NULL) break;
				d++;
				tmp = tmp->next;
			}
			if (a == b)
			{
				status = color;
			} else {
				a->nodes[ai].x = b->nodes[!bi].x;
				a->nodes[ai].y = b->nodes[!bi].y;
				a->nodes[ai].arrow = b->nodes[!bi].arrow;
				QueueDelete(q, bd);

			}

		}else if (!bottomC && topC && !leftC && rightC)
		{
			a = NULL;
			b = NULL;
			bd = -1;
			ad = -1;
			d = 0;
			tmp = q->first;
			while(tmp != NULL)
			{
				if (tmp->nodes[0].y == y-1 && tmp->nodes[0].x == x && tmp->nodes[0].arrow == Bottom)
				{
					a = tmp;
					ai = 0;
					ad = d;
				} else if (tmp->nodes[1].y == y-1 && tmp->nodes[1].x == x && tmp->nodes[1].arrow == Bottom)
				{
					a = tmp;
					ai = 1;
					ad = d;
				}

				if (tmp->nodes[0].y == y && tmp->nodes[0].x == x+1 && tmp->nodes[0].arrow == Left)
				{
					b = tmp;
					bd = d;
					bi = 0;
				} else if (tmp->nodes[1].y == y && tmp->nodes[1].x == x+1 && tmp->nodes[1].arrow == Left)
				{
					bd = d;
					b = tmp;
					bi = 1;
				}
				if (a != NULL && b != NULL) break;
				d++;
				tmp = tmp->next;
			}
			if (a == b)
			{
				status = color;
			} else {
				a->nodes[ai].x = b->nodes[!bi].x;
				a->nodes[ai].y = b->nodes[!bi].y;
				a->nodes[ai].arrow = b->nodes[!bi].arrow;
				QueueDelete(q, bd);

			}

		}else if (!bottomC && !topC && leftC && rightC)
		{
			a = NULL;
			b = NULL;
			bd = -1;
			ad = -1;
			d = 0;
			tmp = q->first;
			while(tmp != NULL)
			{
				if (tmp->nodes[0].y == y && tmp->nodes[0].x == x-1 && tmp->nodes[0].arrow == Right)
				{
					a = tmp;
					ai = 0;
					ad = d;
				} else if (tmp->nodes[1].y == y && tmp->nodes[1].x == x-1 && tmp->nodes[1].arrow == Right)
				{
					a = tmp;
					ai = 1;
					ad = d;
				}

				if (tmp->nodes[0].y == y && tmp->nodes[0].x == x+1 && tmp->nodes[0].arrow == Left)
				{
					b = tmp;
					bd = d;
					bi = 0;
				} else if (tmp->nodes[1].y == y && tmp->nodes[1].x == x+1 && tmp->nodes[1].arrow == Left)
				{
					bd = d;
					b = tmp;
					bi = 1;
				}
				if (a != NULL && b != NULL) break;
				d++;
				tmp = tmp->next;
			}
			if (a == b)
			{
				status = color;
			} else {
				a->nodes[ai].x = b->nodes[!bi].x;
				a->nodes[ai].y = b->nodes[!bi].y;
				a->nodes[ai].arrow = b->nodes[!bi].arrow;
				QueueDelete(q, bd);

			}

		}
	}

	struct Path* initialPath()
	{
		struct Path* ret = malloc(sizeof(struct Path));
		ret->next = NULL;
	}

	int getBestMove(struct Queue* queue, int color, int* move)
	{
		int tmpLen;
		int x,y, bestLen = 99999, bef_y, bef_x;
		struct Path* path = queue->first;
		int tempMove[3];
		int* a;
		int i, j;
		while(path != NULL)
		{
			bef_x = path->nodes[0].x;
			bef_y = path->nodes[0].y;
			if(path->nodes[0].arrow == Left)
			{
				x = path->nodes[0].x - 1;
				y = path->nodes[0].y;
			}else if(path->nodes[0].arrow == Right)
			{
				x = path->nodes[0].x + 1;
				y = path->nodes[0].y;
			}else if(path->nodes[0].arrow == Top)
			{
				x = path->nodes[0].x;
				y = path->nodes[0].y - 1;
			}else if(path->nodes[0].arrow == Bottom)
			{
				x = path->nodes[0].x;
				y = path->nodes[0].y + 1;
			}
			a = getAvailableTileTypes(y,x);
			for (i = 1; i < 7; ++i)
			{
				if (a[i])
				{
					tmpLen = getRecBestMove(bef_y, bef_x, y,x, i, tempMove, SEARCH_LVL, &(path->nodes[1]), color) + 1;

					if(tmpLen < bestLen)
					{
						for (j = 0; j < 3; ++j)
						{
							move[j] = tempMove[j];
						}
						bestLen = tmpLen;
					}
				}
			}
			path = path->next;
		}
		return bestLen;

	}

	int getRecBestMove(int bef_y,int bef_x, int y,int x,int type,int* move,int search_lvl, struct PathNode * destination,int color)
	{

		int k;
		int* a,*b,*c,*d;
		int ai = 0, bi = 0, ci = 0, di = 0;
		int tempMove[3];
		int bestLen = 9999;
		int tmpLen = 10000;
		int i,j;
		struct Path* path;
		a = b = c= d = NULL;
		if (search_lvl == 0){
			move[0] = y;
			move[1] = x;
			move[2] = type;
			return 9999;
		}

		if (board[y][x] != NULL)
		{
			move[0] = y;
			move[1] = x;
			move[2] = type;
			return 9999;
		}
		board[y][x] = initialTile(type);
		//printBoard();
		if (destination->arrow == Top && y == destination->y - 1 && x == destination->x)
		{
			board[y][x] = NULL;
			move[0] = y;
			move[1] = x;
			move[2] = type;
			return 0;
		}
		if (destination->arrow == Bottom && y == destination->y + 1 && x == destination->x)
		{
			board[y][x] = NULL;
			move[0] = y;
			move[1] = x;
			move[2] = type;
			return 0;
		}
		if (destination->arrow == Left && y == destination->y && x == destination->x - 1)
		{
			board[y][x] = NULL;
			move[0] = y;
			move[1] = x;
			move[2] = type;
			return 0;
		}
		if (destination->arrow == Right && y == destination->y && x == destination->x + 1)
		{
			board[y][x] = NULL;
			move[0] = y;
			move[1] = x;
			move[2] = type;
			return 0;
		}




		if (board[y][x]->top == color && board[y-1][x] == NULL)
		{
			a = getAvailableTileTypes(y-1,x);
			if (a[7])
			{
				for (i = 1; i < 7; ++i)
				{
					if (a[i])
					{
						tmpLen = getRecBestMove(y, x, y - 1, x, i, tempMove, search_lvl, destination, color);
						if (tmpLen < bestLen)
						{
							bestLen = tmpLen;
							for (j = 0; j < 3; ++j)
							{
								move[j] = tempMove[j];
							}
						}
						if ((type == 1 || type == 4) && tmpLen < 9999 && tempMove[2] != 1 && tempMove[2] != 4)
						{
							bestLen = tmpLen - 1;
							move[0] = y;
							move[1] = x;
							move[2] = type;
						}
						break;
					}
				}
			}
			else if (a[0])
			{
				for (i = 1; i < 7; ++i)
				{
					if (a[i])
					{

						tmpLen = getRecBestMove(y, x, y - 1, x, i, tempMove, search_lvl-1, destination, color) + 1;
						if (tmpLen < bestLen)
						{
							bestLen = tmpLen;
							for (j = 0; j < 3; ++j)
							{
								move[j] = tempMove[j];
							}
						}
					}
				}
			}
		} else if(board[y-1][x] != NULL && board[y][x]->top == color && (y-1 != bef_y || x != bef_x))
		{
			path = (color == WHITE) ? whitePathes->first : blackPathes->first;
			while(path != NULL)
			{
				if (path->nodes[0].y == y-1 && path->nodes[0].x == x )
				{
					if (path->nodes[1].arrow == Top)
					{
						a = getAvailableTileTypes(path->nodes[1].y - 1, path->nodes[1].x);
						for (i = 1; i < 7; ++i)
						{
							if (a[i])
							{
								tmpLen = getRecBestMove(path->nodes[1].y, path->nodes[1].x, path->nodes[1].y - 1, path->nodes[1].x, i, tempMove, search_lvl-1, destination, color) + 1;
								if (tmpLen < bestLen)
								{
									bestLen = tmpLen;
									for (j = 0; j < 3; ++j)
									{
										move[j] = tempMove[j];
									}
								}
							}
						}
						break;
					} else if (path->nodes[1].arrow == Bottom)
					{
						a = getAvailableTileTypes(path->nodes[1].y + 1, path->nodes[1].x);
						for (i = 1; i < 7; ++i)
						{
							if (a[i])
							{
								tmpLen = getRecBestMove(path->nodes[1].y, path->nodes[1].x, path->nodes[1].y + 1, path->nodes[1].x, i, tempMove, search_lvl-1, destination, color) + 1;
								if (tmpLen < bestLen)
								{
									bestLen = tmpLen;
									for (j = 0; j < 3; ++j)
									{
										move[j] = tempMove[j];
									}
								}
							}
						}
						break;
					} else if (path->nodes[1].arrow == Left)
					{
						a = getAvailableTileTypes(path->nodes[1].y, path->nodes[1].x - 1);
						for (i = 1; i < 7; ++i)
						{
							if (a[i])
							{
								tmpLen = getRecBestMove(path->nodes[1].y, path->nodes[1].x, path->nodes[1].y, path->nodes[1].x - 1, i, tempMove, search_lvl-1, destination, color) + 1;
								if (tmpLen < bestLen)
								{
									bestLen = tmpLen;
									for (j = 0; j < 3; ++j)
									{
										move[j] = tempMove[j];
									}
								}
							}
						}
						break;
					} else {
						a = getAvailableTileTypes(path->nodes[1].y, path->nodes[1].x + 1);
						for (i = 1; i < 7; ++i)
						{
							if (a[i])
							{
								tmpLen = getRecBestMove(path->nodes[1].y, path->nodes[1].x, path->nodes[1].y, path->nodes[1].x + 1, i, tempMove, search_lvl-1, destination, color) + 1;
								if (tmpLen < bestLen)
								{
									bestLen = tmpLen;
									for (j = 0; j < 3; ++j)
									{
										move[j] = tempMove[j];
									}
								}
							}
						}
						break;
					}
				} else if (path->nodes[1].y == y-1 && path->nodes[1].x == x )
				{
					if (path->nodes[0].arrow == Top)
					{
						a = getAvailableTileTypes(path->nodes[0].y - 1, path->nodes[0].x);
						for (i = 1; i < 7; ++i)
						{
							if (a[i])
							{
								tmpLen = getRecBestMove(path->nodes[0].y, path->nodes[0].x, path->nodes[0].y - 1, path->nodes[0].x, i, tempMove, search_lvl-1, destination, color) + 1;
								if (tmpLen < bestLen)
								{
									bestLen = tmpLen;
									for (j = 0; j < 3; ++j)
									{
										move[j] = tempMove[j];
									}
								}
							}
						}
						break;
					} else if (path->nodes[0].arrow == Bottom)
					{
						a = getAvailableTileTypes(path->nodes[0].y + 1, path->nodes[0].x);
						for (i = 1; i < 7; ++i)
						{
							if (a[i])
							{
								tmpLen = getRecBestMove(path->nodes[0].y, path->nodes[0].x, path->nodes[0].y + 1, path->nodes[0].x, i, tempMove, search_lvl-1, destination, color) + 1;
								if (tmpLen < bestLen)
								{
									bestLen = tmpLen;
									for (j = 0; j < 3; ++j)
									{
										move[j] = tempMove[j];
									}
								}
							}
						}
						break;
					} else if (path->nodes[0].arrow == Left)
					{
						a = getAvailableTileTypes(path->nodes[0].y, path->nodes[0].x - 1);
						for (i = 1; i < 7; ++i)
						{
							if (a[i])
							{
								tmpLen = getRecBestMove(path->nodes[0].y, path->nodes[0].x, path->nodes[0].y, path->nodes[0].x - 1, i, tempMove, search_lvl-1, destination, color) + 1;
								if (tmpLen < bestLen)
								{
									bestLen = tmpLen;
									for (j = 0; j < 3; ++j)
									{
										move[j] = tempMove[j];
									}
								}
							}
						}
						break;
					} else {
						a = getAvailableTileTypes(path->nodes[0].y, path->nodes[0].x + 1);
						for (i = 1; i < 7; ++i)
						{
							if (a[i])
							{
								tmpLen = getRecBestMove(path->nodes[0].y, path->nodes[0].x, path->nodes[0].y, path->nodes[0].x + 1, i, tempMove, search_lvl-1, destination, color) + 1;
								if (tmpLen < bestLen)
								{
									bestLen = tmpLen;
									for (j = 0; j < 3; ++j)
									{
										move[j] = tempMove[j];
									}
								}
							}
						}
						break;
					}
				}
				path = path->next;
			}
		}
		if (board[y][x]->bottom == color && board[y+1][x] == NULL)
		{
			a = getAvailableTileTypes(y+1,x);
			if (a[7])
			{
				for (i = 1; i < 7; ++i)
				{
					if (a[i])
					{
						tmpLen = getRecBestMove(y, x, y + 1, x, i, tempMove, search_lvl, destination, color);
						if (tmpLen < bestLen)
						{
							bestLen = tmpLen;
							for (j = 0; j < 3; ++j)
							{
								move[j] = tempMove[j];
							}
						}
						if ((type == 1 || type == 4) && tmpLen < 9999 && tempMove[2] != 1 && tempMove[2] != 4)
						{
							bestLen = tmpLen - 1;
							move[0] = y;
							move[1] = x;
							move[2] = type;
						}
						break;
					}
				}
			}
			else if (a[0])
			{
				for (i = 1; i < 7; ++i)
				{
					if (a[i])
					{
						tmpLen = getRecBestMove(y, x, y + 1, x, i, tempMove, search_lvl-1, destination, color) + 1;
						if (tmpLen < bestLen)
						{
							bestLen = tmpLen;
							for (j = 0; j < 3; ++j)
							{
								move[j] = tempMove[j];
							}
						}
					}
				}
			}
		} else if(board[y+1][x] != NULL && board[y][x]->bottom == color && (y+1 != bef_y || x != bef_x))
		{
			path = (color == WHITE) ? whitePathes->first : blackPathes->first;
			while(path != NULL)
			{
				if (path->nodes[0].y == y+1 && path->nodes[0].x == x )
				{
					if (path->nodes[1].arrow == Top)
					{
						a = getAvailableTileTypes(path->nodes[1].y - 1, path->nodes[1].x);
						for (i = 1; i < 7; ++i)
						{
							if (a[i])
							{
								tmpLen = getRecBestMove(path->nodes[1].y, path->nodes[1].x, path->nodes[1].y - 1, path->nodes[1].x, i, tempMove, search_lvl-1, destination, color) + 1;
								if (tmpLen < bestLen)
								{
									bestLen = tmpLen;
									for (j = 0; j < 3; ++j)
									{
										move[j] = tempMove[j];
									}
								}
							}
						}
						break;
					} else if (path->nodes[1].arrow == Bottom)
					{
						a = getAvailableTileTypes(path->nodes[1].y + 1, path->nodes[1].x);
						for (i = 1; i < 7; ++i)
						{
							if (a[i])
							{
								tmpLen = getRecBestMove(path->nodes[1].y, path->nodes[1].x, path->nodes[1].y + 1, path->nodes[1].x, i, tempMove, search_lvl-1, destination, color) + 1;
								if (tmpLen < bestLen)
								{
									bestLen = tmpLen;
									for (j = 0; j < 3; ++j)
									{
										move[j] = tempMove[j];
									}
								}
							}
						}
						break;
					} else if (path->nodes[1].arrow == Left)
					{
						a = getAvailableTileTypes(path->nodes[1].y, path->nodes[1].x - 1);
						for (i = 1; i < 7; ++i)
						{
							if (a[i])
							{
								tmpLen = getRecBestMove(path->nodes[1].y, path->nodes[1].x, path->nodes[1].y, path->nodes[1].x - 1, i, tempMove, search_lvl-1, destination, color) + 1;
								if (tmpLen < bestLen)
								{
									bestLen = tmpLen;
									for (j = 0; j < 3; ++j)
									{
										move[j] = tempMove[j];
									}
								}
							}
						}
						break;
					} else {
						a = getAvailableTileTypes(path->nodes[1].y, path->nodes[1].x + 1);
						for (i = 1; i < 7; ++i)
						{
							if (a[i])
							{
								tmpLen = getRecBestMove(path->nodes[1].y, path->nodes[1].x, path->nodes[1].y, path->nodes[1].x + 1, i, tempMove, search_lvl-1, destination, color) + 1;
								if (tmpLen < bestLen)
								{
									bestLen = tmpLen;
									for (j = 0; j < 3; ++j)
									{
										move[j] = tempMove[j];
									}
								}
							}
						}
						break;
					}
				} else if (path->nodes[1].y == y+1 && path->nodes[1].x == x )
				{
					if (path->nodes[0].arrow == Top)
					{
						a = getAvailableTileTypes(path->nodes[0].y - 1, path->nodes[0].x);
						for (i = 1; i < 7; ++i)
						{
							if (a[i])
							{
								tmpLen = getRecBestMove(path->nodes[0].y, path->nodes[0].x, path->nodes[0].y - 1, path->nodes[0].x, i, tempMove, search_lvl-1, destination, color) + 1;
								if (tmpLen < bestLen)
								{
									bestLen = tmpLen;
									for (j = 0; j < 3; ++j)
									{
										move[j] = tempMove[j];
									}
								}
							}
						}
						break;
					} else if (path->nodes[0].arrow == Bottom)
					{
						a = getAvailableTileTypes(path->nodes[0].y + 1, path->nodes[0].x);
						for (i = 1; i < 7; ++i)
						{
							if (a[i])
							{
								tmpLen = getRecBestMove(path->nodes[0].y, path->nodes[0].x, path->nodes[0].y + 1, path->nodes[0].x, i, tempMove, search_lvl-1, destination, color) + 1;
								if (tmpLen < bestLen)
								{
									bestLen = tmpLen;
									for (j = 0; j < 3; ++j)
									{
										move[j] = tempMove[j];
									}
								}
							}
						}
						break;
					} else if (path->nodes[0].arrow == Left)
					{
						a = getAvailableTileTypes(path->nodes[0].y, path->nodes[0].x - 1);
						for (i = 1; i < 7; ++i)
						{
							if (a[i])
							{
								tmpLen = getRecBestMove(path->nodes[0].y, path->nodes[0].x, path->nodes[0].y, path->nodes[0].x - 1, i, tempMove, search_lvl-1, destination, color) + 1;
								if (tmpLen < bestLen)
								{
									bestLen = tmpLen;
									for (j = 0; j < 3; ++j)
									{
										move[j] = tempMove[j];
									}
								}
							}
						}
						break;
					} else {
						a = getAvailableTileTypes(path->nodes[0].y, path->nodes[0].x + 1);
						for (i = 1; i < 7; ++i)
						{
							if (a[i])
							{
								tmpLen = getRecBestMove(path->nodes[0].y, path->nodes[0].x, path->nodes[0].y, path->nodes[0].x + 1, i, tempMove, search_lvl-1, destination, color) + 1;
								if (tmpLen < bestLen)
								{
									bestLen = tmpLen;
									for (j = 0; j < 3; ++j)
									{
										move[j] = tempMove[j];
									}
								}
							}
						}
						break;
					}
				}
				path = path->next;
			}
		}
		if (board[y][x]->right == color && board[y][x+1] == NULL)
		{
			a = getAvailableTileTypes(y,x+1);
			if (a[7])
			{
				for (i = 1; i < 7; ++i)
				{
					if (a[i])
					{
						tmpLen = getRecBestMove(y, x, y , x + 1, i, tempMove, search_lvl, destination, color);
						if (tmpLen < bestLen)
						{
							bestLen = tmpLen;
							for (j = 0; j < 3; ++j)
							{
								move[j] = tempMove[j];
							}
						}
						if ((type == 1 || type == 4) && tmpLen < 9999 && tempMove[2] != 1 && tempMove[2] != 4)
						{
							bestLen = tmpLen - 1;
							move[0] = y;
							move[1] = x;
							move[2] = type;
						}
						break;
					}
				}
			}
			else if (a[0])
			{
				for (i = 1; i < 7; ++i)
				{
					if (a[i])
					{
						tmpLen = getRecBestMove(y, x, y , x + 1, i, tempMove, search_lvl-1, destination, color) + 1;
						if (tmpLen < bestLen)
						{
							bestLen = tmpLen;
							for (j = 0; j < 3; ++j)
							{
								move[j] = tempMove[j];
							}
						}
					}
				}
			}
		} else if(board[y][x+1] != NULL && board[y][x]->right == color && (y != bef_y || x+1 != bef_x))
		{
			path = (color == WHITE) ? whitePathes->first : blackPathes->first;
			while(path != NULL)
			{
				if (path->nodes[0].y == y && path->nodes[0].x == x+1 )
				{
					if (path->nodes[1].arrow == Top)
					{
						a = getAvailableTileTypes(path->nodes[1].y - 1, path->nodes[1].x);
						for (i = 1; i < 7; ++i)
						{
							if (a[i])
							{
								tmpLen = getRecBestMove(path->nodes[1].y, path->nodes[1].x, path->nodes[1].y - 1, path->nodes[1].x, i, tempMove, search_lvl-1, destination, color) + 1;
								if (tmpLen < bestLen)
								{
									bestLen = tmpLen;
									for (j = 0; j < 3; ++j)
									{
										move[j] = tempMove[j];
									}
								}
							}
						}
						break;
					} else if (path->nodes[1].arrow == Bottom)
					{
						a = getAvailableTileTypes(path->nodes[1].y + 1, path->nodes[1].x);
						for (i = 1; i < 7; ++i)
						{
							if (a[i])
							{
								tmpLen = getRecBestMove(path->nodes[1].y, path->nodes[1].x, path->nodes[1].y + 1, path->nodes[1].x, i, tempMove, search_lvl-1, destination, color) + 1;
								if (tmpLen < bestLen)
								{
									bestLen = tmpLen;
									for (j = 0; j < 3; ++j)
									{
										move[j] = tempMove[j];
									}
								}
							}
						}
						break;
					} else if (path->nodes[1].arrow == Left)
					{
						a = getAvailableTileTypes(path->nodes[1].y, path->nodes[1].x - 1);
						for (i = 1; i < 7; ++i)
						{
							if (a[i])
							{
								tmpLen = getRecBestMove(path->nodes[1].y, path->nodes[1].x, path->nodes[1].y, path->nodes[1].x - 1, i, tempMove, search_lvl-1, destination, color) + 1;
								if (tmpLen < bestLen)
								{
									bestLen = tmpLen;
									for (j = 0; j < 3; ++j)
									{
										move[j] = tempMove[j];
									}
								}
							}
						}
						break;
					} else {
						a = getAvailableTileTypes(path->nodes[1].y, path->nodes[1].x + 1);
						for (i = 1; i < 7; ++i)
						{
							if (a[i])
							{
								tmpLen = getRecBestMove(path->nodes[1].y, path->nodes[1].x, path->nodes[1].y, path->nodes[1].x + 1, i, tempMove, search_lvl-1, destination, color) + 1;
								if (tmpLen < bestLen)
								{
									bestLen = tmpLen;
									for (j = 0; j < 3; ++j)
									{
										move[j] = tempMove[j];
									}
								}
							}
						}
						break;
					}
				} else if (path->nodes[1].y == y && path->nodes[1].x == x+1 )
				{
					if (path->nodes[0].arrow == Top)
					{
						a = getAvailableTileTypes(path->nodes[0].y - 1, path->nodes[0].x);
						for (i = 1; i < 7; ++i)
						{
							if (a[i])
							{
								tmpLen = getRecBestMove(path->nodes[0].y, path->nodes[0].x, path->nodes[0].y - 1, path->nodes[0].x, i, tempMove, search_lvl-1, destination, color) + 1;
								if (tmpLen < bestLen)
								{
									bestLen = tmpLen;
									for (j = 0; j < 3; ++j)
									{
										move[j] = tempMove[j];
									}
								}
							}
						}
						break;
					} else if (path->nodes[0].arrow == Bottom)
					{
						a = getAvailableTileTypes(path->nodes[0].y + 1, path->nodes[0].x);
						for (i = 1; i < 7; ++i)
						{
							if (a[i])
							{
								tmpLen = getRecBestMove(path->nodes[0].y, path->nodes[0].x, path->nodes[0].y + 1, path->nodes[0].x, i, tempMove, search_lvl-1, destination, color) + 1;
								if (tmpLen < bestLen)
								{
									bestLen = tmpLen;
									for (j = 0; j < 3; ++j)
									{
										move[j] = tempMove[j];
									}
								}
							}
						}
						break;
					} else if (path->nodes[0].arrow == Left)
					{
						a = getAvailableTileTypes(path->nodes[0].y, path->nodes[0].x - 1);
						for (i = 1; i < 7; ++i)
						{
							if (a[i])
							{
								tmpLen = getRecBestMove(path->nodes[0].y, path->nodes[0].x, path->nodes[0].y, path->nodes[0].x - 1, i, tempMove, search_lvl-1, destination, color) + 1;
								if (tmpLen < bestLen)
								{
									bestLen = tmpLen;
									for (j = 0; j < 3; ++j)
									{
										move[j] = tempMove[j];
									}
								}
							}
						}
						break;
					} else {
						a = getAvailableTileTypes(path->nodes[0].y, path->nodes[0].x + 1);
						for (i = 1; i < 7; ++i)
						{
							if (a[i])
							{
								tmpLen = getRecBestMove(path->nodes[0].y, path->nodes[0].x, path->nodes[0].y, path->nodes[0].x + 1, i, tempMove, search_lvl-1, destination, color) + 1;
								if (tmpLen < bestLen)
								{
									bestLen = tmpLen;
									for (j = 0; j < 3; ++j)
									{
										move[j] = tempMove[j];
									}
								}
							}
						}
						break;
					}
				}
				path = path->next;
			}
		}
		if (board[y][x]->left == color && board[y][x-1] == NULL)
		{
			a = getAvailableTileTypes(y,x-1);
			if (a[7])
			{
				for (i = 1; i < 7; ++i)
				{
					if (a[i])
					{
						tmpLen = getRecBestMove(y, x, y , x - 1, i, tempMove, search_lvl, destination, color);
						if (tmpLen < bestLen)
						{
							bestLen = tmpLen;
							for (j = 0; j < 3; ++j)
							{
								move[j] = tempMove[j];
							}
						}
						if ((type == 1 || type == 4) && tmpLen < 9999 && tempMove[2] != 1 && tempMove[2] != 4)
						{
							bestLen = tmpLen - 1;
							move[0] = y;
							move[1] = x;
							move[2] = type;
						}
						break;
					}
				}
			}
			else if (a[0])
			{
				for (i = 1; i < 7; ++i)
				{
					if (a[i])
					{
						tmpLen = getRecBestMove(y, x, y , x - 1, i, tempMove, search_lvl-1, destination, color) + 1;
						if (tmpLen < bestLen)
						{
							bestLen = tmpLen;
							for (j = 0; j < 3; ++j)
							{
								move[j] = tempMove[j];
							}
						}
					}
				}
			}
		} else if(board[y][x+1] != NULL && board[y][x]->left == color && (y != bef_y || x-1 != bef_x))
		{
			path = (color == WHITE) ? whitePathes->first : blackPathes->first;
			while(path != NULL)
			{
				if (path->nodes[0].y == y && path->nodes[0].x == x-1 )
				{
					if (path->nodes[1].arrow == Top)
					{
						a = getAvailableTileTypes(path->nodes[1].y - 1, path->nodes[1].x);
						for (i = 1; i < 7; ++i)
						{
							if (a[i])
							{
								tmpLen = getRecBestMove(path->nodes[1].y, path->nodes[1].x, path->nodes[1].y - 1, path->nodes[1].x, i, tempMove, search_lvl-1, destination, color) + 1;
								if (tmpLen < bestLen)
								{
									bestLen = tmpLen;
									for (j = 0; j < 3; ++j)
									{
										move[j] = tempMove[j];
									}
								}
							}
						}
						break;
					} else if (path->nodes[1].arrow == Bottom)
					{
						a = getAvailableTileTypes(path->nodes[1].y + 1, path->nodes[1].x);
						for (i = 1; i < 7; ++i)
						{
							if (a[i])
							{
								tmpLen = getRecBestMove(path->nodes[1].y, path->nodes[1].x, path->nodes[1].y + 1, path->nodes[1].x, i, tempMove, search_lvl-1, destination, color) + 1;
								if (tmpLen < bestLen)
								{
									bestLen = tmpLen;
									for (j = 0; j < 3; ++j)
									{
										move[j] = tempMove[j];
									}
								}
							}
						}
						break;
					} else if (path->nodes[1].arrow == Left)
					{
						a = getAvailableTileTypes(path->nodes[1].y, path->nodes[1].x - 1);
						for (i = 1; i < 7; ++i)
						{
							if (a[i])
							{
								tmpLen = getRecBestMove(path->nodes[1].y, path->nodes[1].x, path->nodes[1].y, path->nodes[1].x - 1, i, tempMove, search_lvl-1, destination, color) + 1;
								if (tmpLen < bestLen)
								{
									bestLen = tmpLen;
									for (j = 0; j < 3; ++j)
									{
										move[j] = tempMove[j];
									}
								}
							}
						}
						break;
					} else {
						a = getAvailableTileTypes(path->nodes[1].y, path->nodes[1].x + 1);
						for (i = 1; i < 7; ++i)
						{
							if (a[i])
							{
								tmpLen = getRecBestMove(path->nodes[1].y, path->nodes[1].x, path->nodes[1].y, path->nodes[1].x + 1, i, tempMove, search_lvl-1, destination, color) + 1;
								if (tmpLen < bestLen)
								{
									bestLen = tmpLen;
									for (j = 0; j < 3; ++j)
									{
										move[j] = tempMove[j];
									}
								}
							}
						}
						break;
					}
				} else if (path->nodes[1].y == y && path->nodes[1].x == x-1 )
				{
					if (path->nodes[0].arrow == Top)
					{
						a = getAvailableTileTypes(path->nodes[0].y - 1, path->nodes[0].x);
						for (i = 1; i < 7; ++i)
						{
							if (a[i])
							{
								tmpLen = getRecBestMove(path->nodes[0].y, path->nodes[0].x, path->nodes[0].y - 1, path->nodes[0].x, i, tempMove, search_lvl-1, destination, color) + 1;
								if (tmpLen < bestLen)
								{
									bestLen = tmpLen;
									for (j = 0; j < 3; ++j)
									{
										move[j] = tempMove[j];
									}
								}
							}
						}
						break;
					} else if (path->nodes[0].arrow == Bottom)
					{
						a = getAvailableTileTypes(path->nodes[0].y + 1, path->nodes[0].x);
						for (i = 1; i < 7; ++i)
						{
							if (a[i])
							{
								tmpLen = getRecBestMove(path->nodes[0].y, path->nodes[0].x, path->nodes[0].y + 1, path->nodes[0].x, i, tempMove, search_lvl-1, destination, color) + 1;
								if (tmpLen < bestLen)
								{
									bestLen = tmpLen;
									for (j = 0; j < 3; ++j)
									{
										move[j] = tempMove[j];
									}
								}
							}
						}
						break;
					} else if (path->nodes[0].arrow == Left)
					{
						a = getAvailableTileTypes(path->nodes[0].y, path->nodes[0].x - 1);
						for (i = 1; i < 7; ++i)
						{
							if (a[i])
							{
								tmpLen = getRecBestMove(path->nodes[0].y, path->nodes[0].x, path->nodes[0].y, path->nodes[0].x - 1, i, tempMove, search_lvl-1, destination, color) + 1;
								if (tmpLen < bestLen)
								{
									bestLen = tmpLen;
									for (j = 0; j < 3; ++j)
									{
										move[j] = tempMove[j];
									}
								}
							}
						}
						break;
					} else {
						a = getAvailableTileTypes(path->nodes[0].y, path->nodes[0].x + 1);
						for (i = 1; i < 7; ++i)
						{
							if (a[i])
							{
								tmpLen = getRecBestMove(path->nodes[0].y, path->nodes[0].x, path->nodes[0].y, path->nodes[0].x + 1, i, tempMove, search_lvl-1, destination, color) + 1;
								if (tmpLen < bestLen)
								{
									bestLen = tmpLen;
									for (j = 0; j < 3; ++j)
									{
										move[j] = tempMove[j];
									}
								}
							}
						}
						break;
					}
				}
				path = path->next;
			}
		}
		board[y][x] = NULL;
		free(a);
		free(b);
		free(c);
		free(d);
		return bestLen;

	}

	void reParsemove(int y, int x, int type, char ret[])
	{
		int i, j, m = 0, n = 0, k;
		i = y - Y_INDEX + 1;
		j = x - X_INDEX + 1;
		if (j == 0)
		{
			ret[m++] = '@';
		} else {
			j--;
			if (j / 26 > 0)
			{
				ret[m++] = j / 26 + '@';
				ret[m++] = j % 26 + 'A';
			} else {
				ret[m++] = j + 'A';
			}
		}

		if (i == 0)
		{
			ret[m++] = '0';
		} else {
			if (i/100 >= 1)
			{
				ret[m++] = i / 100 + '0';
				i %= 100;
			}
			if(i/10 >= 1)
			{
				ret[m++] = i / 10 + '0';
				i %= 10;
			}
			ret[m++] = i + '0';
		}
		type = (type > 3) ? type - 3 : type;
		switch ( type )
		{
			case 1:
				ret[m++] = '+';
				break;
			case 2:
				ret[m++] = '/';
				break;
			case 3:
				ret[m++] = '\\';
				break;
			default:
				ret[m++] = '\\';
				break;
		}
		ret[m] = '\n';
		for (k = m+1; k < 10; ++k)
		{
			ret[k] = 0;
		}
	}

	void AI(int lenW,int y_w,int x_w,int m_w,int lenB,int y_b,int x_b,int m_b,int* res_mov,int color)
	{
		int* a, i;
		if (!hasMoved)
		{
			res_mov[0] = Y_INDEX - 1;
			res_mov[1] = X_INDEX - 1;
			res_mov[2] = 1;
		}else
		{
			if (color == BLACK)
			{
				if (lenB < lenW || lenB == 1)
				{
					res_mov[0] = y_b;
					res_mov[1] = x_b;
					res_mov[2] = m_b;
				} else {
					a = getAvailableTileTypes(y_w, x_w);
					for (i = 1; i < 7; ++i)
					{
						if (a[i] &&  i != m_w)
						{
							res_mov[0] = y_w;
							res_mov[1] = x_w;
							res_mov[2] = i;
							break;
						}
					}
					for (i = 1; i < 7; ++i)
					{
						if (a[i] && i != m_w && i != 1 && i != 4)
						{
							res_mov[0] = y_w;
							res_mov[1] = x_w;
							res_mov[2] = i;
							break;
						}
					}
				}
			} else {
				if (lenW < lenB || lenW == 1)
				{
					res_mov[0] = y_w;
					res_mov[1] = x_w;
					res_mov[2] = m_w;
				} else {
					a = getAvailableTileTypes(y_b, x_b);
					for (i = 1; i < 7; ++i)
					{
						if (a[i] && i != m_b)
						{
							res_mov[0] = y_b;
							res_mov[1] = x_b;
							res_mov[2] = i;
							break;
						}
					}
					for (i = 1; i < 7; ++i)
					{
						if (a[i] && i != m_b && i != 1 && i != 4)
						{
							res_mov[0] = y_b;
							res_mov[1] = x_b;
							res_mov[2] = i;
							break;
						}
					}
				}
			}
		}
	}
/*/Functions*/

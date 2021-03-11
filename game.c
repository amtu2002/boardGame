#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LINE_SIZE 100

int height; int width;
char commands[] = {'a', 'd', 'w', 's', 'q'} ;
char playerIcon = 'A'; char powerIcon = 'B'; char enemyIcon = 'X';
int building = 0; int enemy = 0; int items = 0; int powerOrb = 0; int playerStart[2];

void deallocateBoard(char*** board, int height)
{
	for(int y = 0; y < height; y++)
		free((*board)[y]);
	free(*board);
}

void printBoard(char*** board, int width, int height)
{
	for(int y = 0; y < height;y++)
	{
		for(int x = 0; x < width; x++)
		{
			printf("%c", (*board)[y][x]);
			if(x == width-1)
				printf("\n");
		}
	}
}

void allocateBoard(char*** board, int width, int height)
{
	*board = (char**)malloc(height*sizeof(char*));
	for(int y = 0; y < height;y++)
		(*board)[y] = (char*)malloc(width*sizeof(char));
	for( int i = 0; i < height; i++)
		memset((*board)[i], ' ', width);
}

void readInput(const char* filename, char*** board)
{
	char letter;
	int h;
	int w;
	int n = 0;
	char buf[MAX_LINE_SIZE];
	int isBlank = 0;
	FILE* gameOutline = fopen(filename,"r");
	if(n == 0)
	{
		fscanf(gameOutline, "%d %d\n", &height, &width);
		height+=2; width+=2;
		n++;
	}

	allocateBoard(board, width, height);

	if(n == 1)
	{
		fgets(buf,MAX_LINE_SIZE, gameOutline);
		if(buf[4] >= 48 && buf[4] <= 57)
		{
			isBlank = 1;
			n++;
		}
		else
		{
			char* tok = strtok(buf," ");
			commands[0] = *tok;

			int i = 1;
			while(tok != NULL)
			{	
	 			tok = strtok(NULL, " ");
	 			if(tok == NULL)
	 				break;
	 			commands[i] = *tok;
	 			i++;
	 		}
			n++;
		}	
	}

	if(n == 2)
	{
		// blank line condition
		if(isBlank == 1)
		{
			playerIcon = buf[0];
			powerIcon = buf[2];
			playerStart[1] = atoi(&buf[4]) + 1;
			playerStart[0] = atoi(&buf[6]) + 1;
			(*board)[playerStart[1]][playerStart[0]] = playerIcon;
			
		}
		else{
			fscanf(gameOutline, "%c %c %d %d", &playerIcon, &powerIcon, &playerStart[1], &playerStart[0]);
			playerStart[0]++;
      		playerStart[1]++;
      		(*board)[playerStart[1]][playerStart[0]] = playerIcon;
		}
	}

	for(int y = 0; y < height; y++)
	{
		for(int x = 0; x < width; x++)
		{
			if(y == 0 || y == height - 1)
				(*board)[y][x] = '*';
			else if(x == 0 || x == width-1)
				(*board)[y][x] = '*';
			else if(x == playerStart[0] && y == playerStart[1])
        		(*board)[y][x] = playerIcon;
		}
	}

	while(fscanf(gameOutline, "%c %d %d\n", &letter, &h, &w) != EOF)
    {
        if(letter == 'B')
        {
        	for(int i = h+1; i< h+5;i++)
        	{
        		for(int j = w+1; j < w+7;j++)
        		{
        			if(i >= h+3 && i <= h+4 && j >= w+3 && j <= w+4)
        				(*board)[i][j] = '&';
        			else
        				(*board)[i][j] = '-';
        		}
        	}
        }
        if(letter == 'E')
        	(*board)[h+1][w+1] = 'X';
        if(letter == 'I')
        {
        	(*board)[h+1][w+1] = '$';
        	items++;
        }
        if(letter == 'P')
        {
        	(*board)[h+1][w+1] = '*';
        	powerOrb++;
        }
    }
    fclose(gameOutline);
}

int playGame(const char* filename)
{
	if(!filename)
		return 0;
	char** board;
	int itemsGotten = 0;
	int orbsGotten = 0;
	int score = 0;
	char input = 'f';
	int p = 0;
	int powerModeLeft = 0;
	int isPowerMode = 0;
	readInput(filename,&board);
	while(p == 0)
	{
		if(isPowerMode == 1)
		{
			board[playerStart[1]][playerStart[0]] = powerIcon;
			powerModeLeft++;
			if(powerModeLeft == 8)
			{
				board[playerStart[1]][playerStart[0]] = playerIcon;
				isPowerMode = 0;
			}
		}
		
		else
			board[playerStart[1]][playerStart[0]] = playerIcon;
		printf("Score: %d\n",score);
		printf("Items remaining: %d\n",items);
		printBoard(&board,width,height);

		int valid = 0;
		while(valid == 0)
		{
			printf("Enter input: ");
			scanf(" %c", &input);
			for(int i = 0; i < 5; i++)
			{
				if(input == commands[i])
				{
					valid = 1;
					break;
				}

			}
			if(valid == 0)
				printf("Invalid input.\n");

			else
				break;
		}
		for(int i = 0; i < 5; i++)
		{
			if(input == commands[i])
			{
				//left
				if(i == 0)
				{
					//boarder
					if(playerStart[0]-1 == 0)
						break;
					//building
					else if(board[playerStart[1]][playerStart[0]-1] == '-')
						break;
					//power mode
					else if(board[playerStart[1]][playerStart[0]-1] == '*')
					{
						board[playerStart[1]][playerStart[0]] = ' ';
						playerStart[0]--;
						goto power;
					}
					//enemy in power mode
					else if(isPowerMode == 1 && board[playerStart[1]][playerStart[0]-1] == enemyIcon)
					{
						if(powerModeLeft == 7)
						{
							board[playerStart[1]][playerStart[0]] = ' ';
							playerStart[0]--;
							goto killed;
						}
						board[playerStart[1]][playerStart[0]] = ' ';
						playerStart[0]--;
						score++;
						break;
					}
					//enemy not in power mode
					else if(playerIcon != powerIcon && board[playerStart[1]][playerStart[0]-1] == enemyIcon)
					{
						board[playerStart[1]][playerStart[0]] = ' ';
						playerStart[0]--;
						board[playerStart[1]][playerStart[0]] = '@';
						goto killed;
					}
					//item
					else if(board[playerStart[1]][playerStart[0]-1] == '$')
					{
						board[playerStart[1]][playerStart[0]] = ' ';
						playerStart[0]--;
						goto gotItem;
					}
					else 
					{
						board[playerStart[1]][playerStart[0]] = ' ';
						playerStart[0]--;
						break;
					}
				}
				//right
				if(i == 1)
				{
					if(playerStart[0]+1 == width-1)
						break;
					//building
					else if(board[playerStart[1]][playerStart[0]+1] == '-')
						break;
					//power mode
					else if(board[playerStart[1]][playerStart[0]+1] == '*')
					{
						board[playerStart[1]][playerStart[0]] = ' ';
						playerStart[0]++;
						goto power;
					}
					//enemy in power mode
					else if(isPowerMode == 1 && board[playerStart[1]][playerStart[0]+1] == enemyIcon)
					{
						if(powerModeLeft == 7)
						{
							board[playerStart[1]][playerStart[0]] = ' ';
							playerStart[0]++;
							goto killed;
						}
						board[playerStart[1]][playerStart[0]] = ' ';
						playerStart[0]++;
						score++;
						break;
					}
					//enemy not in power mode
					else if(playerIcon != powerIcon && board[playerStart[1]][playerStart[0]+1] == enemyIcon)
					{
						board[playerStart[1]][playerStart[0]] = ' ';
						playerStart[0]++;
						board[playerStart[1]][playerStart[0]] = '@';
						goto killed;
					}
					//item
					else if(board[playerStart[1]][playerStart[0]+1] == '$')
					{
						board[playerStart[1]][playerStart[0]] = ' ';
						playerStart[0]++;
						goto gotItem;
					}
					else
					{
						board[playerStart[1]][playerStart[0]] = ' ';
						playerStart[0]++;
						break;
					}
									
				}
				//up
				if(i == 2)
				{
					if(playerStart[1]-1 == 0)
						break;
					//building
					else if(board[playerStart[1]-1][playerStart[0]] == '-' || board[playerStart[1]-1][playerStart[0]] == '&' )
						break;
					//power mode
					else if(board[playerStart[1]-1][playerStart[0]] == '*')
					{
						board[playerStart[1]][playerStart[0]] = ' ';
						playerStart[1]--;
						goto power;
					}
					//enemy in power mode
					else if(isPowerMode == 1 && board[playerStart[1]-1][playerStart[0]] == enemyIcon)
					{
						if(powerModeLeft == 7)
						{
							board[playerStart[1]][playerStart[0]] = ' ';
							playerStart[1]--;
							goto killed;
						}
						board[playerStart[1]][playerStart[0]] = ' ';
						playerStart[1]--;
						score++;
						break;
					}
					//enemy not in power mode
					else if(playerIcon != powerIcon && board[playerStart[1]-1][playerStart[0]] == enemyIcon)
					{
						board[playerStart[1]][playerStart[0]] = ' ';
						playerStart[1]--;
						board[playerStart[1]][playerStart[0]] = '@';
						goto killed;
					}
					//items
					else if(board[playerStart[1]-1][playerStart[0]] == '$')
					{
						board[playerStart[1]][playerStart[0]] = ' ';
						playerStart[1]--;
						goto gotItem;
					}
					else
					{
						board[playerStart[1]][playerStart[0]] = ' ';
						playerStart[1]--;
						break;
					}
				}
				//down
				if(i == 3)
				{
					if(playerStart[1]+1 == height-1)
						break;
					else if(board[playerStart[1]+1][playerStart[0]] == '-')
						break;
					//power mode
					else if(board[playerStart[1]+1][playerStart[0]] == '*')
					{
						if(powerModeLeft == 7)
						{
							board[playerStart[1]][playerStart[0]] = ' ';
							playerStart[1]++;
							goto killed;
						}
						board[playerStart[1]][playerStart[0]] = ' ';
						playerStart[1]++;
						goto power;
					}
					//enemy in power mode
					else if(isPowerMode == 1 && board[playerStart[1]+1][playerStart[0]] == enemyIcon)
					{
						board[playerStart[1]][playerStart[0]] = ' ';
						playerStart[1]++;
						score++;
						break;
					}
					// enemy not in power mode
					else if(playerIcon != powerIcon && board[playerStart[1]+1][playerStart[0]] == enemyIcon)
					{
						board[playerStart[1]][playerStart[0]] = ' ';
						playerStart[1]++;
						board[playerStart[1]][playerStart[0]] = '@';
						goto killed;
					}
					//items
					else if(board[playerStart[1]+1][playerStart[0]] == '$')
					{
						board[playerStart[1]][playerStart[0]] = ' ';
						playerStart[1]++;
						goto gotItem;
					}
					else
					{
						board[playerStart[1]][playerStart[0]] = ' ';
						playerStart[1]++;
						break;
					}
				}
				//quit
				if(i == 4)
				{
					printf("You have quit.\n");
					printf("Final score: %d\n", score);
					p = 1;
					break;
				}

				killed:
					board[playerStart[1]][playerStart[0]] = '@';
					printf("Score: %d\n",score);
					printf("Items remaining: %d\n",items);
					printBoard(&board, width, height);
					printf("You have died.\n");
					printf("Final score: %d\n", score);
					p = 1;
					break;
				gotItem:
					items--;
					itemsGotten++;
					score++;
					if(items == 0)
						goto success;
					break;
				success:
					printf("Congratulations! You have won.\nFinal score: %d\n", score);
					p = 1;
					break;
				power:
					orbsGotten++;
					powerModeLeft = 0;
					isPowerMode = 1;
					break;

			}
		}
	}
	deallocateBoard(&board,height);
	return 1;
}


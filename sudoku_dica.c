/*

Projeto 1 - Sistemas Operacinais 1s2014
Sudoku multithread - modulo de dicas
Entrada: matriz 9x9 de caracteres, '1' a '9' pra representar uma posiçao ocupada,
ou 'x' para representar uma posicao vazia
Saida: tabuleiro com cada posicao vazia preenchida com os numeros que podem 
ser colocados naquela posicao, respeitando as regras do Sudoku.

Author: Leonardo de Oliveira Silva RA108330
*/

//Inclusao de bibliotecas
#include <stdio.h>
#include <stdlib.h>
//Tamanho do tabuleiro
#define SIZE 9
//Booleanas
#define TRUE 1
#define FALSE 0
//numero de threads para esse modulo
#define N_THR 9

//variavel global que representa o tabuleiro
int **tab;

//Definicao da estrutura para uma lista ligada de posicoes do tabuleiro
//que mais tarde servira como auxiliar para resolver o problema.
typedef struct position
{
	int row, col;
	struct position *next;
} Position, *PositionList;

//Este 'e um vetor de lista de posicoes, cada posicao i do vetor conter'a uma lista
//que representa as posicoes onde o numero (i+1) pode ser colocado.
//Exemplo desse vetor:
// 0 : (0,0)->(2,4)->(4,5)->(3,3)->NULL
// 1 : (1,3)->(2,2)->(7,5)->NULL
// 2 : NULL
// 3 : (5,4)->NULL
// 4 : (2,2)->NULL
// 5 : (4,3)->(3,2)->NULL
// 6 : (1,1)->NULL
// 7 : (2,3)->NULL
// 8 : (2,1)->NULL
//
// O vetor acima indica que o numero 4 cabe na posicao (5,4) e o numero 3 nao cabe em nenhuma posicao.
PositionList *p; 
//Inicializa vetor de lista de posicoes.
void initPosList()
{
	p = (PositionList*)malloc(sizeof(PositionList)*SIZE);
	int i;
	for (i = 0; i < SIZE; ++i)
	{
		p[i] = NULL;
	}
}
//Cria um n'o da lista de posicoes e o retorna, o n'o criado representa a posicao (i,j)
Position *createPosition(int i, int j)
{
	Position *p = (Position*)malloc(sizeof(Position));
	p->row = i;
	p->col = j;
	p->next = NULL;
	return p;
}
//Libera a regiao de memoria alocada pelo tabuleiro
void freeTab()
{
	int i;
	for (i = 0; i < SIZE; ++i)
	{
		free(tab[i]);
	}
	free(tab);
}
//Esta funcao retorna TRUE se num pode ser colocado na posicao
//(row,col) respeitando-se as regras do jogo
int isPositionOk(int num, int row, int col)
{
	//Variaveis que representam as coordenadas do inicio de uma
	//caixa 3x3 dadas uma linha e uma coluna. Por exemplo:
	//a posicao (7,8) est'a na caixa que comeca na linha
	//boxStartRow = (7/3)*3 = 2*3 = 6, e coluna
	//boxStartCol = (8/3)*3 = 2*3 = 6.
    int boxStartRow = (row/3)*3;
    int boxStartCol = (col/3)*3;
    //loop que verifica se colocar "num" na posicao (row,col)
    //continua respeitando as regras do jogo.
    int i,j;
    for(i = 0; i < SIZE; ++i)
    {
    	//Se o numero ja esta na caixa, na coluna, ou na linha,
    	//entao a funcao retorna FALSE, pois uma das regras ser'a
    	//quebrada
        if (tab[row][i] == num || tab[i][col] == num 
        	|| tab[boxStartRow+(i%3)][boxStartCol+(i/3)] == num)
			return FALSE;
    }
    //cenario de sucesso: o numero pode ser colocado na posicao sem quebrar as regras.
    return TRUE;
}
//Funcao principal de uma thread: recebe como entrada um numero de 0 a 8 e constroi 
//o vetor de lista de posicoes para os numeros dado um certo tabuleiro
void *buildHints(void * nThr)
{
	//o id da thread 'e o numero-1
	int num = (int)nThr;
	//incrementa o numero para construir a lista
	num++;
	//loop que percorre o tabuleiro verificando se o n'umero pode ser
	//colocado em uma posicao vazia
	int i, j;
	for (i = 0; i < SIZE; ++i)
	{
		for (j = 0; j < SIZE; ++j)
		{
			//se a posicao esta vazia...
			if (tab[i][j] == 0)
			{
				//...checa se o numero pode ser colocado nela
				if (isPositionOk(num,i,j))
				{
					//se sim, entao adiciona um n'o da lista na posicao
					//num-1 do vetor de listas
					Position *pos = createPosition(i,j);
					pos->next = p[num-1];
					p[num-1] = pos;
				}
			}
		}
	}
	//finaliza execucao da thread
	return NULL;
}
//Funcao que exibe na saida padrao o tabuleiro com as dicas posicionadas
void printHints()
{
	int i,j;
	for (i = 0; i < SIZE; ++i)
    {
    	for (j = 0; j < SIZE; ++j)
    	{
    		//se a posicao for vazia, uma busca 'e feita no vetor
    		//para procurar quais sao os possiveis numeros que podem
    		//ser colocados nela, os valores sao imprimidos entre parenteses
    		if (tab[i][j] == 0)
    		{
    			printf("(");
    			int k;
    			for (k = 0; k < SIZE; ++k)
    			{
    				PositionList list = p[k];
    				while(list)
    				{
    					if (list->row == i && list->col == j)
    					{
    						printf("%d",k+1);
    					}
    				list = list->next;
    				}
    			}
    			printf(") ");
    		}
    			//caso contrario, exibe o numero que esta no tabuleiro
    			else
    			{
    				printf("%d ",tab[i][j]);
    			}
    	}
    	printf("\n");
    }
}

int main()
{
	//vetor de threads
	pthread_t thr[N_THR];
	//tabuleiro
	tab = (int**)malloc(sizeof(int*)*SIZE);
	//aloca o tabuleiro
	int i,j;
	for (i = 0; i < SIZE; ++i)
	{
		tab[i] = (int*)malloc(sizeof(int)*SIZE);
	}
	//le o tabuleiro da entrada padrao
	char ch;
	for (i = 0; i < SIZE; ++i)
	{
		for (j = 0; j < SIZE; ++j)
		{
			scanf(" %c",&ch);
			//conversao de char para numero inteiro
			if (ch >= '1' && ch <= '9')
			{
				tab[i][j] = ch - '0';
			}
				//se nao 'e um digito, entao 'e uma posicao vazia
				else
				{
					tab[i][j] = 0;	
				}
		}
	}
	//inicializa vetor de listas
	initPosList();
	//cria as threads, cada thread ira procurar onde o numero (i+1) pode ser colocado
	//respeitando-se as regras do sudoku
	for (i = 0; i < N_THR; ++i)
	{
		pthread_create(&thr[i], NULL, buildHints, (void*) i);
	}
	//dispara as threads
	for (i = 0; i < N_THR; i++) 
    {
    	pthread_join(thr[i], NULL); 
    }
    //imprime as dicas
    printHints();
    //libera o tabuleiro
	freeTab();
	//finaliza
	return 0;
}

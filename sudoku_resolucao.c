/*

Projeto 1 - Sistemas Operacinais 1s2014
Sudoku multithread - modulo de resolucao
Entrada: matriz 9x9 de caracteres, '1' a '9' pra representar uma posiçao ocupada,
ou 'x' para representar uma posicao vazia
Saida: tabuleiro preenchido com a solucao encontrada.

Author: Leonardo de Oliveira Silva RA108330
*/

//Inclusao de bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
//Tamanho do tabuleiro
#define SIZE 9
//Booleanas
#define TRUE 1
#define FALSE 0
//Número máximo de threads para este módulo
#define N_THR 9
//matriz do tabuleiro
int **tab;

//Esta funcao retorna TRUE se num pode ser colocado na posicao
//(row,col) respeitando-se as regras do jogo
int isPositionOk(int **tab, int num, int row, int col)
{
	//Variaveis que representam as coordenadas do inicio de uma
	//caixa 3x3 dadas uma linha e uma coluna. Por exemplo:
	//a posicao (7,8) est'a na caixa que comeca na linha
	//boxStartRow = (7/3)*3 = 2*3 = 6, e coluna
	//boxStartCol = (8/3)*3 = 2*3 = 6.
    int boxStartRow = (row/3)*3;
    int boxStartCol = (col/3)*3;
    int i,j;
    //loop que verifica se colocar "num" na posicao (row,col)
    //continua respeitando as regras do jogo.
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

//Funçao que procura uma posiçao vazia no tabuleiro
//se encontrar, retorna TRUE e os valores de row e col
//sao alterados para a linha e a coluna da posicao vazia
int emptyPosition(int **tab, int *row, int *col)
{
	int i, j;
	for (i = 0; i < SIZE; ++i)
	{
		for (j = 0; j < SIZE; ++j)
		{
			if (tab[i][j] == 0)
			{
				*row = i;
				*col = j;
				return TRUE;
			}
		}
	}
	//nao encontrou nenhuma posicao vazia
	return FALSE;
}
//Funçao que resolve um tabuleiro do sudoku
int solveSudoku(int **tab)
{
	int row, col;
	//se nao houver mais posiçoes vazias 
	if(!emptyPosition(tab,&row,&col))
	{
		//o tabuleiro foi resolvido
		return TRUE;
	}
	else 
	{
		//caso contrario, este loop tentará posicionar 
		//os numeros de 1 a 9 na posicao
		int num;
		for (num = 1; num <= SIZE; ++num)
		{
			//se o numero pode ser colocado nesta posicao
			if (isPositionOk(tab,num,row,col))
			{
				//tenta resolver recursivamente com o numero na posicao
				tab[row][col] = num;
				if (solveSudoku(tab))
				{
					return TRUE;
				}
					//se nao, esvazia a posicao e passa para a proxima tentativa
					else
					{
						tab[row][col] = 0;
					}
			}
		}
		//se todas as tentativas se esgotarem, nao ha solucao.
		return FALSE;
	}
}
//Funçao principal de uma thread
//REcebe o ponteiro para um tabuleiro e tenta resolve-lo
void *solveThr(void *tabThr)
{
	int **tab;
	tab = (int**)tabThr;
	int i,j;
	//se o tabuleiro foi resolvido, imprime soluçao na saída padrão
	if (solveSudoku(tab))
	{
		for (i = 0; i < SIZE; ++i)
		{
			for (j = 0; j < SIZE; ++j)
			{
				printf("%2d",tab[i][j]);
			}
			printf("\n");
		}	
	}
	//finaliza a execucao, matando as outras threads.
	return NULL;

}

int main()
{
	int i,j;
	pthread_t thr[N_THR];
	tab = (int**)malloc(sizeof(int*)*SIZE);
	//aloca'cao dinamica
	for (i = 0; i < SIZE; ++i)
	{
		tab[i] = (int*)malloc(sizeof(int)*SIZE);
	}
	//leitura pela entrada padrao
	char ch;
	for (i = 0; i < SIZE; ++i)
	{
		for (j = 0; j < SIZE; ++j)
		{
			scanf(" %c",&ch);
			if (ch >= '1' && ch <= '9')
			{
				tab[i][j] = ch - '0'; //conversao de char pra int
			}
				else
				{
					tab[i][j] = 0;	
				}
		}
	}
	//possibleNumbers: vetor booleano, se possibleNumbers[i] = TRUE entao (i+1) cabe em uma posicao
	//nPossibleNumbers: numero de possiveis numeros para uma certa posicao
	int *possibleNumbers,nPossibleNumbers;
	//alocacao do vetor
	possibleNumbers = (int*)malloc(sizeof(int)*SIZE);
	//zera o vetor, no inicio todos sao FALSE
	for (i = 0; i < SIZE; ++i)
	{
		possibleNumbers[i] = 0;
	}
	//flag auxiliar para o loop
	int stop = FALSE;
	//variaveis para salvar a linha e a coluna vazia
	int emptyRow, emptyCol;
	//percorre o tabuleiro em busca de uma posicao vazia
	for (i = 0; i < SIZE && !stop; ++i)
	{
		for (j = 0; j < SIZE && !stop; ++j)
		{
			if (tab[i][j] == 0)
			{
				//muda a flag para parar o loop
				stop = TRUE;
				//salva a linha e a coluna da posicao vazia
				emptyCol = i;
				emptyRow = j;
				//zera a quantidade de numeros possiveis nessa posicao
				nPossibleNumbers = 0;
				int k;
				//loop para contar a quantidade de numeros possiveis nesta posicao
				//futuramente, este sera o numero de threads
				for (k = 0; k < SIZE; ++k)
				{
					if (isPositionOk(tab,k+1,i,j))
					{
						nPossibleNumbers++;
						possibleNumbers[k] = TRUE;
					}
				}
			}
		}
	}
	//percorre o vetor de numeros possiveis
	//criando uma thread para tentar solucionar o tabuleiro
	//com cada um dos numeros possiveis na regiao vazia.
	for (i = 0; i < SIZE; ++i)
	{
		if(possibleNumbers[i])
		{
			int **attempt;
			//aloca uma nova regiao de memoria para a tentativa
			attempt = (int**)malloc(sizeof(int*)*SIZE);
			for (j = 0; j < SIZE; ++j)
			{
				attempt[i] = (int*)malloc(sizeof(int)*SIZE);
			}
			//copia o tabuleiro para a variavel de tentativa
			memcpy(attempt,tab,SIZE*SIZE*sizeof(int));
			//coloca o numero da posicao vazia
			attempt[emptyRow][emptyCol] = i+1;
			//cria a thread para a tentativa
			pthread_create(&thr[i], NULL, solveThr, (void*) attempt);
			//inicializa a thread da tentativa
			pthread_join(thr[i], NULL); 
		}
	}
	free(possibleNumbers);
	return 0;
}

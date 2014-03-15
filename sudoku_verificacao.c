/*

Projeto 1 - Sistemas Operacionais 1s2014
Sudoku multithread - modulo de verificacao
Entrada: matriz 9x9 de numeros inteiros
Saida: primeiro(s) erro(s) encontrado(S)

Author: Leonardo de Oliveira Silva RA108330
*/

//Inclusao de bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
//Tamanho do tabuleiro
#define SIZE 9
//Numero de threads utilizadas neste modulo
#define N_THR 27
//Constantes booleanas
#define TRUE 1
#define FALSE 0
//Variavel global para representacao do tabuleiro
int **tab;


//Funcao que libera a regiao de memoria alocada para o tabuleiro
void freeTab()
{
	int i;
	for (i = 0; i < SIZE; ++i)
	{
		free(tab[i]);
	}
	free(tab);
}

//Função que verifica se uma das linhas está correta de acordo com as regras do sudoku
//Esta funcao recebe como argumento o id da thread que irá executá-la.
//O id pode ser um numero de 0 a 27-1
void *checkRow(void *thrRow)
{
	//o numero da linha a ser averiguada 'e o id da thread % SIZE
	//por exemplo, se a thread recebe 19 como id, a linha a ser checada 'e
	//a linha 1
	int row = ((int)thrRow)%SIZE;
	//inicializacao do vetor de ocorrencias
	//este vetor guarda na posicao i o numero de ocorrencias que i+1 ocorre na linha
	int *occurences = (int*)malloc(sizeof(int)*SIZE);
	int i;
	for (i = 0; i < SIZE; ++i)
	{
		occurences[i] = 0;
	}
	//loop que varre a linha contando as ocorrencias dos numeros de 1 a 9 na linha
	for(i = 0; i < SIZE; ++i)
	{
		//checa se o numero esta dentro do intervalo permitido
		if (tab[row][i] >= 1 && tab[row][i] <= SIZE)
		{
			//se estiver, conta uma ocorrencia do numero
			++occurences[tab[row][i]-1]; 
		} 
			else
			{
				//caso contrario, reporta um erro, libera o vetor e o tabuleiro, e finaliza a execucao
				//do programa, matando todas as outras threads.
				printf("Number out of range on row %d.\n",row+1);
				free(occurences);
				freeTab();
				exit(0);
			}
	}
	//loop que percorre o vetor de ocorrencias checando se ha mais de uma ocorrencia de 
	//um numero na linha
	for (i = 0; i < SIZE; ++i)
	{
		if (occurences[i] > 1)
		{
			//se houver, reporta o erro e finaliza a execucao do programa.
			printf("More than one occurence of number %d on row %d\n",i+1,row+1);
			free(occurences);
			freeTab();
			exit(0);
		}
	}
	//cenario de sucesso, nao houve erros
	//finaliza a execucao da thread liberando o vetor de ocorrencias 
	//e retornando null
	free(occurences);
	return NULL;
}

//Funcao que verifica se uma das colunas está correta de acordo com as regras do sudoku
//Esta funcao é totalmente análoga a anterior, mas ao invés de checar para
//uma linha, checa para uma coluna.
void *checkCol(void* thrCol)
{
	int col = ((int)thrCol)%SIZE;
	int *occurences = (int*)malloc(sizeof(int)*SIZE);
	int i;
	for (i = 0; i < SIZE; ++i)
	{
		occurences[i] = 0;
	}
	for(i = 0; i < SIZE; ++i)
	{
		if (tab[i][col] >= 1 && tab[i][col] <= SIZE)
		{
			++occurences[tab[i][col]-1]; 
		} 
			else
			{
				printf("Number out of range on column %d.\n",col+1);
				free(occurences);
				freeTab();
				exit(0);
			}
	}
	for (i = 0; i < SIZE; ++i)
	{
		if (occurences[i] > 1)
		{
			printf("More than one occurence of number %d on column %d\n",i+1,col+1);
			free(occurences);
			freeTab();
			exit(0);
		}
	}
	free(occurences);
	return NULL;	
}
//Funcao que verifica se uma das caixas 3x3 esta correta de acordo com as regras do sudoku
//Foi considerado que as caixas estao numeradas de 1 a 9, de cima pra baixo, da esquerda pra 
//direita.
void *checkBox(void *thrBox)
{
	//O mesmo raciocinio das linhas e colunas, soma-se 1 pois
	//a numeracao é de 1 a 9 e nao de 0 a 8
	int box = (((int)thrBox)%SIZE)+1;
	//variavel auxiliar para encontrar qual é a caixa procurada
	int boxAux = 0;

	int boxStartCol, boxStartRow, keepGoing;
	keepGoing = TRUE;
	int i,j;
	for (i = 0; i < SIZE && keepGoing; i+=3)
	{
		for (j = 0; j < SIZE && keepGoing; j+=3)
		{
			boxAux++;
			//se essa é a caixa procurada entao salva onde a caixa começa
			if (box == boxAux)
			{
				boxStartCol = j;
				boxStartRow = i;
				keepGoing = FALSE;
			}
		}
	}
	//procura os erros na caixa, seguindo o mesmo procedimento das outras duas funçoes
	int *occurences = (int*)malloc(sizeof(int)*SIZE);
	for (i = 0; i < SIZE; ++i)
	{
		occurences[i] = 0;
	}
	for (i = 0; i < 3; ++i)
	{
		for (j = 0; j < 3; ++j)
		{
			if (tab[i+boxStartRow][j+boxStartCol] >= 1
				&& tab[i+boxStartRow][j+boxStartCol] <=9)
			{
				occurences[tab[i+boxStartCol][j+boxStartRow]-1]++;
			}
				else 
				{
					printf("Number out of range in box %d",box);
					free(occurences);
					freeTab();
					exit(0);
				}
		}
	}
	for (i = 0; i < SIZE; ++i)
	{
		if (occurences[i] > 1)
		{
			printf("More than one occurence of number %d in box %d\n",i,box);
			free(occurences);
			freeTab();
			exit(0);
		}
	}
	free(occurences);
	return NULL;
}

int main()
{
	pthread_t thr[N_THR];
	//alocaçao do tabuleiro
	tab = (int**)malloc(sizeof(int*)*SIZE);
	int i,j;
	for (i = 0; i < SIZE; ++i)
	{
		tab[i] = (int*)malloc(sizeof(int)*SIZE);
	}
	//leitura do tabuleiro
	for (i = 0; i < SIZE; ++i)
	{
		for (j = 0; j < SIZE; ++j)
		{
			scanf("%d",&tab[i][j]);
		}
	}
	//cria 27 threads
	//uma pra cada linha, coluna, e caixa
	//para procurar os possíveis erros no tabuleiro
	for (i = 0; i < N_THR; i++) 
	{
		if (i < 9)
		{
			pthread_create(&thr[i], NULL, checkRow, (void*) i);
		}
			else if (i < 18)
			{
				pthread_create(&thr[i], NULL, checkCol, (void*) i);
			}
				else
				{
					pthread_create(&thr[i], NULL, checkBox, (void*) i);
				}
	}
	//dispara as threads
    for (i = 0; i < N_THR; i++) 
    {
    	pthread_join(thr[i], NULL); 
    }
    //cenario de sucesso: se chegou até aqui, nenhuma thread encontrou erro
    printf("No errors were found.\n");
    //libera regiao de memória alocada para o tabuleiro
    freeTab();
	return 0;
}

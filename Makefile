all:
	gcc -g -pthread sudoku_verificacao.c -o sudoku_verificacao
	gcc -g -pthread sudoku_dica.c -o sudoku_dica
	gcc -g -pthread sudoku_resolucao.c -o sudoku_resolucao
clean:
	rm -rf *.o sudoku_resolucao sudoku_verificacao sudoku_dica

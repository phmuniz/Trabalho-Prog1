all:
	gcc -o prog trabalho.c

run: all
	./prog < Casos-v1/Gabarito/normais/02/mapa.txt >Casos-v1/Gabarito/normais/02/minha_saida.txt
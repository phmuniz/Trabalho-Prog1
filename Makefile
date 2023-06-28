all:
	gcc -o prog trabalho.c

run: all
	./prog < Casos-v1/Gabarito/simples/01/mapa.txt
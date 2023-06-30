all:
	gcc -o prog trabalho.c

run: all
	./prog < Casos-v1/Gabarito/simples/03/mapa.txt >Casos-v1/Gabarito/simples/03/minha_saida.txt

all: client.o ad_exchange.o dsp.o ssp.o
	gcc -o client client.o
	gcc -o ad_exchange ad_exchange.o
	gcc -o dsp dsp.o -lm
	gcc -o ssp ssp.o

client: client.o
	gcc -o client client.o
client.o: client.c
	gcc -c client.c

ad_exchange: ad_exchange.o
	gcc -o ad_exchange ad_exchange.o
ad_exchange.o: ad_exchange.c
	gcc -c ad_exchange.c

dsp: dsp.o
	gcc -o dsp dsp.o -lm
dsp.o: dsp.c
	gcc -c dsp.c

ssp: ssp.o
	gcc -o ssp ssp.o
ssp.o: ssp.c
	gcc -c ssp.c

bid: bid.o
	gcc -o bid bid.o -lm
bid.o: bid.c
	gcc -c bid.c
clean:
	rm client ad_exchange dsp ssp *.o

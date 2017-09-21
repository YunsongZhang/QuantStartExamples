simu:main.o asian.o payoff.o
	g++ main.o asian.o payoff.o -o simu
main.o:main.cpp asian.o payoff.o
	g++ -c main.cpp 
asian.o:asian.cpp
	g++ -c asian.cpp
payoff.o:payoff.cpp
	g++ -c payoff.cpp
clean:
	rm *.o simu


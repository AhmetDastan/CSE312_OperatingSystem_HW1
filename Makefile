All: compile clean

compile: main.cpp memory.cpp mmu.cpp
	@g++ main.cpp memory.cpp mmu.cpp -o exe -lpthread -lm

run:
	@./exe

clean: 
	@rm -f
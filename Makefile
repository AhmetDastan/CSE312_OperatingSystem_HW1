All: compile clean

compile: main.cpp memory.cpp mmu.cpp disk.cpp
	@g++ main.cpp memory.cpp mmu.cpp disk.cpp -o exe -lpthread -lm

run:
	@./exe

clean: 
	@rm -f
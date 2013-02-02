


NAME	:= UCShell
GCC	:= g++
CFLAGS  := -c -Wall
SRC	:= src/CS100HW-03.cpp
all: $(NAME)

# Tool invocations
$(NAME): $(SRC)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++  -o $(NAME) $(SRC) 
	@echo 'Finished building target: $@'
	@echo ' '



clean:
	rm UCShell






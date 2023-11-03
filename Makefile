build:
	gcc -o ./out/jvm ./src/main.c ./src/print_class.c ./src/class_structs.h
clean:
	rm -f main 

build:
	gcc -o ./out/jvm ./src/main.c ./src/print_class.c ./src/query_utils.c ./src/parser.c ./src/class_structs.h 
clean:
	rm -f main 

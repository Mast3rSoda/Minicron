obj = main.c task.c logging.c daemon.c signals.c
build : $(obj)
	gcc $(obj) -o minicron.o

clean:
	rm minicron.o

test:
		@./minicron.o test.txt out.txt

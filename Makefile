all: 
	make -C src/
	cp src/main main
clean:
	make clean -C src/
	rm main -f

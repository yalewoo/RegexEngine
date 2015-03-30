main:main.o lexicalAnalysis.o match.o nfa.o regexSearch.o
	gcc -o main main.o lexicalAnalysis.o match.o nfa.o regexSearch.o

main.o:header.h regexSearch.h match.h lexicalAnalysis.h nfa.h
lexicalAnalysis.o:header.h
match.o:header.h
nfa.o:header.h
regexSearch.h:header.h

.PHONY:clean
clean:
	rm main main.o lexicalAnalysis.o match.o nfa.o regexSearch.o
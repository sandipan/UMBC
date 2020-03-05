bison -y -d parse.y
flex scan.l
g++ -c y_tab.c lex.yy.c
g++ y_tab.o lex.yy.o -o assemble.exe
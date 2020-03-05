/* -------------------
		Sandipan Dey
		CS 611
----------------------*/
%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <vector>
#include <string>
/* prototypes */
unsigned int op;
unsigned int rs, rt, rd, fs, ft, fd;
unsigned int cop1 = 0x11, special = 0x00, fmt = 0;
int offset, base, instr_index;  
int yylex(void);
std::vector<int> objCode;
unsigned int objLine = 0;
extern FILE *yyin;
void yyerror(char *s);
%}

%token ADDRESS
%token GPR
%token FPR
%token DADD
%token DSUB
%token ADDD
%token SUBD
%token MULD
%token DIVD
%token LD
%token LW
%token SD
%token SW
%token J
%token BEQ
%token BNE

%%
ins_list:
          ins				  { ; }	
        | ins_list ins        { ; }
        ;

ins		:
		  dtins
		| aluins
		| ctlins
		;

dtins	: 
		  dtop dtopd						{ 
												printf("%5x R%02d F%02d %5d\n", op, base, rt, offset); 
												objLine = 0; objLine |= op << 26;	objLine |= base << 21;	objLine |= rt << 16; objLine |= offset;
												objCode.push_back(objLine);
											} 
		;
dtop	: 
		  LW								{ op = 0x23; }
		| LD								{ op = 0x37; }
		| SW								{ op = 0x2B; }
		| SD								{ op = 0x3F; }				
		;
dtopd	: 
		  FPR ',' ADDRESS '(' GPR ')' 		{ rt = $1; offset = $3; base = $5; }
		;
		
aluins	: 
		  alufins 
		| aluiins
		;
alufins	: 
		  alufop alufopd					{ 
												printf("%5x %5d F%02d F%02d F%02d %5x\n", cop1, fmt, ft, fs, fd, op); 
												objLine = 0; objLine |= cop1 << 26;	objLine |= fmt << 21;
												objLine |= ft << 16; objLine |= fs << 11; objLine |= fd << 6; objLine |= op;
												objCode.push_back(objLine);
											}
		;
alufop	: 
		  ADDD								{ op = 0x00; }
		| SUBD								{ op = 0x01; }				
		| MULD								{ op = 0x02; }
		| DIVD								{ op = 0x03; }
		;
alufopd	: 
		  FPR ',' FPR ',' FPR  				{ fd = $1; fs = $3; ft = $5; }
		;
aluiins	: 
		  aluiop aluiopd					{ 
												printf("%5x R%02d R%02d R%02d 0 %5x\n", special, rs, rt, rd, op); 
												objLine = 0; objLine |= special << 26;	
												objLine |= rs << 21; objLine |= rt << 16; objLine |= rd << 11; objLine |= op;
												objCode.push_back(objLine);
											}
		;
aluiop	: 
		  DADD								{ op = 0x2C; }
		| DSUB								{ op = 0x2E; }				
		;
aluiopd	: 
		  GPR ',' GPR ',' GPR  				{ rd = $1; rs = $3; rt = $5; }
		;
		
ctlins	: 
		  jins 
		| bins
		;
jins	: 
		  'J' ADDRESS						{ 
												op = 0x02;	instr_index = $2;
												printf("%5x %5d\n", op, instr_index);
												objLine = 0; objLine |= op << 26;	objLine |= instr_index;
												objCode.push_back(objLine);
											}
		;
bins	: 
		  bop bopd							{ 
												printf("%5x R%02d R%02d %5d\n", op, rs, rt, offset);
												objLine = 0; objLine |= op << 26; objLine |= rs << 21; objLine |= rt << 16;	objLine |= offset;
												objCode.push_back(objLine);
											}
		;
bop		: 
		  BEQ								{ op = 0x04; }
		| BNE								{ op = 0x05; }				
		;
bopd	: 
		  GPR ',' GPR ',' ADDRESS 			{ rs = $1; rt = $3; offset = $5; }
		;
%%

void yyerror(char *s) {
    fprintf(stdout, "%s\n", s);
}

std::string toBin(unsigned int n)
{
        std::string s;
        for (int i = 31; i >= 0; --i)
        {
            s += ((n >> i) & 0x01) ? "1" : "0";
        }
        return s;
}

int main(int argc, char *argv[]) {
	if (argc > 1)
	{
		char *file = argv[1];
		yyin = fopen(file, "r");
		if (yyin)
		{
			yyparse();
            char * pch = strtok (file, ".");
            if (pch != NULL)
			{
				FILE *fo = fopen(strcat(pch, ".o"), "wb");
				for (int i = 0; i < objCode.size(); ++i)
				{
						printf(toBin(objCode[i]).c_str());
						printf("\n");
						fwrite(&objCode[i], 1, sizeof(int), fo);
				}
				fclose(fo);
			}
			fclose(yyin);
		}
	}
	return 0;
}

%{
#include <stdlib.h>
#include "y_tab.h"
void yyerror(char *);
%}

%%
R[0-9]+     { 
				yylval = atoi(yytext + 1);
                return GPR;
            }

F[0-9]+     { 
				yylval = atoi(yytext + 1);
                return FPR;
            }

[0-9]+  	{
                yylval = atoi(yytext);
                return ADDRESS;
            }

[,()] 		{
                return *yytext;
            }

"DADD"          return DADD;
"DSUB"          return DSUB;
"ADD.D"         return ADDD;
"SUB.D"         return SUBD;
"MUL.D"        	return MULD;
"DIV.D"         return DIVD;
"L.D"           return LD;
"LW"            return LW;
"S.D"           return SD;
"SW"            return SW;
"J"				return J;
"BEQ"			return BEQ;
"BNE"			return BNE;

[ \t\n]+        ;       /* ignore whitespace */

.               yyerror("Unknown character");
%%

int yywrap(void) {
    return 1;
}

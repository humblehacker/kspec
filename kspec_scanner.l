%{
  enum yytokentype { IDENT = 258 };
%}

%%

[\-a-zA-Z0-9_]+   { return IDENT; }

%%

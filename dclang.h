extern DCLANG_INT;
extern DCLANG_FLT;

extern void dclang_initialize();

extern void dclang_import(char *infilestr);

extern DCLANG_INT dclang_findword(const char *word);

extern void dclang_callword(DCLANG_FLT where);

extern DCLANG_FLT dclang_pop();

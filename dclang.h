extern DCLANG_INT;
extern DCLANG_FLT;

extern void dclang_initialize();

extern DCLANG_FLT dclang_pop();

extern DCLANG_INT dclang_findword(const char *word);

extern void dclang_callword(DCLANG_FLT where);

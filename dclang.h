///////////////
// API calls //
///////////////

extern void dclang_initialize();

extern int dclang_import(char *infilestr);

DCLANG_INT dclang_findword(const char *word);

extern void dclang_callword(unsigned long int where);

extern double dclang_pop();

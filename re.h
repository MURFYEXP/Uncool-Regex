#ifndef RE_H
#define RE_H

#include "nfa.h"

/////////////////////////////////
// data structures
typedef enum Re_Kind_t {
  RE_KIND_EPS,
  RE_KIND_CHAR,
  RE_KIND_CONCAT,
  RE_KIND_ALT,
  RE_KIND_CLOSURE
} Re_Kind_t;


/* e -> \epsilon
      | c
      | e e
      | e | e
      | e*
*/
struct Re_t
{
  Re_Kind_t kind;
};
typedef Re_t *Re;


struct Re_Eps_t
{
  Re_Kind_t kind;
};
typedef Re_Eps_t *Re_Eps;


struct Re_Char_t 
{
  Re_Kind_t kind;
  int c;
};
typedef Re_Char_t *Re_Char;


struct Re_Concat_t
{
  Re_Kind_t kind;
  Re left;
  Re right;
};
typedef Re_Concat_t *Re_Concat;


struct Re_Alt_t
{
  Re_Kind_t kind;
  Re left;
  Re right;
};
typedef Re_Alt_t *Re_Alt;


struct Re_Closure_t
{
  Re_Kind_t kind;
  Re exp;
};
typedef Re_Closure_t *Re_Closure;

//////////////////////////////////
// operations
Re re_Eps_new ();
Re re_Char_new (int c);
Re re_Concat_new (Re left, Re right);
Re re_Alt_new (Re left, Re right);
Re re_Closure_new (Re exp);

void bSplit();
void miniDfa_print();
void minimization();
void nfa_To_dfa(Nfa nfa);
void n_To_d(Nfa nfa);
void Dfa_print();
void Re_print (Re e);
Nfa Re_thompson (Re e);

#endif

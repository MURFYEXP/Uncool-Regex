#include <stdio.h>
#include <stdlib.h>
#include "re.h"
#define EPS (-2)

int main(void)
{
  // First, we make a data structure for this
  // regular expression:
  //    a b
    /*
    Re e =  re_Concat_new (re_Char_new ('a')
                                    , re_Char_new ('b'));
  // and to print e1 out to make sure the
  // data structure is right: (note that
    
  // ASCII is now integers)
  printf ("\nthe regular expression is:\n");
    Re_print (e);

  // convert regular expressions to NFA:
  Nfa nfa = Re_thompson (e);
  // print the NFA out:
  printf ("\nthe NFA is:\n");
  Nfa_print (nfa);
 // n_To_d(nfa);
      // a second regular expression:
  //    a (b|c)*
   */

 /* Re e = re_Concat_new (re_Char_new ('a')
                     , re_Closure_new (re_Alt_new(re_Char_new ('b')
                                                 , re_Char_new ('c')))); */

    Re e = re_Concat_new ( re_Char_new ('f')
                        , re_Alt_new (re_Concat_new(re_Char_new ('e')
                                                    , re_Char_new ('e')),
                                        re_Concat_new(re_Char_new ('i')
                                                    , re_Char_new ('e')))); 
  printf ("\nthe regular expression is:\n");
  Re_print (e);
  // convert it to NFA:
  Nfa nfa = Re_thompson (e);
  
  printf ("\n\nNFA:\n");
  nfa_To_dfa(nfa);
  Nfa_print (nfa);
  Dfa_print();

    minimization();
    miniDfa_print();
  return 0;
}

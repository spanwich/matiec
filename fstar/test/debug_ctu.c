#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "iec_std_lib.h"
#include "iec_std_FB.h"
TIME __CURRENT_TIME = {0, 0};

int main() {
    CTU_data__ ms;
    CTU_init__(&ms, 0);
    __SET_VAR((&ms)->, PV,, (INT)5);

    /* CU: 0,1,0,1,0,1 → 3 rising edges */
    for (int i = 0; i < 7; i++) {
        bool cu = (i % 2 == 1);
        __SET_VAR((&ms)->, CU,, (BOOL)cu);
        __SET_VAR((&ms)->, R,, (BOOL)0);
        CTU_body__(&ms);
        printf("step %d: CU=%d Q=%d CV=%d CU_T.Q=%d CU_T.M=%d CU_T.CLK=%d\n",
               i, cu,
               __GET_VAR(ms.Q,), __GET_VAR(ms.CV,),
               __GET_VAR(ms.CU_T.Q,), __GET_VAR(ms.CU_T.M,),
               __GET_VAR(ms.CU_T.CLK,));
    }
    return 0;
}

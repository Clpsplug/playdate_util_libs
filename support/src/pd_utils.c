#include "pd_utils.h"


inline void pdUtil_InitializeAll(void* playdate_context)
{
    pd_Initialize(playdate_context);
    pdText_Initialize(playdate_context);
    pdScene_Initialize(playdate_context);
}

inline void pdUtil_FinalizeAll(void)
{
    pdScene_Finalize();
    pdText_Finalize();
    pd_Finalize();
}

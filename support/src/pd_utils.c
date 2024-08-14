#include "pd_utils.h"


void pdUtil_InitializeAll(void* playdate_context)
{
    pd_Initialize(playdate_context);
    pdText_Initialize(playdate_context);
    pdScene_Initialize(playdate_context);
}

void pdUtil_FinalizeAll(void)
{
    pdScene_Finalize();
    pdText_Finalize();
    pd_Finalize();
}

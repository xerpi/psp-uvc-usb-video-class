#ifndef UTILS_H
#define UTILS_H

#include <pspkernel.h>
#include <pspdebug.h>

#define printf pspDebugScreenPrintf
#define clear() pspDebugScreenSetXY(0, 1)

int exit_callback(int arg1, int arg2, void *common);
int CallbackThread(SceSize args, void *argp);
int SetupCallbacks(void);
extern int run;


#endif

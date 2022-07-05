#ifndef SIGNAL_FUNCTIONS_H
#define SIGNAL_FUNCTIONS_H
#define _GNU_SOURCE
#include <string.h>
#include <signal.h>
#include "signal_functions.h" /* Declares functions defined here */
#include "tlpi_hdr.h"

void /* Print list of signals within a signal set */
printSigset(FILE *of, const char *prefix, const sigset_t *sigset);
int /* Print mask of blocked signals for this process */
printSigMask(FILE *of, const char *msg);
int /* Print signals currently pending for this process */
printPendingSigs(FILE *of, const char *msg);

#endif
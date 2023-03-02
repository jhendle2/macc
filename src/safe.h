#ifndef SAFE_H
#define SAFE_H

#include <stdlib.h>
#define safeFree(X) if (X) free(X)

int safeExit(const int exit_code);

#endif /* SAFE_H */
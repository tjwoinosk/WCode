#ifndef WMATINTTOOLS_H
#define WMATINTTOOLS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct WMatInt WMatInt;
WMatInt* bilinearSample(WMatInt* input, float scaleX, float scaleY);
void print_WMatInt(WMatInt* input);


#ifdef __cplusplus
}
#endif

#endif

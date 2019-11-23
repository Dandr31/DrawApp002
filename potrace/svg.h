#ifndef SVG_H
#define SVG_H
#ifdef __cplusplus
extern "C" {
#endif

#include "potracelib.h"
#include <stdio.h>
#define DIM_IN (72)
#define DIM_CM (72 / 2.54)
#define DIM_MM (72 / 25.4)
#define DIM_PT (1)
/* structure to hold per-image information, set e.g. by calc_dimensions */
struct imginfo_s {
  int pixwidth;        /* width of input pixmap */
  int pixheight;       /* height of input pixmap */
  double width;        /* desired width of image (in pt or pixels) */
  double height;       /* desired height of image (in pt or pixels) */
  double lmar, rmar, tmar, bmar;   /* requested margins (in pt) */
};
typedef struct imginfo_s imginfo_t;
int page_svg(FILE *fout, potrace_path_t *plist ,imginfo_t *imginfo);


#ifdef  __cplusplus
} /* end of extern "C" */
#endif

#endif // SVG_H

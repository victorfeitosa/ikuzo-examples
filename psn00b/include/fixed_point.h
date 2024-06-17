#ifndef _FIXED_POINT_H_
#define _FIXED_POINT_H_

typedef int Fixed;

#define FRACT_BITS 12
#ifndef ONE
  #define ONE (1 << FRACT_BITS)
#endif
#define inttofix(x) ((x) << FRACT_BITS)
#define ftofix(x) ((int)((x) * (1 << FRACT_BITS)))
#define fixtoint(x) ((x) >> FRACT_BITS)
#define fixtodouble(x) (((double)(x)) / (1 << FRACT_BITS))

#endif // _FIXED_POINT_H_
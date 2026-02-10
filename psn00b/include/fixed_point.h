#ifndef _FIXED_POINT_H_
#define _FIXED_POINT_H_

typedef int Fixed;

// Standard fractional fixed point
#define SHORT_FRAC 12
#define LONG_FRAC 20

#define inttofix(x) ((x) << SHORT_FRAC)
#define ftofix(x) ((int)((x) * (1 << SHORT_FRAC)))
#define fixtoint(x) ((x) >> SHORT_FRAC)
#define fixtodouble(x) (((double)(x)) / (1 << SHORT_FRAC))

// Long fractional fixed point
#ifndef LONG_ONE
#define LONG_ONE (1 << LONG_FRAC)
#endif

#define inttolongfix(x) ((x) << LONG_FRAC)
#define ftolongfix(x) ((int)((x) * (1 << LONG_FRAC)))
#define longfixtoint(x) ((x) >> LONG_FRAC)
#define longfixtodouble(x) (((double)(x)) / (1 << LONG_FRAC))

#endif // _FIXED_POINT_H_
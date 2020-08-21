#include "complex.h"

/*
 * Complex number multiplcation.
 *
 * A demonstration of why the below
 * computation is correct:
 *  (A+Bi)*(C+Di)
 *  A*C + A*Di + Bi*C + Bi*Di
 *  A*C + A*Di + Bi*C + B*D*(i^2)
 *  A*C + A*Di + Bi*C + B*D*(-1)
 *  (A*C - B*D) + (A*Di + C*Bi)
 */
struct Complex cmul(struct Complex a, struct Complex b) {
    struct Complex c;
    c.r = (a.r * b.r) - (a.i * b.i);
    c.i = (a.i * b.r) + (a.r * b.i);
    return c;
}

/*
 * Complex number addition.
 * (A+Bi) + (C+Di)
 * (A+C) + (B+D)i
 */
struct Complex cadd(struct Complex a, struct Complex b) {
    struct Complex c;
    c.r = a.r + b.r;
    c.i = a.i + b.i;
    return c;
}

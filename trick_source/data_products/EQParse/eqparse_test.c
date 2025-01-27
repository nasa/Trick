#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "eqparse.h"
#define EQP_EPSILON 1e-10

int main()
{

        char equation1[512];
        char equation2[512];
        double y;
        int ret;
        div_t divt;


       /*-------------------------------------------------------
        *   Test math functions
        */
        fprintf(stderr, "\n[36mTesting integrity of math functions[00m\n");

        // abs()
        strcpy(equation1, "abs(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, -10.0, &y);
        if (ret != 0 || y != 10.0) {
                printf("[31m[FAIL][00m %s\n", equation2);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // acosh()
        strcpy(equation1, "acosh(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 4.1234, &y);
        if (ret != 0 || y != acosh(4.1234)) {
                printf("[31m[FAIL][00m %s\n", equation2);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // acos()
        strcpy(equation1, "acos(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.9876, &y);
        if (ret != 0 || y != acos(0.9876)) {
                printf("[31m[FAIL][00m %s\n", equation2);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // asinh()
        strcpy(equation1, "asinh(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 10.9876, &y);
        if (ret != 0 || (fabs(y - asinh(10.9876)) > EQP_EPSILON)) {
                printf("[31m[FAIL][00m %s\n", equation2);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // asin()
        strcpy(equation1, "asin(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.2576, &y);
        if (ret != 0 || (fabs(y - asin(0.2576)) > EQP_EPSILON)) {
                printf("[31m[FAIL][00m %s\n", equation2);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // atanh()
        strcpy(equation1, "atanh(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.2456, &y);
        if (ret != 0 || (fabs(y - atanh(0.2456)) > EQP_EPSILON)) {
                printf("[31m[FAIL][00m %s\n", equation2);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // atan2()
        strcpy(equation1, "atan2(x,2.3456)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.2456, &y);
        if (ret != 0 || (fabs(y - atan2(0.2456, 2.3456)) > EQP_EPSILON)) {
                printf("[31m[FAIL][00m %s\n", equation2);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // atan()
        strcpy(equation1, "atan(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 10.2456, &y);
        if (ret != 0 || (fabs(y - atan(10.2456)) > EQP_EPSILON)) {
                printf("[31m[FAIL][00m %s\n", equation2);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // besj0()
        strcpy(equation1, "besj0(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 10.2456, &y);
        if (ret != 0 || (fabs(y - j0(10.2456)) > EQP_EPSILON)) {
                printf("[31m[FAIL][00m %s\n", equation2);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // besj1()
        strcpy(equation1, "besj1(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 11.2456, &y);
        if (ret != 0 || (fabs(y - j1(11.2456)) > EQP_EPSILON)) {
                printf("[31m[FAIL][00m %s\n", equation2);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // besy0()
        strcpy(equation1, "besy0(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 10.2456, &y);
        if (ret != 0 || (fabs(y - y0(10.2456)) > EQP_EPSILON)) {
                printf("[31m[FAIL][00m %s\n", equation2);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // besy1()
        strcpy(equation1, "besy1(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 11.2456, &y);
        if (ret != 0 || (fabs(y - y1(11.2456)) > EQP_EPSILON)) {
                printf("[31m[FAIL][00m %s\n", equation2);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // ceil()
        strcpy(equation1, "ceil(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 11.2456, &y);
        if (ret != 0 || (fabs(y - ceil(11.9999)) > EQP_EPSILON)) {
                printf("[31m[FAIL][00m %s\n", equation2);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // cosh()
        strcpy(equation1, "cosh(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 11.2456, &y);
        if (ret != 0 || (fabs(y - cosh(11.2456)) > EQP_EPSILON)) {
                printf("[31m[FAIL][00m %s\n", equation2);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // cos()
        strcpy(equation1, "cos(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 11.2456, &y);
        if (ret != 0 || (fabs(y - cos(11.2456)) > EQP_EPSILON)) {
                printf("[31m[FAIL][00m %s\n", equation2);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // div()
        strcpy(equation1, "div(x, 43)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 123456, &y);
        divt = div(123456, 43);
        if (ret != 0 || (fabs(y - divt.quot) > EQP_EPSILON)) {
                printf("[31m[FAIL][00m %s\n", equation2);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // erfc()
        strcpy(equation1, "erfc(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.54321, &y);
        if (ret != 0 || (fabs(y - erfc(0.54321)) > EQP_EPSILON)) {
                printf("[31m[FAIL][00m %s\n", equation2);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // erf()
        strcpy(equation1, "erf(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.54321, &y);
        if (ret != 0 || (fabs(y - erf(0.54321)) > EQP_EPSILON)) {
                printf("[31m[FAIL][00m %s\n", equation2);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // exp()
        strcpy(equation1, "exp(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 6.54321, &y);
        if (ret != 0 || (fabs(y - exp(6.54321)) > EQP_EPSILON)) {
                printf("[31m[FAIL][00m %s\n", equation2);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // floor()
        strcpy(equation1, "floor(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 6.50321, &y);
        if (ret != 0 || (fabs(y - floor(6.54321)) > EQP_EPSILON)) {
                printf("[31m[FAIL][00m %s\n", equation2);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // gamma()
        strcpy(equation1, "gamma(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 6, &y);
        if (ret != 0 || (fabs(y - 120) > EQP_EPSILON)) {
                printf("[31m[FAIL][00m %s\n", equation2);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // int()
        strcpy(equation1, "int(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, -6.123, &y);
        if (ret != 0 || (fabs(y + 6) > EQP_EPSILON)) {
                printf("[31m[FAIL][00m %s\n", equation2);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }
        ret = equationparse(equation1, -6.8, &y);
        if (ret != 0 || (fabs(y + 6) > EQP_EPSILON)) {
                printf("[31m[FAIL][00m %s\n", equation2);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }


        // inverf()
        strcpy(equation1, "inverf(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.5, &y);
        if (ret != 0 || (fabs(erf(y) - 0.5) > 10e-16)) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, erf(y));
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // invnorm()
        strcpy(equation1, "invnorm(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.75, &y);
        if (ret != 0 || (fabs(y - 0.67449) > EQP_EPSILON * 1e+5)) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // lgamma()
        strcpy(equation1, "lgamma(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 5.0, &y);
        if (ret != 0 || (fabs(y - lgamma(5.0)) > EQP_EPSILON)) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // log10()
        strcpy(equation1, "log10(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 15.5432, &y);
        if (ret != 0 || (fabs(y - log10(15.5432)) > EQP_EPSILON)) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // log()
        strcpy(equation1, "log(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 9.8765, &y);
        if (ret != 0 || (fabs(y - log(9.8765)) > EQP_EPSILON)) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // norm()
        strcpy(equation1, "norm(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 2.5, &y);
        if (ret != 0 || (fabs(y - 0.99379) > EQP_EPSILON * 1e+5)) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // sgn()
        strcpy(equation1, "sgn(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.57812, &y);
        if (ret != 0 || (fabs(y - 1) > EQP_EPSILON)) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // sinh()
        strcpy(equation1, "sinh(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 1.13579, &y);
        if (ret != 0 || (fabs(y - sinh(1.13579)) > EQP_EPSILON)) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // sin()
        strcpy(equation1, "sin(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.97531, &y);
        if (ret != 0 || (fabs(y - sin(0.97531)) > EQP_EPSILON)) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // sqrt()
        strcpy(equation1, "sqrt(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 6.97939, &y);
        if (ret != 0 || (fabs(y - sqrt(6.97939)) > EQP_EPSILON)) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // tanh()
        strcpy(equation1, "tanh(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 2.5, &y);
        if (ret != 0 || (fabs(y - tanh(2.5)) > EQP_EPSILON)) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // tan()
        strcpy(equation1, "tan(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.78912, &y);
        if (ret != 0 || (fabs(y - tan(0.78912)) > EQP_EPSILON)) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

       /*-------------------------------------------------------
        *   Test operators
        */
        fprintf(stderr, "\n[36mTesting operators and precedence.[00m\n");

        // x + x
        strcpy(equation1, "x + x");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 1.23, &y);
        if (ret != 0 || (fabs(y - 2.46)) > EQP_EPSILON) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // x - x
        strcpy(equation1, "x - x");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 1.2356789, &y);
        if (ret != 0 || (fabs(y - 0.0)) > EQP_EPSILON) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // x * x
        strcpy(equation1, "x*x");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 1.2356789, &y);
        if (ret != 0 || (fabs(y - 1.2356789 * 1.2356789)) > EQP_EPSILON) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // x / x
        strcpy(equation1, "x/x");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 1.2356789, &y);
        if (ret != 0 || (fabs(y - 1.0)) > EQP_EPSILON) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // x ^ 3
        strcpy(equation1, "x^3");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 1.23, &y);
        if (ret != 0 || (fabs(y - 1.23 * 1.23 * 1.23)) > EQP_EPSILON) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // x % 3
        strcpy(equation1, "x%3");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 43.45321, &y);
        if (ret != 0 || (fabs(y - 1.45321)) > EQP_EPSILON) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // x*(x * x + x - x * x + x ^ 2 - x + x ^ 2 + x % 5) + 1
        strcpy(equation1,
              "x*(x / 3 + x * x + x - x * x + x ^ 2 - x + x ^ 2 + x % 5) + 1 ");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 6.0, &y);
        if (ret != 0 || (fabs(y - 451)) > EQP_EPSILON) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

       /*-------------------------------------------------------
        *   Test expressions
        */
        fprintf(stderr, "\n[36mTesting parenthetical expressions.[00m\n");

        // (x)
        strcpy(equation1, "(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 1.0, &y);
        if (ret != 0 || (fabs(y - 1.0)) > EQP_EPSILON) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // (asin(sin(x)))*(acos(cos(x)))*(atan(tan(x)))
        strcpy(equation1, "(asin(sin(x)))*(acos(cos(x)))*(atan(tan(x)))");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.5, &y);
        if (ret != 0 || (fabs(y - 0.125)) > EQP_EPSILON) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // (sinh(asinh(x)))*(acosh(cosh(x)))*(atanh(tanh(x)))
        strcpy(equation1, "(sinh(asinh(x)))*(acosh(cosh(x)))*(atanh(tanh(x)))");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.5, &y);
        if (ret != 0 || (fabs(y - 0.125)) > EQP_EPSILON) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // (sinh(asinh(x)))*(acosh(cosh(x)))*(atanh(tanh(x)))
        strcpy(equation1, "(sinh(asinh(x)))*(acosh(cosh(x)))*(atanh(tanh(x)))");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.5, &y);
        if (ret != 0 || (fabs(y - 0.125)) > EQP_EPSILON) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // log(exp((cos(x)*cos(x) + sin(x)*sin(x))^100))
        strcpy(equation1, "log(exp((cos(x)*cos(x) + sin(x)*sin(x))^100))");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 3.21, &y);
        if (ret != 0 || (fabs(y - 1.0)) > EQP_EPSILON) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // log(exp((cos(x)^2 + sin(x)^2)^100))
        strcpy(equation1, "log(exp((cos(x)^2 + sin(x)^2)^100))");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 3.21, &y);
        if (ret != 0 || (fabs(y - 1.0)) > EQP_EPSILON) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }


        // sqrt(sqrt(x^2 + abs(-16))^2)
        strcpy(equation1, "sqrt(sqrt(x^2 + abs(-16))^2)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 3.0, &y);
        if (ret != 0 || (fabs(y - 5.0)) > EQP_EPSILON) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // (-1)*(-2)*(-cos(0))
        strcpy(equation1, "(-1)*(-2)*(-cos(0))");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 3.0, &y);
        if (ret != 0 || (fabs(y + 2.0)) > EQP_EPSILON) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

       /*-------------------------------------------------------
        * Test constants
        */
        fprintf(stderr, "\n[36mTesting constants.[00m\n");

        // M_PI
        strcpy(equation1, "sin(M_PI/2)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 3.0, &y);
        if (ret != 0 || (fabs(y - sin(M_PI / 2))) > EQP_EPSILON) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // PI
        strcpy(equation1, "sin(PI/2)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 3.0, &y);
        if (ret != 0 || (fabs(y - sin(M_PI / 2))) > EQP_EPSILON) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // M_E
        strcpy(equation1, "log(M_E)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 3.0, &y);
        if (ret != 0 || (fabs(y - 1.0)) > EQP_EPSILON) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

       /*-------------------------------------------------------
        * Test math errors
        */
        fprintf(stderr, "\n[36mTesting math error handling.[00m\n");

        // Divide by zero
        strcpy(equation1, "1/( sin(0) )");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 6) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

#ifndef __sun
        // acos(2) x: out of domain
        strcpy(equation1, "acos(2)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 0 || !isnan(y)) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // asin(2) x: out of domain
        strcpy(equation1, "asin(2)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 0 || !isnan(y)) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }
#endif

        // atanh(2) x: out of domain
        strcpy(equation1, "atanh(2)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 11) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // besy0(-2) x: out of domain
        strcpy(equation1, "besy0(-2)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 8) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // besy1(-2) x: out of domain
        strcpy(equation1, "besy1(-2)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 18) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // inverf(2) x: out of domain
        strcpy(equation1, "inverf(2)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 19) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // invnorm(2) x: out of domain
        strcpy(equation1, "invnorm(2)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 16) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // log(0) x: out of domain
        strcpy(equation1, "log(0)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 21) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // log(-1) x: out of domain
        strcpy(equation1, "log(-1)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 21) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // log10(0) x: out of domain
        strcpy(equation1, "log10(0)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 20) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // log10(-1) x: out of domain
        strcpy(equation1, "log10(-1)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 20) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // sqrt(-1) x: out of domain
        strcpy(equation1, "sqrt(-1)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 0 || !isnan(y)) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

       /*-------------------------------------------------------
        * Test syntax errors
        */
        fprintf(stderr, "\n[36mTesting syntax error handling.[00m\n");

        // Passing '.' by itself
        strcpy(equation1, ".");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 15) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // An expression with '.' on end is a syntax error
        strcpy(equation1, "x+1.");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 15) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // A misplaced '.'
        strcpy(equation1, "sin(.x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 15) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }
        strcpy(equation1, "1. 2 + 3");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 15) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // Passing '_' by itself
        strcpy(equation1, "_");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 15) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // '_' at end
        strcpy(equation1, "1 + x_");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 15) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // '_' at beginning
        strcpy(equation1, "_x + x");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 15) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // '_' not preceded by M
        strcpy(equation1, "N_PI");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 15) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // '_' not followed by P or E
        strcpy(equation1, "M_pI");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 15) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // "M_P" not followed by I
        strcpy(equation1, "M_PE");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 15) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // Missing second arg
        strcpy(equation1, "div(6,)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 15) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // Missing first arg
        strcpy(equation1, "div(,32)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 15) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // Missing opening parens in 2 arg func
        strcpy(equation1, "div2,32)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 15) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // Num args don't match
        strcpy(equation1, "sin(x,32)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 15) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // Mispelled div()
        strcpy(equation1, "dev(x,32)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 15) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // Mispelled atan2()
        strcpy(equation1, "atuna2(x,32)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 15) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // Args don't match
        strcpy(equation1, "atan2(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 17) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // Illegal character anywhere
        strcpy(equation1, "sin(x) = 1");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 15) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // Illegal character anywhere
        strcpy(equation1, "7!");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 15) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }


        // Illegal character anywhere
        strcpy(equation1, "sin(x) + x|7 + x^2");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 15) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // Illegal character anywhere
        strcpy(equation1, "x|7");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 15) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // Bad function name
        strcpy(equation1, "sun(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 14) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // Bad function name
        strcpy(equation1, "sincos(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 14) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // Operands without operator
        strcpy(equation1, "sin(x1)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 4) {
                printf("[31m[FAIL][00m %s %lf %d\n", equation2, y, ret);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // Bad function name (interpreted as missing operator)
        strcpy(equation1, "sing(x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 4) {
                printf("[31m[FAIL][00m %s %lf %d\n", equation2, y, ret);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // Function missing closing paren
        strcpy(equation1, "sin(x + 1 + exp((x+1)+1)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 1) {
                printf("[31m[FAIL][00m %s %lf\n", equation2, y);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // Two letter variable
        strcpy(equation1, "xy + 1");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 14) {
                printf("[31m[FAIL][00m %s %lf %d\n", equation2, y, ret);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // Missing paren
        strcpy(equation1, "(x + 2");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 1) {
                printf("[31m[FAIL][00m %s %lf %d\n", equation2, y, ret);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }
        strcpy(equation1, "[x + 2");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 2) {
                printf("[31m[FAIL][00m %s %lf %d\n", equation2, y, ret);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }
        strcpy(equation1, "{x + 2");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 3) {
                printf("[31m[FAIL][00m %s %lf %d\n", equation2, y, ret);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // Matching wrong types of braces
        strcpy(equation1, "(x + 2}*2");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 1) {
                printf("[31m[FAIL][00m %s %lf %d\n", equation2, y, ret);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }
        strcpy(equation1, "[((x + 2)]*2");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 1) {
                printf("[31m[FAIL][00m %s %lf %d\n", equation2, y, ret);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }
        strcpy(equation1, "[{(x + 2)]}*2");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 3) {
                printf("[31m[FAIL][00m %s %lf %d\n", equation2, y, ret);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // Operator without operand
        strcpy(equation1, "x++");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 5) {
                printf("[31m[FAIL][00m %s %lf %d\n", equation2, y, ret);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // Operand without operator
        strcpy(equation1, "x x");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 4) {
                printf("[31m[FAIL][00m %s %lf %d\n", equation2, y, ret);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        //  Pass in NULL
        strcpy(equation1, "");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (ret != 22) {
                printf("[31m[FAIL][00m NULL %lf %d\n", y, ret);
                return (-1);
        } else {
                printf("[32m[PASS][00m NULL\n");
        }


       /*-------------------------------------------------------
        * Other Tests
        */
        fprintf(stderr, "\n[36mMore tests.[00m\n");

#if ( __linux__ | __APPLE__ )
        // Infinity
        strcpy(equation1,
               "10000000000000000000000000000000000^1000000000000000000000000000000000000");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 0.0, &y);
        if (!isinf(y)) {
                printf("[31m[FAIL][00m %s %lf %d\n", equation2, y, ret);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }
#endif

        // Curly braces
        strcpy(equation1, "sin( {x + 1}^2 )");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 1.0, &y);
        if (ret != 0 || (fabs(y - sin(4)) > EQP_EPSILON)) {
                printf("[31m[FAIL][00m %s %lf %d\n", equation2, y, ret);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // Square brackets
        strcpy(equation1, "[sin( {x + 1}^2 )]*[ ( x + 1) ]");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 1.0, &y);
        if (ret != 0 || (fabs(y - sin(4) * 2) > EQP_EPSILON)) {
                printf("[31m[FAIL][00m %s %lf %d\n", equation2, y, ret);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        // x)  Death!
        strcpy(equation1, "x)");
        strcpy(equation2, equation1);
        ret = equationparse(equation1, 1.0, &y);
        if (ret != 23) {
                printf("[31m[FAIL][00m %s %lf %d\n", equation2, y, ret);
                return (-1);
        } else {
                printf("[32m[PASS][00m %s\n", equation2);
        }

        return 0 ;
}

#ifndef _GOA_VECTOR_H_
#define _GOA_VECTOR_H_

#include "vector_template.h"
#include "matrix_template.h"
#include "quaternion_template.h"

typedef vector_t<double> vector;
typedef matrix_t<double> matrix;
typedef quaternion_t<double> quaternion;

typedef vector_t<float> vector_f;
typedef matrix_t<float> matrix_f;
typedef quaternion_t<float> quaternion_f;

#endif

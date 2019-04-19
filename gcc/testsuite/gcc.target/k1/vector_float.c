
typedef float float32x2_t __attribute__((vector_size(8)));


float32x2_t k1_fabswp_builtin(float32x2_t a) {
    return __builtin_k1_fabswp(a);
}


float32x2_t k1_fnegwp_builtin(float32x2_t a) {
    return __builtin_k1_fnegwp(a);
}


float32x2_t k1_fmaxwp_builtin(float32x2_t a, float32x2_t b) {
    return __builtin_k1_fmaxwp(a, b);
}


float32x2_t k1_fminwp_builtin(float32x2_t a, float32x2_t b) {
    return __builtin_k1_fminwp(a, b);
}


float32x2_t k1_faddwp(float32x2_t a, float32x2_t b) {
    return a + b;
}

float32x2_t k1_faddwp_builtin(float32x2_t a, float32x2_t b) {
    return __builtin_k1_faddwp(a, b, ".rn");
}


float32x2_t k1_fsbfwp(float32x2_t a, float32x2_t b) {
    return a - b;
}

float32x2_t k1_fsbfwp_builtin(float32x2_t a, float32x2_t b) {
    return __builtin_k1_fsbfwp(a, b, ".rn");
}


float32x2_t k1_fmulwp(float32x2_t a, float32x2_t b) {
    return a * b;
}

float32x2_t k1_fmulwp_builtin(float32x2_t a, float32x2_t b) {
    return __builtin_k1_fmulwp(a, b, ".rn");
}


float32x2_t k1_ffmawp(float32x2_t a, float32x2_t b, float32x2_t c) {
    return a + b * c;
}

float32x2_t k1_ffmawp_builtin(float32x2_t a, float32x2_t b, float32x2_t c) {
    return __builtin_k1_ffmawp(a, b, c, ".rn");
}


float32x2_t k1_ffmswp(float32x2_t a, float32x2_t b, float32x2_t c) {
    return a - b * c;
}

float32x2_t k1_ffmswp_builtin(float32x2_t a, float32x2_t b, float32x2_t c) {
    return __builtin_k1_ffmswp(a, b, c, ".rn");
}


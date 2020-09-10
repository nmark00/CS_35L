#include <stdio.h>

#include "vector.h"

//#define DEBUG


IntVector *int_vec_with_capacity(size_t const capacity) {
    int *buf = malloc(sizeof(int) * capacity);
    if (!buf) {
        fprintf(stderr, "failed to create IntVector buffer with capacity %zu", capacity);
        exit(EXIT_FAILURE);
    }
    IntVector *vec = malloc(sizeof(IntVector));
    if (!vec) {
        fprintf(stderr, "failed to create IntVector");
        exit(EXIT_FAILURE);
    }
    vec->capacity = capacity;
    vec->len = 0;
    vec->buf = buf;
    return vec;
}

IntVector *int_vec() {
    return int_vec_with_capacity(2);
}

int int_vec_get(IntVector const *const vec, size_t const index) {
    if (index >= vec->len) {
        fprintf(stderr, "vector index %zu out of range", index);
        exit(EXIT_FAILURE);
    }
    return vec->buf[index];
}

size_t int_vec_len(IntVector const *const vec) {
    return vec->len;
}

void int_vec_reserve(IntVector *const vec, size_t const extra_capacity) {
    #ifdef DEBUG
    printf(
        "int_vec_reserve\tcurrent len: %zu current capacity: %zu extra_capacity: %zu\n\n",
        vec->len, vec->capacity, extra_capacity
    );
    #endif

    size_t const new_capacity = vec->capacity + extra_capacity;
    vec->buf = realloc(vec->buf, sizeof(int) * new_capacity);
    if (!vec->buf) {
        fprintf(stderr, "failed to reserve an extra capacity of %zu for IntVector", extra_capacity);
        exit(EXIT_FAILURE);
    }
    vec->capacity = new_capacity;
}

void int_vec_push(IntVector *const vec, int const item) {
    #ifdef DEBUG
    printf(
        "int_vec_push\tcurrent len: %zu current capacity: %zu pushing item: %d\n",
        vec->len, vec->capacity, item
    );
    #endif

    if (vec->len == vec->capacity) {
        int_vec_reserve(vec, vec->capacity);
    }
    vec->buf[vec->len] = item;
    vec->len += 1;
}

void int_vec_free(IntVector *const vec) {
    free(vec->buf);
    free(vec);
}

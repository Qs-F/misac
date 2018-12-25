#include <stdio.h>
#include <stdlib.h>

#define TABLE_SIZE 8

// UINTMAX_MAX: 18446744073709551615
// WANT: 651687674221131912
// 65C40 must be expressed in uintmax

typedef struct elem_t {
  unsigned int key;
  int n;
  struct elem_t *PREV;
  struct elem_t *NEXT;
} elem_t;

typedef elem_t **table_t;

table_t init() {
  table_t table;
  table = (table_t)malloc(sizeof(elem_t) * TABLE_SIZE);
  if (table == NULL) {
    fprintf(stderr, "Cannot allocate memory\n");
    return NULL;
  }
  for (int i = 0; i < TABLE_SIZE; i++) {
    table[i] = NULL;
  }
  return table;
}

void printb(unsigned int v) {
  unsigned int mask = (int)1 << (sizeof(v) * 4 - 1);
  do
    putchar(mask & v ? '1' : '0');
  while (mask >>= 1);
  putchar('\n');
}

void printe(elem_t *e) {
  printf("me: %p, key: %u, n: %d, PREV: %p, NEXT: %p\n", e, e->key, e->n,
         e->PREV, e->NEXT);
}

void print(table_t t) {
  elem_t *e;
  for (int i = 0; i < TABLE_SIZE; i++) {
    if (t[i] != NULL) {
      printf("TABLE: %d\n", i);
      e = t[i];
      while (e != NULL) {
        printe(e);
        e = e->NEXT;
      }
    }
  }
  puts("");
}

unsigned int hash(unsigned int key) {
  key = key >> 1;
  key &= 7;
  return key;
}

elem_t *search(table_t table, unsigned int key) {
  unsigned int h = hash(key);
  if (table[h] == NULL) return NULL;
  elem_t *e = table[h];
  while (e != NULL && e->key != key) {
    e = e->NEXT;
  }
  if (e == NULL || e->key != key) return NULL;
  return e;
}

int ope(table_t table, unsigned int key, int n) {
  elem_t *v;
  unsigned int h = hash(key);
  if ((v = search(table, key)) != NULL) {
    if (((v->n) + n) == 0) {
      if (v->PREV != NULL) v->PREV->NEXT = v->NEXT;
      if (v->NEXT != NULL) v->NEXT->PREV = v->PREV;
      if (table[h] == v) table[h] = v->NEXT;
      v->n = 0;
      free(v);
      return 0;
    }
    v->n += n;
    return (v->n);
  }
  elem_t *e = (elem_t *)malloc(sizeof(elem_t));
  if (e == NULL) {
    fprintf(stderr, "Cannot allocate memory for element\n");
    return 0;
  }
  e->key = key;
  e->n = n;
  e->PREV = NULL;
  e->NEXT = table[h];
  if (e->NEXT != NULL) e->NEXT->PREV = e;
  table[h] = e;
  return n;
}

int inc(table_t table, unsigned int key) { return ope(table, key, 1); }

int dec(table_t table, unsigned int key) { return ope(table, key, -1); }

void factor_reduction(table_t table, unsigned int dividend,
                      unsigned int divisor) {
  for (int i = 2; i <= dividend; i++) {
    while (dividend % i == 0) {
      dividend /= i;
      inc(table, i);
    }
  }
  for (int i = 2; i <= divisor; i++) {
    while (divisor % i == 0) {
      divisor /= i;
      dec(table, i);
    }
  }
}

unsigned long long int ullpow(unsigned int x, unsigned int y) {
  unsigned long long int p = 1;
  for (unsigned int i = 0; i < y; i++) {
    p *= x;
  }
  return p;
}

unsigned long long int combination(unsigned int n, unsigned int r) {
  r = (n - r > r) ? r : n - r;
  table_t t = init();
  if (t == NULL) {
    return 0;
  }
  while (r > 0) factor_reduction(t, n--, r--);

  unsigned long long int product = 1;
  elem_t *e;
  for (int i = 0; i < TABLE_SIZE; i++) {
    if (t[i] != NULL) {
      e = t[i];
      while (e != NULL) {
        product *= ullpow(e->key, e->n);
        e = e->NEXT;
      }
    }
  }
  return product;
}

int main(void) {
  printf("%llu\n", combination(65, 40));
  return 0;
}

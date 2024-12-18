#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hash.h"

//#define DEBUG 0
// gcc -DDEBUG to turn on debug statements

static struct entry *table[NBUCKET];

static int
hash(char *name)
{
#ifdef DEBUG
  char *initname = name;
#endif
  int hashval = 0;
  while (*name)
    hashval += *name++;
#ifdef DEBUG
  printf("name: %s, hashval: %d, index: %d\n", initname, hashval, hashval % NBUCKET);
#endif
  return hashval;
}

static void
insert(char *name, int age, struct entry **p, struct entry *n)
{
  struct entry *e = malloc(sizeof(struct entry));
  e->name = name;
  e->age = age;
  e->next = n;
  *p = e;
}

static struct entry *
get_entry(char *name)
{
  struct entry *e = 0;
  for (e = table[hash(name) % NBUCKET]; e != 0; e = e->next) {
    if (strcmp(name, e->name) == 0)
      break;
  }
  return e;
}

void
put(char *name, int age)
{
  struct entry *e = get_entry(name);
  if(e){
    e->age = age; // update the existing key.
  } else {
    // don't use address passed in case it is reused or local
    char *uniquename = malloc(strlen(name)+1);
    strcpy(uniquename, name);
    int i = hash(uniquename) % NBUCKET;
    insert(uniquename, age, &table[i], table[i]);
  }
}

int
get(char *name)
{
  struct entry *e = get_entry(name);
  return e != 0 ? e->age : 0;
}

void
printhash()
{
  for (int i = 0; i < NBUCKET; i++)
    for (struct entry *e = table[i]; e != 0; e = e->next)
      printf("name: %s, age: %d, hashval: %d, index: %d\n", 
           e->name,  e->age, hash(e->name),    i);
}

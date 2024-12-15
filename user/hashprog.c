#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define NBUCKET 5
#define NKEYS 100000

struct entry {
  char *name; // name is key
  int age;    // age is value
  struct entry *next;
};

void put(char *name, int age);
int get(char *name);
void printhash();

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

// The function coletta() has a local char[] coletta.
// put() must make a copy of coletta[] because the stack is reused
void 
coletta()
{
  char coletta[] = "coletta";
  put(coletta, 10);
}

char *ja = "jerri anne\0was here";

int
main(int argc, char *argv[])
{
  // put() must make a copy of opal[] because opal[] is reused
  // and we want two entries - opal and open
  char opal[] = "opal";   // opal[] contains "opal\0"
  put("gusty", 22);
  put(opal, 4);
  opal[2] = 'e';
  opal[3] = 'n';          // opal[] contains "open\0"
  put(opal, 100);
  char *tysug = malloc(6);
  strcpy(tysug, "tysug"); // tysug[] contains "tysug\0"
  // put() must make a copy of tysug[] because tysug[] is reused
  // and we want two entries - tysug and ty
  put(tysug, 222);
  *(tysug+2) = 0;         // tysug[] contains "ty\0ug\0"
  put(tysug, 333);
  coletta();
  put(ja, 20);
  char florence[] = {0x66, 0x6c, 0x6f, 0x72, 0x65, 0x6e, 0x63, 0x65, 0};
  put(florence, 1);
  printf("opal: %d\n", get("opal"));
  printf("tysug: %d\n", get("tysug"));
  printf("notthere: %d\n", get("notthere"));
  if (argc == 3)   // ./hashprog name age has been entered
    put(argv[1], atoi(argv[2]));
  printhash();
  return 0;
}

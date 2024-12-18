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

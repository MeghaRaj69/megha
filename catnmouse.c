#include <sys/types.h>
#include <stdlib.h>
#include <pthread.h>
#if OPT_A1
#include "kitchen.h"
#endif 
extern int initialize_bowls(unsigned int bowlcount);
extern void cat_eat(unsigned int bowlnumber);
extern void mouse_eat(unsigned int bowlnumber);
extern void cat_sleep(void);
extern void mouse_sleep(void);
int NumBowls;
int NumCats;   
int NumMice;   
int NumLoops;  
struct semaphore *CatMouseWait;
#if OPT_A1
struct kitchen *k = NULL; 
struct kitchen *kitchen_create() {
    int i;
    struct kitchen *k = kmalloc(sizeof(struct kitchen));
    if (k == NULL) {
        return NULL;
    }
    k->bowl_locks = kmalloc(NumBowls * sizeof(struct lock *));
    if (k->bowl_locks == NULL) {
        kfree(k);
        return NULL;
    }
    for (i = 0; i < NumBowls; i++) {
        k->bowl_locks[i] = lock_create("bowl");
    }
    k->kitchen_lock = lock_create("enter");
    k->kitchen_cv = cv_create("kitchen");
    k->group_list = q_create(1);
    k->current_creature = 2;
    k->creature_count = 0;
    return k;
}
void enter_kitchen(const int creature_type) { 
    else if (!q_empty(k->group_list)) {
        int index = q_getend(k->group_list) > 0 ? q_getend(k->group_list)-1 : q_getsize(k->group_list) - 1;
        struct kgroup *g = (struct kgroup *)q_getguy(k->group_list, index);
        if (g->type == creature_type) {
            g->amount++;
        } else {
            // Otherwise, start a new last group
            g = kmalloc(sizeof(struct kgroup));
            g->type = creature_type;
            g->amount = 1;
            q_addtail(k->group_list, g);
        }
        cv_wait(k->kitchen_cv, k->kitchen_lock);
    }
   
    else if (k->current_creature != creature_type) {
        struct kgroup *g = kmalloc(sizeof(struct kgroup));
        g->type = creature_type;
        g->amount = 1;
        q_addtail(k->group_list, g);
        cv_wait(k->kitchen_cv, k->kitchen_lock);
    }
    k->current_creature = creature_type;
    k->creature_count++;
    lock_release(k->kitchen_lock);
}

void exit_kitchen() {
    lock_acquire(k->kitchen_lock);
    k->creature_count--;
    if (!q_empty(k->group_list) && k->creature_count == 0) {
        struct kgroup *g = q_remhead(k->group_list);
        int i;
        for (i = 0; i < g->amount; i++) {
            cv_signal(k->kitchen_cv, k->kitchen_lock);
        }
        kfree(g);
    } else if (q_empty(k->group_list) && k->creature_count == 0) {
        k->current_creature = 2;
    }
    lock_release(k->kitchen_lock);
}

void eat(const int creature_type) {
    enter_kitchen(creature_type);
    unsigned int bowl = ((unsigned int)random() % NumBowls);
    int i = 0;
    for (i = 0; i < NumBowls; i++) {
        if (lock_tryacquire(k->bowl_locks[bowl])) break;
        bowl = (bowl+1) % NumBowls;
    }
    if (i == NumBowls) {
        bowl %= NumBowls;
        lock_acquire(k->bowl_locks[bowl]);
    }
    assert(lock_do_i_hold(k->bowl_locks[bowl]));
    if (creature_type) {
        mouse_eat(bowl+1);
    } else {
        cat_eat(bowl+1);
    }
    lock_release(k->bowl_locks[bowl]);
    exit_kitchen();
}

void kitchen_destroy(struct kitchen *k) {
    int i;
    while (!q_empty(k->group_list)) {
        kfree(q_remhead(k->group_list));
    }
   q_destroy(k->group_list);
    cv_destroy(k->kitchen_cv);
    lock_destroy(k->kitchen_lock);
    for (i = 0; i < NumBowls; i++) {
        lock_destroy(k->bowl_locks[i]);
    }
    kfree(k->bowl_locks);
    kfree(k);
    k = NULL;
}

#endif //OPT_A1
static
void
cat_simulation(void * unusedpointer, 
	       unsigned long catnumber)
{
  int i;
  unsigned int bowl;
  (void) unusedpointer;
  (void) catnumber;
  for(i=0;i<NumLoops;i++) {
    cat_sleep();
#if OPT_A1
    (void)bowl; //suppress warning
    eat(0);
#else
    bowl = ((unsigned int)random() % NumBowls) + 1;
    cat_eat(bowl);
#endif // OPT_A1

  }
  V(CatMouseWait); 
}
static
void
mouse_simulation(void * unusedpointer,
          unsigned long mousenumber)
{
  int i;
  unsigned int bowl;
  (void) unusedpointer;
  (void) mousenumber;
  for(i=0;i<NumLoops;i++) {
    mouse_sleep();
#if OPT_A1
    (void)bowl; // suppress warning
    eat(1);
#else
    bowl = ((unsigned int)random() % NumBowls) + 1;
    mouse_eat(bowl);
#endif // OPT_A1

  }
  V(CatMouseWait); 
}
int
catmouse(int nargs,
	 char ** args)
{
  int index, error;
  int i;
  if (nargs != 5) {
    kprintf("Usage: <command> NUM_BOWLS NUM_CATS NUM_MICE NUM_LOOPS\n");
    return 1;  // return failure indication
  }
  NumBowls = atoi(args[1]);
  if (NumBowls <= 0) {
    kprintf("catmouse: invalid number of bowls: %d\n",NumBowls);
    return 1;
  }
  NumCats = atoi(args[2]);
  if (NumCats < 0) {
    kprintf("catmouse: invalid number of cats: %d\n",NumCats);
    return 1;
  }
  NumMice = atoi(args[3]);
  if (NumMice < 0) {
    kprintf("catmouse: invalid number of mice: %d\n",NumMice);
    return 1;
  }
  NumLoops = atoi(args[4]);
  if (NumLoops <= 0) {
    kprintf("catmouse: invalid number of loops: %d\n",NumLoops);
    return 1;
  }
  kprintf("Using %d bowls, %d cats, and %d mice. Looping %d times.\n",
          NumBowls,NumCats,NumMice,NumLoops);
  CatMouseWait = sem_create("CatMouseWait",0);
  if (CatMouseWait == NULL) {
    panic("catmouse: could not create semaphore\n");
  }
  if (initialize_bowls(NumBowls)) {
    panic("catmouse: error initializing bowls.\n");
  }

#if OPT_A1
  // Create the kitchen
  k = kitchen_create();
#endif
  for (index = 0; index < NumCats; index++) {
    error = thread_fork("cat_simulation thread",NULL,index,cat_simulation,NULL);
    if (error) {
      panic("cat_simulation: thread_fork failed: %s\n", strerror(error));
    }
  }
  for (index = 0; index < NumMice; index++) {
    error = thread_fork("mouse_simulation thread",NULL,index,mouse_simulation,NULL);
    if (error) {
      panic("mouse_simulation: thread_fork failed: %s\n",strerror(error));
    }
  } 
  for(i=0;i<(NumCats+NumMice);i++) {
    P(CatMouseWait);
  }
#if OPT_A1
  // Cleanup the kitchen lol
  kitchen_destroy(k);
#endif
  sem_destroy(CatMouseWait);
  return 0;
}

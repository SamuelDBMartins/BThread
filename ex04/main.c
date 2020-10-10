#include <setjmp.h>
#include <stdio.h>

#define CUSHION_SIZE 10000
#define save_context(CONTEXT) setjmp(CONTEXT)
#define restore_context(CONTEXT) longjmp(CONTEXT, 1)

typedef enum { __BTHREAD_UNINITIALIZED, __BTHREAD_READY, __BTHREAD_DEAD } bthread_state;
typedef void *(*bthread_routine) (void *);

void create_cushion_and_call(bthread_routine fn, bthread_state* state);
void* bthread1(void* arg);
void* bthread2(void* arg);
void* bthread3(void* arg);
void* bthread4(void* arg);

typedef struct bthread {
    jmp_buf buf;
    bthread_state state;
} Bthread;

jmp_buf main_buf;
Bthread bthreads[4];

void initialize_threads() {
    for (int i = 0; i < 4; ++i) {
        bthreads[i].state = __BTHREAD_UNINITIALIZED;
    }
}

void create_cushion_and_call(bthread_routine fn, bthread_state* state)
{
    char cushion[CUSHION_SIZE];
    cushion[CUSHION_SIZE-1] = cushion[0];
    *state = __BTHREAD_READY;
    fn(NULL);
}

jmp_buf *next_bthread(int index)
{
    for (int i = 1; i <= 4; ++i) {
        int next_index = (index + i) % 4;
        if(bthreads[next_index].state != __BTHREAD_DEAD)
            return &bthreads[next_index].buf;
    }
    return &main_buf;
}

void* bthread1(void* arg)
{
    volatile int i;
    for(i=0;i<10000;i++) {
        printf("BThread1, i=%d\n", i);
        /* Yield to next bthread */
        if (!save_context(bthreads[0].buf)) {
            if (bthreads[1].state == __BTHREAD_UNINITIALIZED) {
                create_cushion_and_call(bthread2, &bthreads[1].state);
            } else {
                restore_context(*next_bthread(0));
            }
        }
    }
    bthreads[0].state = __BTHREAD_DEAD;
    restore_context(*next_bthread(0));
}

void* bthread2(void* arg)
{
    volatile int i;

    for(i=0;i<10000;i++) {
        printf("BThread2, i=%d\n", i);

        /* Yield to next bthread */
        if (!save_context(bthreads[1].buf)) {
            if (bthreads[2].state == __BTHREAD_UNINITIALIZED) {
                create_cushion_and_call(bthread3, &bthreads[2].state);
            } else {
                restore_context(*next_bthread(1));
            }
        }
    }
    bthreads[1].state = __BTHREAD_DEAD;
    restore_context(*next_bthread(1));
}

void* bthread3(void* arg)
{
    volatile int i;
    for(i=0;i<1000;i++) {
        printf("BThread3, i=%d\n", i);

        /* Yield to next bthread */
        if (!save_context(bthreads[2].buf)) {
        // We assume that bthread1 is already initialized
            if (bthreads[3].state == __BTHREAD_UNINITIALIZED) {
                create_cushion_and_call(bthread4, &bthreads[3].state);
            } else {
                restore_context(*next_bthread(2));
            }
        }
    }
    bthreads[2].state = __BTHREAD_DEAD;
    restore_context(*next_bthread(2));
}

void* bthread4(void* arg)
{
    volatile int i;
    for(i=0;i<10000;i++) {
        printf("BThread4, i=%d\n", i);

        if(i % 5 == 0) {
            /* Yield to next bthread */
            if (!save_context(bthreads[3].buf)) {
                // We assume that bthread1 is already initialized
                restore_context(*next_bthread(3));
            }
        }

    }
    bthreads[3].state = __BTHREAD_DEAD;
    restore_context(*next_bthread(3));
}

int main()
{
    initialize_threads();
   if (!save_context(main_buf)) {
       create_cushion_and_call(bthread1, &bthreads[0].state);
    }
    return 0;
}

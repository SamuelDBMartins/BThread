#include <setjmp.h>
#include <stdio.h>

#define CUSHION_SIZE 10000
#define save_context(CONTEXT) setjmp(CONTEXT)
#define restore_context(CONTEXT) longjmp(CONTEXT, 1)

typedef enum { __BTHREAD_UNINITIALIZED, __BTHREAD_READY } bthread_state;
typedef void *(*bthread_routine) (void *);

void create_cushion_and_call(bthread_routine fn, bthread_state* state);
void* bthread1(void* arg);
void* bthread2(void* arg);
void* bthread3(void* arg);
void* bthread4(void* arg);

jmp_buf main_buf, bthread1_buf, bthread2_buf, bthread3_buf, bthread4_buf;
bthread_state bthread1_state = __BTHREAD_UNINITIALIZED;
bthread_state bthread2_state = __BTHREAD_UNINITIALIZED;
bthread_state bthread3_state = __BTHREAD_UNINITIALIZED;
bthread_state bthread4_state = __BTHREAD_UNINITIALIZED;

void create_cushion_and_call(bthread_routine fn, bthread_state* state)
{
    char cushion[CUSHION_SIZE];
    cushion[CUSHION_SIZE-1] = cushion[0];
    *state = __BTHREAD_READY;
    fn(NULL);
}

void* bthread1(void* arg)
{
    volatile int i;
    for(i=0;i<10000;i++) {
        printf("BThread1, i=%d\n", i);
        /* Yield to next bthread */
        if (!save_context(bthread1_buf)) {
            if (bthread2_state == __BTHREAD_UNINITIALIZED) {
                create_cushion_and_call(bthread2, &bthread2_state);
            } else {
                restore_context(bthread2_buf);
            }
        }
    }
}

void* bthread2(void* arg)
{
    volatile int i;

    for(i=0;i<10000;i++) {
        printf("BThread2, i=%d\n", i);

        /* Yield to next bthread */
        if (!save_context(bthread2_buf)) {
            if (bthread3_state == __BTHREAD_UNINITIALIZED) {
                create_cushion_and_call(bthread3, &bthread3_state);
            } else {
                restore_context(bthread3_buf);
            }
        }
    }
}

void* bthread3(void* arg)
{
    volatile int i;
    for(i=0;i<1000;i++) {
        printf("BThread3, i=%d\n", i);

        /* Yield to next bthread */
        if (!save_context(bthread3_buf)) {
        // We assume that bthread1 is already initialized
            if (bthread4_state == __BTHREAD_UNINITIALIZED) {
                create_cushion_and_call(bthread4, &bthread4_state);
            } else {
                restore_context(bthread4_buf);
            }
        }
    }
    restore_context(main_buf);
}

void* bthread4(void* arg)
{
    volatile int i;
    for(i=0;i<10000;i++) {
        printf("BThread4, i=%d\n", i);

        if(i % 5 == 0) {
            /* Yield to next bthread */
            if (!save_context(bthread4_buf)) {
                // We assume that bthread1 is already initialized
                restore_context(bthread1_buf);
            }
        }

    }
}

void main()
{
   if (!save_context(main_buf)) {
       create_cushion_and_call(bthread1, &bthread1_state);
    }

}
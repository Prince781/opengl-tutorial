#include "gl_program.h"

void prog1_init(void);
void prog1_uninit(void);
void prog1_render(void);

void prog2_init(void);
void prog2_uninit(void);
void prog2_render(void);

void prog3_init(void);
void prog3_uninit(void);
void prog3_render(void);

void prog4_init(void);
void prog4_uninit(void);
void prog4_render(void);

void prog5_init(void);
void prog5_uninit(void);
void prog5_render(void);

void prog6_init(void);
void prog6_uninit(void);
void prog6_render(void);

struct GLProgram programs[] = {
    { &prog1_init, &prog1_uninit, &prog1_render },
    { &prog2_init, &prog2_uninit, &prog2_render },
    { &prog3_init, &prog3_uninit, &prog3_render },
    { &prog4_init, &prog4_uninit, &prog4_render },
    { &prog5_init, &prog5_uninit, &prog5_render },
    { &prog6_init, &prog6_uninit, &prog6_render }
};

const int num_programs = sizeof(programs)/sizeof(programs[0]);

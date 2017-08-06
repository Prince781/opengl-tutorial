#ifndef GL_PROGRAM_H
#define GL_PROGRAM_H

struct GLProgram {
    void (*init)(void);
    void (*uninit)(void);
    void (*render)(void);
};

extern struct GLProgram programs[];
extern const int num_programs;

#endif

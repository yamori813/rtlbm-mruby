#include <asm/ptrace.h>

struct irqaction {
        void (*handler)(int, void *, struct pt_regs *);
        void *dev_id;
};

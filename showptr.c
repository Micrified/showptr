#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/mman.h>
#include <ucontext.h>

// Page size. 
int pagesize;

// Pointer to allocated page.
void *page;

// Configures a signal handler, then returns it's pointer. Use for ONE signal.
void setHandler (int signal, void (*f)(int, siginfo_t *, void *)) {
	static struct sigaction handler;		
	handler.sa_flags = SA_SIGINFO;			// Set to use signal-handler.
	sigemptyset(&handler.sa_mask);			// Block no signals during handler.
	handler.sa_sigaction = f;				// Assign handler function.

	// Specify signal action: Verify success.
	if (sigaction(signal, &handler, NULL) == -1) {
		fprintf(stderr, "Error: sigaction(%d, %p, NULL) failed: %s\n", 
			signal, &handler, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

// Wrapper for mprotect. Exists with error on failure.
void safeProtect (void *addr, size_t size, int perms) {
	if (mprotect(addr, size, perms) == -1) {
		fprintf(stderr, "Error: mprotect(%p, %zu, %d) failed. Reason: %s\n", 
			addr, size, perms, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

// Handler: Segmentation Fault.
void handler (int signal, siginfo_t *info, void *ucontext) {
    printf("Segmentation Fault:\n\tsi_ptr:\t\t%p\n\tsi_addr:\t%p\n",
        info->si_ptr, info->si_addr);
	ucontext_t *context = (ucontext_t *)ucontext;
	printf("\tInstruction:\t%p\n", (void *)context->uc_mcontext.gregs[REG_RIP]);

	printf("Returning write permissions...\n");
	safeProtect(info->si_addr, pagesize, PROT_WRITE);
}


int main (void) {

    // Set page size.
    pagesize = sysconf(_SC_PAGE_SIZE);

    // Configure handler.
    setHandler(SIGSEGV, handler);

    // Allocate page.
    if (posix_memalign(&page, pagesize, pagesize) != 0) {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Protect page.
    safeProtect(page, pagesize, PROT_READ);

    // Do something bad.
    int *p = (int *)page; 
	
	*p = 0; // Set BREAK here.

    // Free page.
    free(page);

    return 0;
}
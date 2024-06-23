#include "loader.h"
#include <signal.h>
#define _XOPEN_SOURCE 700

Elf32_Ehdr *ehdr;
int fd;
int total_page_faults = 0;
int total_page_allocations = 0;
int total_internal_fragmentation = 0;

void handle_segmentation_fault(int signo, siginfo_t *info, void *context) 
{
    if(signo==SIGSEGV)
    {
        // Handle a segmentation fault here
        printf("once%p\n",info->si_addr);
        total_page_faults++;

        // Get the faulting address
        void *addr=info->si_addr;
        // printf("%p\n",addr);
        // void *fault_address = (void *)(((int)&signo) - 1);

        // Calculate the page start address
        // void *page_start = (void *)((int)fault_address & ~(4096 - 1));

        // Determine the size of the segment containing the fault address
        Elf32_Phdr phdr;
        lseek(fd, ehdr->e_phoff, SEEK_SET);
        for (int i = 0; i < ehdr->e_phnum; i++) 
        {
            // printf("befr\n");
            read(fd, &phdr, sizeof(Elf32_Phdr));
            // printf("afr\n");

            printf("%d < ",(int)phdr.p_vaddr);
            printf("%d < ",(int)addr);
            printf("%d \n",(int)(phdr.p_vaddr + phdr.p_memsz));

            if (phdr.p_vaddr <= (int)addr && (int)addr < (phdr.p_vaddr + phdr.p_memsz)) 
            {
                printf("in loop2\n");
                // Allocate memory for the segment
                // int segment_size = phdr.p_filesz;
                int size=phdr.p_memsz;
                void *allocd=mmap(phdr.p_vaddr,size,PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
                // void *segment_memory = mmap(page_start, segment_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);

                if (allocd == MAP_FAILED) 
                {
                    perror("mmap");
                    exit(1);
                }

                // Read segment data from the ELF file and copy it to the allocated memory
                lseek(fd, phdr.p_offset, SEEK_SET);
                read(fd, allocd, phdr.p_filesz);

                total_page_allocations++;
                total_internal_fragmentation += size - phdr.p_filesz;

                // Resume execution by returning
                return;
            }
            // printf("idk%d %d\n",ehdr->e_phentsize - sizeof(Elf32_Phdr)+ehdr->e_phoff,ehdr->e_phoff+(i*ehdr->e_phentsize));
            // lseek(fd, ehdr->e_phentsize - sizeof(Elf32_Phdr), SEEK_CUR);
            lseek(fd,ehdr->e_phoff+(i*ehdr->e_phentsize),SEEK_SET);
        }

        // If no segment is found, exit with an error
        printf("Segmentation fault at address %p\n", addr);
        exit(1);
    }
}


void loader_cleanup() {
    free(ehdr);
    close(fd);
}

void load_and_run_elf(char** exe) 
{
    fd = open(exe[1], O_RDONLY);
    ehdr = (Elf32_Ehdr*)malloc(sizeof(Elf32_Ehdr));
    int sz = read(fd, ehdr, sizeof(Elf32_Ehdr));

    if (sz != sizeof(Elf32_Ehdr)) {
        printf("File not read correctly\n");
        exit(0);
    }

    // Set up a signal handler for segmentation faults
    struct sigaction sa;
    memset(&sa, 0, sizeof(sigaction));
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handle_segmentation_fault;
    if (sigaction(SIGSEGV, &sa, NULL) == -1) 
    {
        perror("sigaction");
        exit(0);
    }
    printf("idk\n");
    // Run the _start method (may generate a segmentation fault)
    int (*start_func)() = (int(*)())(ehdr->e_entry);
    printf("idk\n");
    int result = start_func();
    printf("done\n");
    
    printf("User _start return value = %d\n",result);

    // Report statistics
    printf("Total page faults: %d\n", total_page_faults);
    printf("Total page allocations: %d\n", total_page_allocations);
    printf("Total internal fragmentation (KB): %d\n", total_internal_fragmentation);

    loader_cleanup();
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <ELF Executable>\n", argv[0]);
        exit(1);
    }

    load_and_run_elf(argv);
    return 0;
}

#include "loader.h"
#include <signal.h>
#define _XOPEN_SOURCE 700

Elf32_Ehdr *ehdr;
int fd;
int total_page_faults = 0;
int total_page_allocations = 0;
int total_internal_fragmentation = 0;
int k=0;

void handle_segmentation_fault(int signo, siginfo_t *info, void *context) 
{
    printf("In handler\n");
    if(signo==SIGSEGV)
    {
        printf("segfault addressP: %p\n",info->si_addr);
        total_page_faults++;
        void *addr=info->si_addr;
        Elf32_Phdr phdr;
        lseek(fd, ehdr->e_phoff, SEEK_SET);
        for (int i = 0; i < ehdr->e_phnum; i++) 
        {
            read(fd, &phdr, sizeof(Elf32_Phdr));
            if (phdr.p_vaddr <= (int)addr && (int)addr < (phdr.p_vaddr + phdr.p_memsz)) 
            {
                int size=phdr.p_memsz;
                void *allocd=mmap(phdr.p_vaddr+(k*4096),4096,PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
                k++;
                if (allocd == MAP_FAILED) {
                    perror("mmap");
                    exit(1);
                }
                lseek(fd, phdr.p_offset, SEEK_SET);
                // printf("idk\n");
                read(fd, allocd, phdr.p_filesz);
                total_page_allocations++;
                if(phdr.p_memsz%4096==0)
                {
                    total_internal_fragmentation=0;
                }
                else
                {
                    total_internal_fragmentation=4096-(phdr.p_memsz%4096);
                }
                return;
            }
            lseek(fd,ehdr->e_phoff+(i*ehdr->e_phentsize),SEEK_SET);
        }
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
    int (*start_func)() = (int(*)())(ehdr->e_entry);
    int result = start_func();
    printf("done\n");
    
    printf("User _start return value = %d\n",result);

    // Report statistics
    printf("Total page faults: %d\n", total_page_faults);
    printf("Total page allocations: %d\n", total_page_allocations);
    printf("Total internal fragmentation: %d\n", total_internal_fragmentation);

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

#include "loader.h"

Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
int fd;

// #include <errno.h>
void print_elf32_ehdr(Elf32_Ehdr *ehdr) {
    printf("ELF Header:\n");
    printf("  e_ident:     %c%c%c%c\n", ehdr->e_ident[0], ehdr->e_ident[1], ehdr->e_ident[2], ehdr->e_ident[3]);
    printf("  e_type:      0x%x\n", ehdr->e_type);
    printf("  e_machine:   0x%x\n", ehdr->e_machine);
    printf("  e_version:   0x%x\n", ehdr->e_version);
    printf("  e_entry:     0x%x\n", ehdr->e_entry);
    printf("  e_phoff:     0x%x\n", ehdr->e_phoff);
    printf("  e_shoff:     0x%x\n", ehdr->e_shoff);
    printf("  e_flags:     0x%x\n", ehdr->e_flags);
    printf("  e_ehsize:    %u\n", ehdr->e_ehsize);
    printf("  e_phentsize: %u\n", ehdr->e_phentsize);
    printf("  e_phnum:     %u\n", ehdr->e_phnum);
    printf("  e_shentsize: %u\n", ehdr->e_shentsize);
    printf("  e_shnum:     %u\n", ehdr->e_shnum);
    printf("  e_shstrndx:  %u\n", ehdr->e_shstrndx);
    printf("\n");
}
void print_elf32_phdr(Elf32_Phdr *phdr) {
    printf("Program Header:\n");
    printf("  p_type:   0x%x\n", phdr->p_type);
    printf("  p_offset: 0x%x\n", phdr->p_offset);
    printf("  p_vaddr:  0x%x\n", phdr->p_vaddr);
    printf("  p_paddr:  0x%x\n", phdr->p_paddr);
    printf("  p_filesz: %u\n", phdr->p_filesz);
    printf("  p_memsz:  %u\n", phdr->p_memsz);
    printf("  p_flags:  0x%x\n", phdr->p_flags);
    printf("  p_align:  %u\n", phdr->p_align);
    printf("\n");
}
void print_strings_in_form(char **strings, int num_strings) {
    for (int i = 0; i < num_strings; i++) 
    {
      printf("\n%s\n", strings[i]);
    }
}
/*
 * release memory and other cleanups
 */
void loader_cleanup() {
    // Clean up any resources, such as closing the file descriptor
  free(phdr);
  free(ehdr);
  close(fd);
}
// extern int _start();
int check_cond(Elf32_Phdr *phdr,Elf32_Ehdr *ehdr)
{
  if((ehdr->e_entry > phdr->p_vaddr) && (ehdr->e_entry <  phdr->p_vaddr+phdr->p_memsz))
  {
    return 1;
  }
  return 0;
}
void load_and_run_elf(char** exe) 
{
  // Open the ELF executable file for reading
  // printf("\n%s\n",*exe);
  // print_strings_in_form(exe,sizeof(exe)/sizeof(exe[0]));
  // printf("exe values:%s %s\n",exe[0],exe[1]);
  fd = open(exe[1], O_RDONLY);
  // Read ELF header
  ehdr = (Elf32_Ehdr*)malloc(sizeof(Elf32_Ehdr));
  phdr = (Elf32_Phdr *)malloc(sizeof(Elf32_Phdr));
  int sz=read(fd,ehdr,sizeof(Elf32_Ehdr)); //size read
  // print_elf32_ehdr(ehdr);
  if(sz != sizeof(Elf32_Ehdr))
  {
    printf("File not read correctly\n");
    exit(0);
  }
  lseek(fd,ehdr->e_phoff,SEEK_SET);
  // Iterate through program headers to find PT_LOAD segment

  read(fd,phdr,sizeof(Elf32_Phdr));
  int i = 0;
  while (i < ehdr->e_phnum)
  {
    if (phdr->p_type == PT_LOAD)
    {
      if (check_cond(phdr,ehdr)==1)
      {
      // Allocate memory for the segment
        void* vmem = mmap((void*)phdr->p_vaddr,
  phdr->p_memsz,
                PROT_READ|PROT_WRITE|PROT_EXEC,
     MAP_PRIVATE|MAP_FIXED,
             fd,
          phdr->p_offset
        );

        // printf("%u\n",ehdr->e_entry);
        // printf("%u\n",phdr->p_vaddr);
        // printf("%u\n",phdr->p_memsz);

        // Calculate the function pointer for the entry point
        // int (*start_func)() = NULL;
        // printf("\ncode\n");
        // for (int i = 0; i < phdr->p_memsz / sizeof(int); i++)
        // {
        //   int *addr = vmem + i;
        //   if (*addr == ehdr->e_entry)
        //   {
        //     printf("\nKAN%d\n",*addr);
        //     printf("KAN%d",(ehdr->e_entry ));
        //     printf("\ncode\n");
        //     start_func = (int (*)())addr;
        //     break;
        //   }
        //   // printf("\nKAN\n");
        // }
        //USE OF ABOVE IS TO GET TO EHDR->E_ENTRY ONLY TF MAN!!!
        
        int (*start_func)() = (int(*)())(ehdr->e_entry);

        // printf("\ncode\n");
        // printf("herh\n");
        int result = start_func();
        // printf("\ncode\n");
        printf("User _start return value = %d\n", result);
        
        // Clean up resources
        // free(phdr);
        munmap(vmem, phdr->p_memsz);
        break; // We only need to execute one PT_LOAD segment
      }
      //to seek to entrysize+offset
      lseek(fd,ehdr->e_phoff+(i*ehdr->e_phentsize),SEEK_SET);
      read(fd,phdr,sizeof(Elf32_Phdr));
    }
    i++;
  }
}

int main(int argc, char** argv) 
{
    if (argc != 2)
    {
      printf("Usage: %s <ELF Executable>\n", argv[0]);
      exit(1);
    }
    load_and_run_elf(argv);
    loader_cleanup();
    return 0;
}
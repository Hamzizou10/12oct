#define _DEFAULT_SOURCE
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include "malloc.h"
#define LENGTH 4053


struct metadata *metadata = NULL;


static void *init_page(size_t size)
{
    metadata = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    size_t new_size = LENGTH;
    while (size >= new_size)
    {
        metadata = mremap(metadata, new_size, new_size*2, MREMAP_MAYMOVE, metadata);
        new_size *= 2;
    }
    metadata->free = 0; 
    metadata->size = size;
    void *tmp = metadata;
    char *tmp2 = tmp;
    tmp2 += sizeof (struct metadata) + size;
    tmp = tmp2;
    metadata->next = tmp;
    metadata->next->size = LENGTH - size;
    metadata->next->free = 1;
    metadata->next->next = NULL;
    return metadata +1;
}

static void *add_page(struct metadata *sentinel, size_t size)
{
    sentinel->next = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    sentinel = sentinel->next;
    sentinel->free = 0;
    void *tmp = sentinel;
    char *tmp2 = tmp;
    tmp2 += sizeof (struct metadata) + size;
    tmp = tmp2;
    sentinel->next = tmp;
    sentinel->next->free = 1;
    sentinel->next->size = LENGTH - size;
    sentinel->size = size;
    sentinel->next->next = NULL;
    return sentinel + 1;
}


static void *search_free(size_t size)
{
    int i = 0;
    struct metadata *sentinel = metadata;
    for (; sentinel->next; i++)
    {
        if (sentinel->free)
        {
            if (sentinel->size >= size)
                break;
        }
        sentinel = sentinel->next;
    }
    if (sentinel->next == NULL && sentinel->size < size)
        return add_page(sentinel, size);

    sentinel->free = 0;
    void *tmp = sentinel;
    char *tmp2 = tmp;
    tmp2 += sizeof (struct metadata) + size;
    tmp = tmp2;
    sentinel->next = tmp;
    sentinel->next->free = 1;
    sentinel->next->size = sentinel->size - size;
    sentinel->size = size;
    sentinel->next->next = NULL;
    return sentinel + 1;
}
__attribute__((__visibility__("default")))
void *malloc(size_t size)
{
    if (!metadata)
    {
        return init_page(size);
    }
    return  search_free(size);
}

void free(void *ptr)
{
    
}


int main(void)
{
    printf("struct size: %zu\n", sizeof (struct metadata));
    printf("page size: %ld\n", sysconf(_SC_PAGESIZE));
    char *str = malloc(5);
    int *tab = malloc(sizeof(int)*5);
    char *str2 = malloc(5);
    str2[0] = 'l';
    for (int i = 0; i < 5; i++)
        str[i] = 'a';

    for (int i = 0; i < 10; i++)
        tab[i] = 1996;
    str[5] = '\0';
    tab[5] = 0;
    for (int i = 0; str[i]; i++)
        printf("%c ", str[i]);

    putchar('\n');
    for (int i = 0; tab[i]; i++)

        printf("%d ", tab[i]);
    putchar('\n');
    return 1;
}

#define _DEFAULT_SOURCE
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include "malloc.h"
#define LENGTH 4096


struct metadata *metadata = NULL;


static void *init_page(size_t size)
{
    size_t h = 1;
    size_t i = size;
    while (i >= LENGTH)
    {
        i -= LENGTH;
        h++;
    }
    if (size < 4096 && size >= 4048)
        metadata = mmap(0, size * 2, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    else
        metadata = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    metadata->free = 0; 
    metadata->size = size;
    void *tmp = metadata;
    char *tmp2 = tmp;
    tmp2 += sizeof (struct metadata) + size;
    tmp = tmp2;
    metadata->next = tmp;
    metadata->next->size = (LENGTH * h) - metadata->size - (sizeof(struct metadata) * 2);
    metadata->next->free = 1;
    metadata->next->next = NULL;
    return metadata +1;
}

static void *add_page(struct metadata *sentinel, size_t size)
{
    size_t h = 1;
    size_t i = size;
    for (; i >= LENGTH; h++)
        i -= LENGTH;
    if (size >= 4053 && size <= 4096)
        sentinel->next = mmap(0, size * 2, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);     
    else
    sentinel->next = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    sentinel = sentinel->next;
    sentinel->free = 0;
    void *tmp = sentinel;
    char *tmp2 = tmp;
    tmp2 += sizeof (struct metadata) + size;
    tmp = tmp2;
    sentinel->next = tmp;
    sentinel->size = size;
    sentinel->next->free = 1;
    sentinel->next->size = (LENGTH * h) - sentinel->size - (sizeof (struct metadata)) * 2;
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
            if (sentinel->size >= size + sizeof (struct metadata) + 1)
                break;
        }
        sentinel = sentinel->next;
    }
    if (sentinel->next == NULL && sentinel->size  < size + sizeof (struct metadata) + 1)
        return add_page(sentinel, size);

    sentinel->free = 0;
    if (!sentinel->next)
    {
        void *tmp = sentinel;
        char *tmp2 = tmp;
        tmp2 += sizeof (struct metadata) + size;
        tmp = tmp2;
        sentinel->next = tmp;
        sentinel->next->free = 1;
        sentinel->next->size = sentinel->size - size - sizeof (struct metadata);
    sentinel->next->next = NULL;
    }
    sentinel->size = size;
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
__attribute__((__visibility__("default")))
void free(void *ptr)
{
    if (!ptr)
        return;
    struct metadata *sentinel;
    char *tmp = ptr;
    tmp -= sizeof (struct metadata);
    ptr = tmp;
    sentinel = ptr;
    sentinel->free = 1;
    if (!sentinel->next->next)
    {
        sentinel->size += sentinel->next->size;
        sentinel->next = NULL;
    }
    else
        sentinel->size = (sentinel->next - sentinel);
}
__attribute__((__visibility__("default")))
void *calloc(size_t nmemb, size_t size)
{
    void *tmp = malloc(nmemb * size);
    tmp = memset(tmp, 0, nmemb * size);
    return tmp;
}

__attribute__((__visibility__("default")))
void *realloc(void *ptr, size_t size)
{
    char *tmp = ptr;
    tmp -= sizeof(struct metadata);
    void *tmp2 = tmp;
    struct metadata *sentinel = tmp2;
    size_t old_size = sentinel->size;
    if (sentinel->size >= size)
        return ptr;
    if (sentinel->next->free && sentinel->next->size + old_size >= size + 24)
    {
        sentinel->size = size;
        tmp2 = sentinel->next;
        tmp = tmp2;
        tmp += size - old_size;
        tmp2 = tmp;
        size_t next_size = sentinel->next->size;
        sentinel->next = tmp2;
        sentinel->next->size = next_size - (size - old_size);
        sentinel->next->free = 1;
    }
    else 
    {
        free(ptr);
        ptr = malloc(size);
        memcpy(ptr, sentinel, old_size);
    }
    return ptr;
}

int main(void)
{
    char *str = malloc(200);
    char *str2 = malloc(200);
    char *str3 = malloc(200);
    struct metadata *sentinel = metadata;
    while (sentinel)
    {
        void *tmp = sentinel;
        printf("free: %d, size: %zu, add: %p\n", sentinel->free, sentinel->size, tmp);
        sentinel = sentinel->next;
    }
    free(str2);
    str2 =  malloc(20);
    free(str2);
    //str = realloc(str, 5000);
    printf("FREE\n");
    sentinel = metadata;
    while (sentinel)
    {
        void *tmp = sentinel;
        printf("free: %d, size: %zu, add: %p\n", sentinel->free, sentinel->size, tmp);
        sentinel = sentinel->next;
    }
    str[0] = 'a';
    str2[0] = 'a';
    str3[0] = 'b';
    return 1;
}

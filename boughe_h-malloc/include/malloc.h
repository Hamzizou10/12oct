#ifndef MALLOC_H
# define MALLOC_H


struct metadata
{
    char free;
    size_t  size;
    struct metadata *next;
};

#endif /* !MALLOC_H */

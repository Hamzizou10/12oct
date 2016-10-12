#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

static void check_malloc(void)
{
  // Allocate.
  int *mem = malloc(sizeof (int));
  assert(mem && "Malloc failed.");
  // Write.
  *mem = 42;
  // Read.
  assert(*mem == 42 && "Read of written value failed.");

  // Allocate more.
  int *arr = malloc(100 * sizeof (int));
  assert(arr && "Malloc failed.");
  // Write.
  for (int i = 0; i < 100; ++i)
    arr[i] = i;

  // Check for corruption of old blocks.
  assert(*mem == 42 && "Allocation corrupted old blocks.");
  // Read.
  for (int i = 0; i < 100; ++i)
    assert(arr[i] == i && "Read of written value failed.");
}

static void check_free(void)
{
  // Allocate.
  int *mem = malloc(sizeof (int));
  assert(mem && "Malloc failed.");
  // Write.
  *mem = 42;
  // Read.
  assert(*mem == 42 && "Read of written value failed.");

  // Allocate more.
  int *arr = malloc(100 * sizeof (int));
  assert(arr && "Malloc failed.");
  // Write.
  for (int i = 0; i < 100; ++i)
    arr[i] = i;

  // Check for corruption of old blocks.
  assert(*mem == 42 && "Allocation corrupted old blocks.");
  // Read.
  for (int i = 0; i < 100; ++i)
    assert(arr[i] == i && "Read of written value failed.");
  // Free.
  free(arr);

  // Free.
  free(mem);
}

static void check_realloc(void)
{
  // Allocate.
  int *mem = malloc(sizeof (int));
  assert(mem && "Malloc failed.");
  // Write.
  *mem = 42;
  // Read.
  assert(*mem == 42 && "Read of written value failed.");
  // Reallocate.
  int *new = realloc(mem, 2 * sizeof (int));
  assert(new && "Realloc failed");
  // Write again.
  *(new + 1) = 43;
  assert(*new == 42 && "Read of written value failed.");
  assert(*(new + 1) == 43 && "Read of written value failed.");

  // Allocate more.
  int *arr = malloc(100 * sizeof (int));
  assert(arr && "Malloc failed.");
  // Write.
  for (int i = 0; i < 100; ++i)
    arr[i] = i;

  // Reallocate the double.
  int *new_arr = realloc(arr, 200 * sizeof (int));
  assert(new_arr && "Realloc failed.");
  // Write the rest.
  for (int i = 100; i < 200; ++i)
    new_arr[i] = i;

  // Check for corruption of old blocks.
  assert(*new == 42 && "Allocation corrupted old blocks.");
  assert(*(new + 1) == 43 && "Allocation corrupted old blocks.");

  // Read.
  for (int i = 0; i < 200; ++i)
    assert(new_arr[i] == i && "Read of written value failed.");
}

static void check_calloc(void)
{
  // Allocate.
  int *mem = calloc(1, sizeof (int));
  assert(mem && "Malloc failed.");
  // Check for 0-filled.
  assert(*mem == 0 && "Calloc did not zero-fill the array.");
  // Write.
  *mem = 42;
  // Read.
  assert(*mem == 42 && "Read of written value failed.");

  // Allocate more.
  int *arr = calloc(100, sizeof (int));
  assert(arr && "Malloc failed.");
  // Check for 0-filled.
  for (int i = 0; i < 100; ++i)
    assert(arr[i] == 0 && "Read of written value failed.");
  // Write.
  for (int i = 0; i < 100; ++i)
    arr[i] = i;

  // Check for corruption of old blocks.
  assert(*mem == 42 && "Allocation corrupted old blocks.");
  // Read.
  for (int i = 0; i < 100; ++i)
    assert(arr[i] == i && "Read of written value failed.");
}

int main(void)
{
  check_malloc();
  printf("Malloc: \t\033[32mOK\033[0m\n");
  check_free();
  printf("Free: \t\t\033[32mOK\033[0m\n");
  check_realloc();
  printf("Realloc: \t\033[32mOK\033[0m\n");
  check_calloc();
  printf("Calloc: \t\033[32mOK\033[0m\n");
}

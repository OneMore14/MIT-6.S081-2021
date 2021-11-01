// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);
void do_kfree(void *pa, int init);
extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

int refCount[PGROUNDUP(PHYSTOP) / PGSIZE];

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE) {
    do_kfree(p, 1);
  }
}


void
do_kfree(void *pa, int init) {

  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  r = (struct run*)pa;

  acquire(&kmem.lock);
  if (!init) {
    if (refCount[PA2PGID(pa)] > 0) {
      refCount[PA2PGID(pa)] -= 1;
    }
    if (refCount[PA2PGID(pa)] > 0) {
      release(&kmem.lock);
      return;
    }
  } else {
    refCount[PA2PGID(pa)] = 0;
  }
  release(&kmem.lock);
  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  
  release(&kmem.lock);
}
// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  do_kfree(pa, 0);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r) {

    memset((char*)r, 5, PGSIZE); // fill with junk
    addRefCount(r);
  }

  return (void*)r;
}

void 
addRefCount(void *pa) {

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("addRefCount");
    
  acquire(&kmem.lock);
  refCount[PA2PGID(pa)] += 1;
  release(&kmem.lock);
}

// #pragma clang diagnostic ignored "-Wdeprecated-declarations"

#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#define PAGE_SIZE 4096

typedef unsigned char BYTE;

typedef struct chunkhead
{
    int size;
    int inuse;
    BYTE *next, *prev;
} chunkhead;

void *startofheap;

/*
    findSmallestChunk
    finds and returns the smallest suitable chunk
*/
chunkhead *findSmallestChunk(int pages, chunkhead *firstSuitableChunk)
{
    chunkhead *current = firstSuitableChunk;
    chunkhead *min = firstSuitableChunk;

    while (current)
    {
        // check if:
        // chunk is not inuse &,
        // chunk is large enough &,
        // chunk is smaller than the current smalles suitable chunk
        if (current->inuse == 0 && pages <= current->size && current->size < min->size)
            min = current;

        current = (chunkhead *)(current->next);
    }
    return min;
}

/*
    combineChunk method
    combine first chunk with second chunk
*/
chunkhead *combineChunks(chunkhead *first, chunkhead *second)
{
    // check for edge case :
    // if combining the last element with the first element,
    // (meaning only 2 elements in list),
    // uninitialize startofheap,
    // (empty heap)
    if (first == (chunkhead *)startofheap && !(second->next))
        startofheap = NULL;

    first->next = second->next;
    first->size += second->size;

    if (second->next)
        ((chunkhead *)(second->next))->prev = (BYTE *)first;

    return first;
}

/*
    mymalloc method
*/
BYTE *mymalloc(unsigned long size)
{
    int pages = ((size + sizeof(chunkhead)) / PAGE_SIZE) + 1;

    // if heap is empty,
    // initialize startofheap,
    // move Program Break,
    // create first chunk,
    // store chunk on the heap
    if (!startofheap)
    {
        startofheap = sbrk(pages * PAGE_SIZE);
        chunkhead firstChunk = {pages, 1, NULL, NULL};
        memcpy(startofheap, &firstChunk, sizeof(chunkhead));
        return (BYTE *)(startofheap + sizeof(chunkhead));
    }
    else
    {
        chunkhead *current = (chunkhead *)startofheap;
        while ((current->inuse == 1 || current->size < pages) && current->next != NULL)
            current = (chunkhead *)current->next;

        // if a suitable unused chunk was found,
        // search the rest of list for a smaller suitable chunk
        if (current->inuse == 0)
        {
            current = findSmallestChunk(pages, current);

            // if most suitable node is larger than it needs to be,
            // break off a chunk of desired size
            if(current -> size > pages)
            {
                chunkhead emptyChunk = {current -> size - pages, 0, current -> next, current};
                memcpy((BYTE*)current + pages * PAGE_SIZE, &emptyChunk, sizeof(chunkhead));
                current -> size = pages;
                current -> next = (BYTE*)current + pages * PAGE_SIZE;
            }
            current->inuse = 1;
            return (BYTE *)((BYTE *)current + sizeof(chunkhead));
        }

        // iterated to end of list,
        // need to move Program Break,
        // create a new chunk,
        // store chunk on the heap
        else
        {

            chunkhead newChunk = {pages, 1, NULL, (BYTE *)current};
            current->next = (BYTE *)sbrk(pages * PAGE_SIZE);
            memcpy(current->next, &newChunk, sizeof(chunkhead));
            return (BYTE *)((current->next) + sizeof(chunkhead));
        }
    }
    return NULL;
}

/*
    myfree method
*/
BYTE *myfree(BYTE *address)
{
    // if address is not on the heap, return
    if (address < (BYTE *)startofheap || address > (BYTE *)sbrk(0))
        return NULL;

    // get the chunkhead element for the specified address to free
    chunkhead *freeChunk = (chunkhead *)(address - sizeof(chunkhead));
    freeChunk->inuse = 0;

    // if the chunk we are freeing is last chunk in the list,
    // shrink Program Break to free the memory
    if (!(freeChunk->next))
    {
        // if the head chunk is also the last chunk,
        // reset startofheap to NULL
        if (!(freeChunk->prev))
            startofheap = NULL;

        else if (((chunkhead *)freeChunk->prev)->inuse == 0)
            freeChunk = combineChunks(((chunkhead *)freeChunk->prev), freeChunk);

        else
            ((chunkhead *)(freeChunk->prev))->next = NULL;

        sbrk(-1 * (freeChunk->size) * PAGE_SIZE);
        return (BYTE *)sbrk(0);
    }

    // if chunk is not the last element in the list
    else
    {
        // check if the following chunk is inuse,
        // if not, combine current with the following chunk
        if (((chunkhead *)freeChunk->next)->inuse == 0)
            freeChunk = combineChunks(freeChunk, (chunkhead *)freeChunk->next);

        // if current is the head element, do nothing
        if (freeChunk->prev)
        {
            // check if the previous chunk is in use,
            // if not, combine current with the previous chunk
            if (((chunkhead *)freeChunk->prev)->inuse == 0)
                freeChunk = combineChunks(((chunkhead *)freeChunk->prev), freeChunk);
        }

        sbrk(-1 * (freeChunk->size) * PAGE_SIZE);
        return (BYTE *)sbrk(0);
    }
}

/*
    analyze method
*/
void analyze()
{
    printf("\n--------------------------------------------------------------\n");
    if (!startofheap)
    {
        printf("no heap, program break on address: %x\n", sbrk(0));
        return;
    }

    chunkhead *ch = (chunkhead *)startofheap;
    for (int no = 0; ch; ch = (chunkhead *)ch->next, no++)
    {
        printf("%d | current addr: %x |", no, ch);
        printf("size: %d | ", ch->size * PAGE_SIZE);
        printf("info: %d | ", ch->inuse);
        printf("next: %x | ", ch->next);
        printf("prev: %x", ch->prev);
        printf("      \n");
    }
    printf("program break on address: %x\n", sbrk(0));
}

int main()
{

    BYTE*a[100];
    analyze();//50% points

    for(int i=0;i<100;i++)
        a[i]= mymalloc(1000);

    for(int i=0;i<90;i++)
        myfree(a[i]);
    analyze(); //50% of points if this is correct

    myfree(a[95]);
    analyze();
    a[95] = mymalloc(1000);
    analyze();//25% points, this new chunk should fill the smaller free one
    //(best fit)

    for(int i=90;i<100;i++)
        myfree(a[i]);
    analyze();// 25% should be an empty heap now with the start address
    //from the program start
    printf("\n");

    // int possibe_sizes[] = {1000, 6000, 20000, 30000};
    // srand(time(0));
    // BYTE *a[100];
    // analyze();
    // for (int i = 0; i < 100; i++)
    // {
    //     a[i] = mymalloc(possibe_sizes[rand() % 4]);
    // }
    // analyze();
    // int count = 0;
    // for (int i = 0; i < 99; i++)
    // {
    //     if (rand() % 2 == 0)
    //     {
    //         myfree(a[i]);
    //         a[i] = 0;
    //         count++;
    //     }
    //     if (count >= 50)
    //         break;
    // }
    // analyze();
    // for (int i = 0; i < 100; i++)
    // {
    //     if (a[i] == 0)
    //     {
    //         a[i] = mymalloc(possibe_sizes[rand() % 4]);
    //     }
    // }
    // analyze();
    // for (int i = 0; i < 100; i++)
    // {
    //     myfree(a[i]);
    // }
    // analyze();

    return 0;
}
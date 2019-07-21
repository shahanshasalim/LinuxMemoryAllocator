/* file : malloc.c
 * description : implements malloc system call and other necessary functions
 * acknowledgements : 
 * Author :
 * Date Created:
 * Revisions :
 */

//todo : move all define and typedef to common.h

#define FREE_BLOCK -1
#define ALLOCATED_BLOCK 0

#include "../include/common.h"
#include "../include/commonFunctions.h"
#include <assert.h>

/*
+----------------+-------------------------------------+
|free?  |pointer |                                     |
+----------------+                                     |
| Block Meta data|  Actual memory block                |
|                |  (Actual memory requested by the    |
|(This is extra  |  program)                           |
|space used by   |                                     |
|every block)    |                                     |
|                |                                     |
|                |                                     |
+----------------+-----------+-------------------------+
 Total size = Requested Size + Block meta data size
*/

typedef struct BlockMetaData{
	size_t block_size;
	struct BlockMetaData *next;
	int status;
}BlockMeta;

#define BLOCK_META_SIZE sizeof(BlockMeta)

void *head = NULL;

BlockMeta* findCompatibleBlock(BlockMeta *head, size_t request_size)
{
	assert(head != NULL);
	assert(request_size > 0);
	BlockMeta *temp = head;
	while(temp->status != ALLOCATED_BLOCK && NULL != temp){
		if(temp->block_size <= request_size)
			return(temp+1);
		temp = temp->next;
	}
	return(temp);
}

void*  requestToOperatingSystem(size_t size)
{
	/*refer to man page for sbrk */
	//TODO : see if size is greater than threshold to use mmap
	size_t requestSize = size + BLOCK_META_SIZE;
	void *presentBreak = sbrk(0);
	void *allocatedBlock = sbrk((intptr_t)requestSize);
	if((void*)-1 == allocatedBlock)
		return NULL;
	((BlockMeta *)presentBreak)->block_size = requestSize;
	((BlockMeta *)presentBreak)->status = ALLOCATED_BLOCK;
	((BlockMeta *)presentBreak)->next = NULL;
	return(presentBreak);
}

void* u_malloc(size_t size)
{
	assert(size > 0);
	BlockMeta *block = NULL;
	if(NULL == head){ /* this is the first request since program start*/
		head = requestToOperatingSystem(size);
		block = head;
	}
	else{
		BlockMeta *compatibleBlock = NULL;
		compatibleBlock = findCompatibleBlock(head, size);
		if(NULL != compatibleBlock){
			block = compatibleBlock;
		}
		else{
			BlockMeta *last_block = head;
			while(NULL != last_block->next)
				last_block = last_block->next;
			block = requestToOperatingSystem(size);
			last_block->next = block;
		}
	}
	return(block+1);
}


	

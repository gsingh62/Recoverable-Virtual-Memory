/**begin a transaction that will modify the segments listed in segbases. If any of the specified segments
is already being modified by a transaction, then the call should fail and return(trans_t) - 1.*/
trans_t rvm_begin_trans(rvm_t rvm, int numsegs, void **segbases);

/**declare that the library is about to modify a specified range of memory in the specified segment. 
The segment must be one of the segments specified in the call to rvm_begin_trans. Your library needs to ensure
that the old memory has been saved, in case an abort is executed. It is legal call rvm_about_to_modify
multiple times on the same memory area*/
void rvm_about_to_modify(trans_t tid, void * segbase, int offset, int size);

/**commit all changes that have been made within the specified transaction. When the call returns, then enough 
information should have been saved to disk so that, even if the program crashes, the changes will be seen by 
the program when it restarts.*/
void rvm_commit_trans(trans_t tid);

/**undo all changes that have happened within the specified transaction.*/
void rvm_abort_trans(trans_t tid);
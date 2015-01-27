/**begin a transaction that will modify the segments listed in segbases. If any of the specified segments
is already being modified by a transaction, then the call should fail and return(trans_t) - 1.*/
trans_t rvm_begin_trans(rvm_t rvm, int numsegs, void ** segbases){
	int i;
	transaction_t * t_new = (transaction_t*)malloc(sizeof(transaction_t));
	t_new->tid = transactionCounter++;
	rvm.transactions.push_back(t_new);
	std::list<file_t>::iterator file_iter;
	trans_to_rvm[t_new->tid] = rvm;
	
	/**
	* confirm that the segbase exist in the rvm and that it is not already being
	* modified by a transaction. Then add to the list of segbases for the transaction.
	*/
	for(i=0;i<numsegs;i++){
		for(file_iter = (rvm->files).begin();file_iter!=file_list.end();file_iter++){
			if((*file_iter)->fptr == segbases[i] && (*file_iter)->busy == 1){
				return -1;
			}
		}
		(t_new->files).push_back(segbases[i]);
	}
	
	return t_new->tid;
}

/**declare that the library is about to modify a specified range of memory in the specified segment. 
The segment must be one of the segments specified in the call to rvm_begin_trans. Your library needs to ensure
that the old memory has been saved, in case an abort is executed. It is legal call rvm_about_to_modify
multiple times on the same memory area*/
void rvm_about_to_modify(trans_t tid, void * segbase, int offset, int size){
	std::list<file_t>::iterator file_iter;
	rvm_t rvm = trans_to_rvm[tid];
	for(tran_iter =rvm->transactions.begin();tran_iter!=rvm->transactions.end();tran_iter++){
		if(tid==tran_iter->tid){
			about_to_modify = &*tran_iter;
		}
	}
	
	if(about_to_modify){
		file_t file_list = about_to_modify->files;
		for(file_iter = file_list.begin();file_iter!=file_list.end();file_iter++){
			if((*file_iter)->fptr == segbase){
				(*file_iter)->backup_fptr = (*file_iter)->fptr;
				(*file_iter)->busy =1;
			}
		}
	}
	
}

/**commit all changes that have been made within the specified transaction. When the call returns, then enough 
information should have been saved to disk so that, even if the program crashes, the changes will be seen by 
the program when it restarts.*/
void rvm_commit_trans(trans_t tid){
	/**
	* 1. Find the transaction struct related to the tid. This can be found by looking up the 
	* rvm for the tid. the rvm should contain a list of transaction structs.
	* 2. Find the transaction with the given tid.
	* 3. Find all segments related to this transaction.
	* 4. fsync them all.
	* 5. update the log file to contain a history of all changes contained in its segments.
	* 6. fsync the log file.
	*/
	std::list<transaction_t>::iterator tran_iter;
	std::list<segment_t>::iterator seg_iter;
	
	char ch;
	transaction_t * to_commit = NULL;
	rvm_t rvm = trans_to_rvm[tid];
	for(tran_iter = rvm->transactions.begin();tran_iter!=rvm->transactions.end();tran_iter++){
		if(tid==tran_iter->tid)
			to_commit = &*tran_iter;
	}
	if(to_commit){
		file_t file_list = to_commit->files;
		for(file_iter = file_list.begin();file_iter!=file_list.end();file_iter++){
			char * filename = (*file_iter)->filename;
			FILE * fileptr = fopen(filename,"r");
			FILE * log_fd = fopen((rvm->pathname+"/rvmlog").c_str(),"a");
			while((ch = fgetc(fileptr)) != EOF)
				fputc(ch,log_fd);
				
			fsync(fileno(fileptr));
			fileno((*file_iter)->backup_fptr) = NULL;
		}
		fsync(log_fd);
		rvm->transactions.erase(tran_iter);
	}
	
}

/**undo all changes that have happened within the specified transaction.*/
void rvm_abort_trans(trans_t tid){
	/**
	* 1. Find the transaction struct related to the tid. This can be found by looking up the 
	* rvm for the tid. the rvm should contain a list of transaction structs.
	* 2. Find the transaction with the given tid.
	* 3. Find all segments related to this transaction.
	* 4. fsync them all.
	* 5. update the log file to contain a history of all changes contained in its segments.
	* 6. fsync the log file.
	*/
	std::list<transaction_t>::iterator tran_iter;
	std::list<segment_t>::iterator seg_iter;
	
	char ch;
	transaction_t * to_commit = NULL;
	rvm_t rvm = trans_to_rvm[tid];
	for(tran_iter = rvm->transactions.begin();tran_iter!=rvm->transactions.end();tran_iter++){
		if(tid==tran_iter->tid)
			to_commit = &*tran_iter;
	}
	if(to_abort){
		file_t file_list = to_abort->files;
		for(file_iter = file_list.begin();file_iter!=file_list.end();file_iter++){
			(*file_iter)->fptr = (*file_iter)->backup_fptr;
			fileno((*file_iter)->backup_fptr) = NULL;
		}
	}
}
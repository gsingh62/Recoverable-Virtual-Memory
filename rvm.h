#ifndef _RVM_H
#define _RVM_H
#include <string>
#include <list>
#include <map>
#include <set>

typedef struct file{
std::string filename; 
int size;
void * fptr; 
void * backup_fptr
int busy; 
} file_t;

typedef struct trans_range {
int offset;
int size;
} trans_range_t;

typedef struct transaction {
int tid;
std::set<void *> files; 
std::map<void *, std::list<trans_range_t> > trans_range_file_mapping;
} transaction_t;

typedef struct rvm_struct {
std::string directory; 
std::list<file_t> files;
std::list<transaction_t> transactions;
} rvm_t;

rvm_t rvm_init(const char *directory);
void *rvm_map(rvm_t rvm, const char *segname, int size_to_create);
void rvm_unmap(rvm_t rvm, void *segbase);
void rvm_destroy(rvm_t rvm, const char *segname);
trans_t rvm_begin_trans(rvm_t rvm, int numsegs, void **segbases);
void rvm_about_to_modify(trans_t tid, void *segbase, int offset, int size);
void rvm_commit_trans(trans_t tid);
void rvm_abort_trans(trans_t tid);
void rvm_truncate_log(rvm_t rvm);
	

	
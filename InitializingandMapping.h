/**Initialize the library with the specified directory as backing store*/
rvm_t rvm_init(const char *directory);

/**map a segment from disk into memory. If the segment does not already exist, then create it and give it size
size_to_create. If the segment exists but is shorter than size_to_create, then extend it until it is long enough.
It is an error to try to map the same segment twice.*/
void *rvm_map(rvm_t rvm, const char *segname, int size_to_create);

/**unmap a segment from memory.*/
void rvm_unmap(rvm_t rvm, void *segbase);

/**destroy a segment completely, erasing its backing store. This
function should not be called on a segment that is currently
mapped.*/
void rvm_destroy(rvm_t rvm, const char *segname);
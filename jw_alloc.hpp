/*
	Learning STL alloc
*/
#pragma once
#include <cstring>

#if 0
#include <new>
#define __THROW_BAD_ALLOC throw bad_alloc
#elif !defined(__THROW_BAD_ALLOC)
#include <iostream>
#define __THROW_BAD_ALLOC std::cerr << "out of memory" << std::endl; exit(1)
#endif

namespace jw {
	// ----------------- define first allocator, __malloc_alloc_template
	template<int inst>
	class __malloc_alloc_template {
	private:
		// process out of memory, oom
		static void* oom_malloc(size_t);
		static void* oom_realloc(void*, size_t);
		// set malloc handler
		using VOIDPF = void(*)();
		static VOIDPF __malloc_alloc_oom_handler;

	public:
		static void* allocate(size_t n) {
			void* mem = malloc(n);
			if (0 == mem) mem = oom_malloc(n);	// out of memory
			return mem;
		}
		static void dealloc(void* p, size_t) {
			free(p);
		}
		static void* realloc(void* p, size_t, size_t new_sz) {
			void* mem = realloc(p, new_sz);
			if (0 == mem) oom_realloc(p, new_sz);	//out of memory
			return mem;
		}

		// set new handler, return old handler
		static VOIDPF set_malloc_handler(VOIDPF f) {
			VOIDPF old = __malloc_alloc_oom_handler;
			__malloc_alloc_oom_handler = f;
			return old;
		}
	};

	template<int inst>
	void (*__malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;

	template<int inst>
	void* __malloc_alloc_template<inst>::oom_malloc(size_t n) {
		VOIDPF my_malloc_handler;
		void* mem;
		while (true)
		{
			my_malloc_handler = __malloc_alloc_oom_handler;
			if (0 == my_malloc_handler) __THROW_BAD_ALLOC;
			(*my_malloc_handler)(); // process oom function, get new heap memory
			mem = malloc(n);
			if (mem) return mem;
		}
	}

	template<int inst>
	void* __malloc_alloc_template<inst>::oom_realloc(void* p, size_t new_sz) {
		VOIDPF my_malloc_handler;
		void* mem;
		while (true)
		{
			my_malloc_handler = __malloc_alloc_oom_handler;
			if (0 == my_malloc_handler) __THROW_BAD_ALLOC;
			(*my_malloc_handler)();
			mem = realloc(p, new_sz);
			if (mem) return mem;
		}
	}

	// ----------------------- sub allocator, __default_alloc_template
	using malloc_alloc = __malloc_alloc_template<0>;
	enum { __ALIGN = 8 };
	enum { __MAX_BYTES = 128 };
	enum { __NFREELISTS = __MAX_BYTES / __ALIGN };
	template<bool threads, int inst>
	class __default_alloc_template {
	private:
		static size_t ROUND_UP(size_t n) {
			return (n + __ALIGN - 1) & ~(__ALIGN - 1);
		}
		static size_t FREELIST_INDEX(size_t n) {
			return (n + __ALIGN - 1) / __ALIGN - 1;
		}
		static void* refill(size_t n);
		static char* chunk_alloc(size_t n, int& nobjs);

	public:
		static void* allocate(size_t n);
		static void deallocate(void* p, size_t n);
		static void* reallocate(void* p, size_t old_sz, size_t new_sz);
	private:
		union obj
		{
			union obj* free_list_link;
			char data[1];
		};
		static obj* volatile free_list[__NFREELISTS];
		
		// chunk
		static char* start_chunk;
		static char* end_chunk;
		static size_t heap_size;
	};

	// define __default_alloc_template variable
	template<bool threads, int inst>
	char* __default_alloc_template<threads, inst>::start_chunk = 0;
	template<bool threads, int inst>
	char* __default_alloc_template<threads, inst>::end_chunk = 0;
	template<bool threads, int inst>
	size_t __default_alloc_template<threads, inst>::heap_size = 0;
	template<bool threads, int inst>
	typename __default_alloc_template<threads, inst>::obj* volatile
		__default_alloc_template<threads, inst>::free_list[__NFREELISTS] = {0};

	//alloc n bytes
	template<bool threads, int inst>
	void* __default_alloc_template<threads, inst>::allocate(size_t n) {
		// if n > 128 bytes
		if (n > (size_t)__MAX_BYTES) {
			return malloc_alloc::allocate(n);
		}
		// else, choose from free_list
		obj* volatile* my_free_list = free_list + FREELIST_INDEX(n);
		obj* mem = (*my_free_list);
		if (0 == mem) {
			mem = static_cast<obj*>( refill(ROUND_UP(n)));
			return mem;
		}
		(*my_free_list) = mem->free_list_link;
		return mem;
	}

	template<bool threads, int inst>
	void __default_alloc_template<threads, inst>::deallocate(void* p, size_t n) {
		// if n > 128 bytes
		if (n > (size_t)__MAX_BYTES) {
			malloc_alloc::dealloc(p, n);
			return;
		}
		// return to free_list
		obj* q = (obj*)p;
		obj* volatile* my_free_list = free_list + FREELIST_INDEX(n);
		q->free_list_link = (*my_free_list);
		(*my_free_list) = q;
	}

	template<bool threads, int inst>
	void* __default_alloc_template<threads, inst>::reallocate(void* p, size_t old_sz, size_t new_sz) {
		if (old_sz > __MAX_BYTES&& new_sz > __MAX_BYTES) return realloc(p, new_sz);
		if (ROUND_UP(old_sz) == ROUND_UP(new_sz)) return p;
		void* mem = allocate(new_sz);
		size_t copy_sz = new_sz < old_sz ? new_sz : old_sz;
		memcpy(mem, p, copy_sz);
		deallocate(p, old_sz);
		return mem;
	}

	// n must use ROUND_UP, because the free memory must be aligned by 8
	template<bool threads, int inst>
	void* __default_alloc_template<threads, inst>::refill(size_t n) {
		int nobjs = 20;
		char* chunk = chunk_alloc(n, nobjs);
		// get 1
		if (1 == nobjs) return (chunk);
		// else, return head block
		obj* volatile* my_free_list = free_list + FREELIST_INDEX(n);
		obj* mem = (obj*)chunk;
		(*my_free_list) = (obj*)(chunk + n);
		// link the left
		obj* next_blk = (*my_free_list);
		obj* current_blk = next_blk;
		for (int i = 1; i < nobjs - 1; ++i) {
			current_blk = next_blk;
			next_blk = (obj*)(next_blk->data + n);
			current_blk->free_list_link = next_blk;
		}
		current_blk->free_list_link = 0;
		return mem;
	}

	template<bool threads, int inst>
	char* __default_alloc_template<threads, inst>::chunk_alloc(size_t n, int& nobjs) {
		char* chunk = start_chunk;
		size_t total_bytes = n * nobjs;
		size_t chunk_left = end_chunk - start_chunk;
		// chunk is enough
		if (chunk_left >= total_bytes) {
			start_chunk += total_bytes;
			return chunk;
		}
		else if (chunk_left >= n) {	// chunk can supply more then one
			nobjs = int(chunk_left / n);
			start_chunk += n * nobjs;
			return chunk;
		}
		else {	// need malloc a new memory block
			if (chunk_left > 0) {	// handle the rest
				obj* volatile* my_free_list = free_list + FREELIST_INDEX(chunk_left);
				((obj*)start_chunk)->free_list_link = (*my_free_list);
				(*my_free_list) = (obj*)start_chunk;
			}
			// malloc
			size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
			start_chunk = (char*)malloc(bytes_to_get);
			if (0 == start_chunk) {
				// check free_list for free block
				obj* volatile* my_free_list;
				obj* p;
				for (size_t index = n; index <= __MAX_BYTES; index += __ALIGN) {
					my_free_list = free_list + FREELIST_INDEX(index);
					p = (*my_free_list);
					if (0 != p) {	// free_list has space
						(*my_free_list) = p->free_list_link;
						start_chunk = (char*)p;
						end_chunk = start_chunk + index;
						return (chunk_alloc(n, nobjs));
					}
				}
				// can't find space, use __malloc_alloc_template's oom_alloc
				end_chunk = 0;
				start_chunk = static_cast<char*>(malloc_alloc::allocate(bytes_to_get));
			}
			// heap has space
			heap_size += bytes_to_get;
			end_chunk = start_chunk + bytes_to_get;
			return (chunk_alloc(n, nobjs));
		}
	}

#ifdef __USE_ALLOCATOR_THREADS
	const bool __NODE_ALLOCATOR_THREADS = true;
#else
	const bool __NODE_ALLOCATOR_THREADS = false;
#endif // __USE_ALLOCATOR_THREADS


#ifdef __USE_MALLOC
	using alloc = malloc_alloc;
#else
	using alloc = __default_alloc_template<__NODE_ALLOCATOR_THREADS, 0>;
#endif // __USE_MALLOC

	// simple allocate, for interface
	template<typename T, typename Alloc=alloc>
	class simple_alloc {
	public:
		static T* allocate(size_t n) {
			return 0 == n ? 0 : (T*)Alloc::allocate(n * sizeof(T));
		}
		static T* allocate(void) {
			return (T*)Alloc::allocate(sizeof(T));
		}
		static void deallocate(T* p) {
			Alloc::deallocate(p, sizeof(T));
		}
		static void deallocate(T* p, size_t n) {
			if (n != 0) Alloc::deallocate(p, n * sizeof(T));
		}
	};
}
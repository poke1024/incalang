// This file is part of incalang.

// incalang is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
//(at your option) any later version.

// incalang is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with Foobar; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

// ===========================================================================
//	icarus_memory.cp
// ===========================================================================

// large parts of this code were copied from the smartnew.cpp memory allocation
// code from Dr Dobb's Journal 1999/01.

#include "icarus.h"

#if !ICARUS_RT_MEMORY

using namespace icarus;

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <new>

BEGIN_ICARUS_NAMESPACE

//  Policy configuration
const size_t granularity_bits = 2;
const size_t granularity = 1 << granularity_bits;
const size_t page_size_bits = 12;
const size_t page_size = 1 << page_size_bits;
const size_t metapage_size_bits = 8;
const size_t metapage_size = 1 << metapage_size_bits;

size_t handled_obj_size(size_t page_free_size)
{
    return page_free_size;
}
//  Utility methods
template<class T>
inline T align_up(T val, size_t alignment)
{
    return (val + alignment-1) & ~(alignment-1);
}
template<class T>
inline T align_down(T ptr, size_t alignment)
{
    return ptr & ~(alignment-1);
}
struct Header {
    Header* next;
    bool is_empty() const { return next == 0; }
    Header* deque() {
        Header* result = next;
        next = result->next;
        return result;
    }
    void enque(Header* obj) {
        obj->next = next;
        next = obj;
    }
};
struct Page {
    // Linked list handling
    Page* prev;
    Page* next;
    bool is_empty() { return next == this; }
    void link(Page* page) {
        page->next = next;
        page->prev = this;
        next->prev = page;
        this->next = page;
    }
    void unlink() {
        next->prev = prev;
        prev->next = next;
    }
    void check_init() {
        if (prev == 0) prev = next = this;
    }
    // Object allocation handling
    size_t alloc_size;
    size_t alloc_count;
    Header free_list;
    void* unallocated;
    bool is_page_full() const {
        return free_list.is_empty() && unallocated == 0;
    }
    bool is_page_empty() const {
        return alloc_count == 0;
    }
    void* allocate() {
        alloc_count++;
        if (!free_list.is_empty())
            return (void*)free_list.deque();
        void* obj = unallocated;
        unallocated = get_next_obj(unallocated);
        if ((size_t)unallocated > (size_t)get_last_obj())
            unallocated = 0;
        return obj;
    }
    void free(void* obj) {
        alloc_count--;
        free_list.enque((Header*)obj);
    }
    void designate(size_t size) {
        alloc_size = size;
        alloc_count = 0;

        free_list.next = 0;
        unallocated = get_first_obj();
    }
    //  Big allocation
    bool is_big_alloc() {
        return alloc_size == 0;
    }
    void* big_alloc(void* place) {
        alloc_size = 0;
        unallocated = place;
        return get_first_obj();
    }
    void* big_free() {
        return unallocated;
    }
private:
    void* get_first_obj() const {
        return (void*)align_up(sizeof(Page) + (ptrdiff_t)this, granularity);
    }
    void* get_last_obj() const {
        return (void*)align_down(page_size - alloc_size + (ptrdiff_t)this, granularity);
    }
    void* get_next_obj(void* obj) const {
        return (void*)(alloc_size + (size_t)obj);
    }
};
struct PagePool {
    static Header free_list;
public:
    static Page* allocate() {
        if (free_list.is_empty()) allocate_metapage();
        Header* page = free_list.deque();
        return (Page*)page;
    }
    static void free(Page* page) {
        free_list.enque((Header*)page);
    }
private:
    static void allocate_metapage() {
        size_t num_pages = metapage_size;
		void* metapage = std::malloc(num_pages * page_size);
        assert(metapage != 0);
        void* aligned_metapage = (void*)align_up((ptrdiff_t)metapage, page_size);
        if (metapage != aligned_metapage)
            num_pages -= 1;        
        void* curr_page = aligned_metapage;
        for (size_t i=0; i<num_pages; ++i) {
            free_list.enque((Header*)curr_page);
            curr_page = (void*)((size_t)curr_page + page_size);
        }
    }
};
Header PagePool::free_list = {0};

const size_t num_sizes = page_size / granularity;
const size_t page_free_space = page_size - sizeof(Page);

Page page_lists[num_sizes];
inline size_t free_list_index(size_t val, size_t alignment)
{
    return val / alignment;
}

static void* icarus_new(size_t size)
{
    if (size < 0) return 0;
    if (size == 0) size = 1;
    size = align_up(size, granularity);

    if (size >= handled_obj_size(page_free_space)) {
        // This is a "big" allocation
		void* place = std::malloc(size+page_size+sizeof(Page));
        //assert(place != 0);
        if( place == 0 )
        	throw_icarus_error( "not enough memory" );
        Page* page = (Page*)align_up((ptrdiff_t)place, page_size);
        return page->big_alloc(place);
    }
    Page& header = page_lists[ free_list_index(size, granularity) ];
    header.check_init();
    if (header.is_empty()) {
        Page* free_page = PagePool::allocate();
        header.link(free_page);
        free_page->designate(size);
    }
    Page* free_page = header.next;
    void* obj = free_page->allocate();
    if (free_page->is_page_full()) {
        free_page->unlink();
    }
        
    return obj;
}

static void icarus_delete(void* ptr)
{
    if (ptr == 0) return;
    Page* page = (Page*)align_down((ptrdiff_t)ptr, page_size);
    if (page->is_big_alloc()) {
        void* place = page->big_free();
		std::free(place);
        return;
    }
    if (page->is_page_full()) {
        page->free(ptr);
        if (page->is_page_empty())
            PagePool::free(page);
        else {
            Page& header = page_lists[ free_list_index(page->alloc_size,granularity) ];
            header.link(page);
        }
        return;
    }
    else {
        page->free(ptr);
        if (page->is_page_empty()) {
            page->unlink();
            PagePool::free(page);
        }
    }
}

// ===========================================================================

void*
imalloc(
	size_t				size )
{
	if( size < 0 )
		throw_icarus_error( "illegal size in new" );
		
    if( s_mem_track )
    	s_mem_alloc += size;

	u08* p = (u08*)icarus_new( size + sizeof( size_t ) );
	
	if( !p )
		throw_icarus_error( "not enough memory" );
	
	std::memset( p, 0xfe, size + sizeof( size_t ) );

	*(size_t*)p = size;
	
	return p + sizeof( size_t );
}

void
ifree(
	void*				ptr )
{
	if( ptr == 0 )
		return;

	u08* p = (u08*)ptr;
	p -= sizeof( size_t );
	size_t size = *(size_t*)p;
		
    if( s_mem_track )
    	s_mem_alloc -= size;

	std::memset( p, 0xfe, size + sizeof( size_t ) );
	icarus_delete( p );
}

void*
icalloc(
	size_t				size,
	size_t				count )
{
	size_t				bytes = size * count;
	void*				p = imalloc( bytes );
	memset( p, 0, bytes );
	return p;
}

void*
irealloc(
	void*				p,
	size_t				qsize )
{
	if( !p )
		return imalloc( qsize );

	u08*				psp = (u08*)p;
	psp -= sizeof( size_t );

	size_t				psize = *(size_t*)psp;
	void*				q = imalloc( qsize );
	
	memcpy( q, p, ( psize < qsize ) ? psize : qsize );
	ifree( p );

	return q;
}

char*
istrdup(
	const char*		s )
{
	char*			t;

	if( !s )
		return 0;

	t = (char*)imalloc(
		sizeof( char ) * ( strlen( s ) + 1 ) );
	strcpy( t, s );

	return t;
}

size_t
imemfree()
{
	return (size_t)-1;
}

END_ICARUS_NAMESPACE

#endif

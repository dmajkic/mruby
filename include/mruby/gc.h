/*
** mruby/gc.h - garbage collector for mruby
**
** See Copyright Notice in mruby.h
*/

#ifndef MRUBY_GC_H
#define MRUBY_GC_H

#include "mruby/common.h"

/**
 * Uncommon memory management stuffs.
 */
MRB_BEGIN_DECL


struct mrb_state;

typedef void (mrb_each_object_callback)(struct mrb_state *mrb, struct RBasic *obj, void *data);
void mrb_objspace_each_objects(struct mrb_state *mrb, mrb_each_object_callback *callback, void *data);
MRB_API void mrb_free_context(struct mrb_state *mrb, struct mrb_context *c);


/* white: 011, black: 100, gray: 000 */
#define MRB_GC_GRAY 0
#define MRB_GC_WHITE_A 1
#define MRB_GC_WHITE_B (1 << 1)
#define MRB_GC_BLACK (1 << 2)
#define MRB_GC_WHITES (MRB_GC_WHITE_A | MRB_GC_WHITE_B)
#define MRB_GC_COLOR_MASK 7

#ifndef MRB_GC_ARENA_SIZE
#define MRB_GC_ARENA_SIZE 100
#endif

typedef enum {
  GC_STATE_ROOT = 0,
  GC_STATE_MARK,
  GC_STATE_SWEEP
} mrb_gc_state;

typedef struct mrb_heap_page {
  struct RBasic *freelist;
  struct mrb_heap_page *prev;
  struct mrb_heap_page *next;
  struct mrb_heap_page *free_next;
  struct mrb_heap_page *free_prev;
  mrb_bool old:1;
  void *objects[];
} mrb_heap_page;

typedef struct mrb_gc {
  mrb_heap_page *heaps;                /* heaps for GC */
  mrb_heap_page *sweeps;
  mrb_heap_page *free_heaps;
  size_t live; /* count of live objects */
#ifdef MRB_GC_FIXED_ARENA
  struct RBasic *arena[MRB_GC_ARENA_SIZE]; /* GC protection array */
#else
  struct RBasic **arena;                   /* GC protection array */
  int arena_capa;
#endif
  int arena_idx;

  mrb_gc_state gc_state; /* state of gc */
  int current_white_part; /* make white object by white_part */
  struct RBasic *gray_list; /* list of gray objects to be traversed incrementally */
  struct RBasic *atomic_gray_list; /* list of objects to be traversed atomically */
  size_t gc_live_after_mark;
  size_t gc_threshold;
  int gc_interval_ratio;
  int gc_step_ratio;
  mrb_bool disabled      :1;
  mrb_bool full          :1;
  mrb_bool generational  :1;
  mrb_bool out_of_memory :1;
  size_t majorgc_old_threshold;
} mrb_gc;

MRB_API mrb_bool
mrb_object_dead_p(struct mrb_state *mrb, struct RBasic *object);

MRB_END_DECL

#endif  /* MRUBY_GC_H */

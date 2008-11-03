#include <cko_types.h>
#include <assert.h>

int cko_types_test() {
  assert(sizeof(cko_u8)==1);
  assert(sizeof(cko_s8)==1);
  assert(sizeof(cko_u16)==2);
  assert(sizeof(cko_s16)==2);
  assert(sizeof(cko_u32)==4);
  assert(sizeof(cko_s32)==4);
  assert(sizeof(cko_u64)==8);
  assert(sizeof(cko_s64)==8);
}


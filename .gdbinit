set confirm off
set verbose off

# set $64BITS = 1
# set $SHOW_CONTEXT = 1
# set $SHOW_NEST_INSN = 0
# set $CONTEXTSIZE_STACK = 6
# set $CONTEXTSIZE_DATA  = 8
# set $CONTEXTSIZE_CODE  = 8

set history filename ~/.gdb_history
set history save on
set history size 10000

# target remote localhost:3333

# load symbol table
# symbol-file .pio/build/hifive1-revb/firmware.elf

# directory src/ include/

b main
#if                                                   \
!(REGSIZE ==512) && !(REGSIZE ==256) &&               \
!(REGSIZE ==128) && !(REGSIZE ==64 ) &&               \
!(REGSIZE ==32 ) && !(REGSIZE ==16 ) &&               \
!(REGSIZE ==8  ) && !(REGSIZE ==4  ) &&               \
!(REGSIZE ==2  ) && !(REGSIZE ==1  )
#error "REGSIZE must be a power of 2, and no longer than 512 (run ./configure --with-regsize=...)"
#endif

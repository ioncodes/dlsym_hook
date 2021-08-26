clang -S -emit-llvm utils.cpp

/home/layle/orchestra/root/bin/llvm-link \
  -S \
  $1 \
  utils.ll \
  ../../../../../../home/layle/orchestra/root/share/revng/support-x86_64-normal.ll \
  -o \
  $1.linked.ll

/home/layle/orchestra/root/bin/llc \
  -O0 \
  $1.linked.ll \
  -o \
  $1.linked.ll.o \
  -disable-machine-licm \
  -filetype=obj

/home/layle/orchestra/root/link-only/bin/c++ \
  ./$1.linked.ll.o \
  -lz \
  -lm \
  -lrt \
  -lpthread \
  -L \
  ./ \
  -o \
  ./dummy.translated \
  -fno-pie \
  -no-pie \
  -Wl,-z,max-page-size=4096 \
  -Wl,--section-start=.o_r_0x400000=0x400000 \
  -Wl,--section-start=.o_rx_0x401000=0x401000 \
  -Wl,--section-start=.o_r_0x402000=0x402000 \
  -Wl,--section-start=.o_rw_0x403d68=0x403d68 \
  -fuse-ld=bfd \
  -Wl,--section-start=.elfheaderhelper=0x3fffff \
  -Wl,-Ttext-segment=0x405000 \
  -Wl,--no-as-needed \
  -ldl \
  -lstdc++ \
  -lc \
  -Wl,--as-needed

cp ./dummy.translated ./dummy.translated.tmp

/home/layle/orchestra/root/bin/revng \
  merge-dynamic \
  ./dummy.translated.tmp \
  ./dummy \
  ./dummy.translated

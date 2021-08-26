# dlsym_hook
Example project created for my blog post ["Instrumenting binaries using revng and LLVM"](https://layle.me/posts/instrumentation-with-revng) using [revng](https://rev.ng/) and [LLVM](https://llvm.org/).  

## What does it do?
It dumps the strings passed to `dlsym` by lifting and instrumenting the binary. We then finally recompile it back to a working executable bypassing anti debug checks. 
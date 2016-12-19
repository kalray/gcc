# Check that the builtin signatures in builtin_k1.h and k1.c are aligned.

check:
	@rm -f OUT1 OUT2
	perl builtin_k1_extract.pl builtin_k1.h > OUT1
	perl k1_extract.pl k1.c > OUT2
	@vimdiff OUT1 OUT2


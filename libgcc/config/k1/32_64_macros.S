#pragma once

#ifdef __K1_64__
	
#define LOAD_PTR ld
#define STR_PTR sd
#define ADD_PTR addd

#else
	
#define LOAD_PTR lwz
#define STR_PTR sw
#define ADD_PTR addw
	
#endif

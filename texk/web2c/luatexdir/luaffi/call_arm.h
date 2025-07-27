/*
** This file has been pre-processed with DynASM.
** http://luajit.org/dynasm.html
** DynASM version 1.4.0, DynASM arm version 1.4.0
** DO NOT EDIT! The original file is in "call_arm.dasc".
*/

/* vim: ts=4 sw=4 sts=4 et tw=78
 * Portions copyright (c) 2015-present, Facebook, Inc. All rights reserved.
 * Portions copyright (c) 2011 James R. McKaskill.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */
 
//The generate code is for arm not for thumb
#if DASM_VERSION != 10400
#error "Version mismatch between DynASM and included encoding engine"
#endif

static const unsigned int build_actionlist[743] = {
0xe28dc000,
0x000b0000,
0xe89c100e,
0x00000000,
0xe28dc000,
0x000b0000,
0xe89c100e,
0x00000000,
0xe28dc000,
0x000b0000,
0xe89c0006,
0x00000000,
0xe51d1000,
0x000e8180,
0x00000000,
0xe2866000,
0x000b0000,
0x00000000,
0xe8b6100e,
0x00000000,
0xe8b60006,
0x00000000,
0xe4961004,
0x00000000,
0xe1a0c00d,
0xe92d000f,
0x00000000,
0xe92d5050,
0xe1a0600c,
0xe3004000,
0x000c0200,
0xe3404000,
0x000c0200,
0xe3002000,
0x000c0200,
0xe3402000,
0x000c0200,
0xe3001000,
0x000c0200,
0xe3401000,
0x000c0200,
0xe1a00004,
0xeb000000,
0x00030000,
0x00000000,
0xe3a02000,
0x000b0000,
0xe3e01000,
0xe1a00004,
0xeb000000,
0x00030000,
0x00000000,
0xe3a02000,
0x000b0000,
0xe3e01000,
0x000b0000,
0xe1a00004,
0xeb000000,
0x00030000,
0xe3002000,
0x000c0200,
0xe3402000,
0x000c0200,
0xe3e01000,
0xe1a00004,
0xeb000000,
0x00030001,
0x00000000,
0xe5801000,
0xe3e01001,
0xe1a00004,
0xeb000000,
0x00030002,
0x00000000,
0xe3a02000,
0x000b0000,
0xe3e01000,
0x000b0000,
0xe1a00004,
0xeb000000,
0x00030000,
0xe3002000,
0x000c0200,
0xe3402000,
0x000c0200,
0xe3e01000,
0xe1a00004,
0xeb000000,
0x00030001,
0x00000000,
0xe5801000,
0x00000000,
0xe28d1000,
0x000b0000,
0x00000000,
0xe1a01006,
0x00000000,
0xe3002000,
0x000c0200,
0xe3402000,
0x000c0200,
0xe300c000,
0x000c0200,
0xe340c000,
0x000c0200,
0xe12fff3c,
0x00000000,
0xe3e01001,
0xe1a00004,
0xeb000000,
0x00030002,
0x00000000,
0xe3a02000,
0x000b0000,
0xe3e01000,
0x000b0000,
0xe1a00004,
0xeb000000,
0x00030000,
0xe3002000,
0x000c0200,
0xe3402000,
0x000c0200,
0xe3e01000,
0xe1a00004,
0xeb000000,
0x00030001,
0x00000000,
0xe880100e,
0xe3e01001,
0xe1a00004,
0xeb000000,
0x00030002,
0x00000000,
0xe3a02000,
0x000b0000,
0xe3e01000,
0x000b0000,
0xe1a00004,
0xeb000000,
0x00030000,
0xe3002000,
0x000c0200,
0xe3402000,
0x000c0200,
0xe3e01000,
0xe1a00004,
0xeb000000,
0x00030001,
0x00000000,
0xe8800006,
0xe3e01001,
0xe1a00004,
0xeb000000,
0x00030002,
0x00000000,
0xe28dc000,
0x000b0000,
0xe89c000c,
0x00000000,
0xe2866004,
0x00000000,
0xe8b6000c,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030003,
0x00000000,
0xe3002000,
0x000c0200,
0xe3402000,
0x000c0200,
0xe3a01000,
0xe1a00004,
0xeb000000,
0x00030001,
0x00000000,
0xe8800006,
0xe3e01001,
0xe1a00004,
0xeb000000,
0x00030002,
0x00000000,
0xe3002000,
0x000c0200,
0xe3402000,
0x000c0200,
0xe3a01000,
0xe1a00004,
0xeb000000,
0x00030001,
0x00000000,
0xe5801000,
0xe3e01001,
0xe1a00004,
0xeb000000,
0x00030002,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030004,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030005,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030006,
0x00000000,
0xe28dc000,
0x000b0000,
0xe89c000c,
0x00000000,
0xe2866004,
0x00000000,
0xe8b6000c,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030007,
0x00000000,
0xe3a02000,
0x000b0000,
0xe3a01000,
0x000b0000,
0xe1a00004,
0xeb000000,
0x00030008,
0x00000000,
0xe3a02000,
0x000b0000,
0xe3e01001,
0xe1a00004,
0xeb000000,
0x00030000,
0xe3003000,
0x000c0200,
0xe3403000,
0x000c0200,
0xe3e02000,
0xe3e01001,
0xe1a00004,
0xeb000000,
0x00030009,
0x00000000,
0xe3a02000,
0x000b0000,
0xe3e01001,
0xe1a00004,
0xeb000000,
0x00030000,
0xe3003000,
0x000c0200,
0xe3403000,
0x000c0200,
0xe3e02000,
0xe3e01001,
0xe1a00004,
0xeb000000,
0x0003000a,
0xe1a06000,
0xe3e01002,
0xe1a00004,
0xeb000000,
0x0003000b,
0x00000000,
0xe5960000,
0x00000000,
0xe51d0000,
0x000e8180,
0xe1a01006,
0xe3002000,
0x000c0200,
0xe3402000,
0x000c0200,
0xe300c000,
0x000c0200,
0xe340c000,
0x000c0200,
0xe12fff3c,
0x00000000,
0xe3a02000,
0x000b0000,
0xe3e01001,
0xe1a00004,
0xeb000000,
0x00030000,
0xe3003000,
0x000c0200,
0xe3403000,
0x000c0200,
0xe3e02000,
0xe3e01001,
0xe1a00004,
0xeb000000,
0x0003000c,
0x00000000,
0xe3e01001,
0xe1a00004,
0xeb000000,
0x0003000b,
0x00000000,
0xe3e01000,
0x00000000,
0xe1a00004,
0xeb000000,
0x0003000d,
0x00000000,
0xe1a00004,
0xeb000000,
0x0003000e,
0x00000000,
0xe3500000,
0x13a00001,
0x00000000,
0xe6ef0070,
0x00000000,
0xe6af0070,
0x00000000,
0xe6ff0070,
0x00000000,
0xe6bf0070,
0x00000000,
0xe3e01000,
0x00000000,
0xe1a00004,
0xeb000000,
0x0003000f,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030010,
0x00000000,
0xe3e01000,
0xe1a00004,
0xeb000000,
0x00030011,
0x00000000,
0xe3e01000,
0xe1a00004,
0xeb000000,
0x00030012,
0x00000000,
0xe3e01000,
0xe1a00004,
0xeb000000,
0x00030013,
0x00000000,
0xe3e02000,
0xe1a01004,
0xe51d0000,
0x000e8180,
0xeb000000,
0x00030014,
0xe3e01002,
0xe1a00004,
0xeb000000,
0x0003000b,
0x00000000,
0xe3e02000,
0xe1a01004,
0xe51d0000,
0x000e8180,
0xeb000000,
0x00030015,
0xe3e01002,
0xe1a00004,
0xeb000000,
0x0003000b,
0x00000000,
0xe1a06000,
0xe3e01002,
0xe1a00004,
0xeb000000,
0x0003000b,
0xe1a00006,
0x00000000,
0xe1a06000,
0xe1a00004,
0xe1a04001,
0xe3e01002,
0xeb000000,
0x0003000b,
0xe1a00006,
0xe1a01004,
0x00000000,
0xe89da050,
0x00000000,
0xe2866004,
0x00000000,
0xe8a60003,
0x00000000,
0xe28dc000,
0x000b0000,
0xe88c0003,
0x00000000,
0xe4860004,
0x00000000,
0xe50d0000,
0x000e8180,
0x00000000,
0xe2866004,
0x00000000,
0xeca60a01,
0x00000000,
0xeca60b02,
0x00000000,
0xeca60a04,
0x00000000,
0xeca60b04,
0x00000000,
0xeca60b06,
0x00000000,
0xeca60b08,
0x00000000,
0xe92d40f0,
0xe28d700c,
0x00000000,
0xe92d4870,
0xe28db00c,
0x00000000,
0xe24dd004,
0x00000000,
0xe1a04000,
0x00000000,
0xe300c000,
0x000c0200,
0xe340c000,
0x000c0200,
0xe04dd00c,
0x00000000,
0xe24dd000,
0x000b0000,
0x00000000,
0xeb000000,
0x00030016,
0xe3500000,
0x000b0000,
0xaa000000,
0x00050001,
0xe3001000,
0x000c0200,
0xe3401000,
0x000c0200,
0xe1a00004,
0xeb000000,
0x00030017,
0x0006000b,
0xe1a05000,
0xe04dd185,
0x00000000,
0xe28d6000,
0x000b0000,
0x00000000,
0xe3002000,
0x000c0200,
0xe3402000,
0x000c0200,
0xe3a01000,
0xe1a00004,
0xeb000000,
0x00030001,
0xe50d0000,
0x000e8180,
0x00000000,
0xe3003000,
0x000c0200,
0xe3403000,
0x000c0200,
0xe3002000,
0x000c0200,
0xe3402000,
0x000c0200,
0xe3a01000,
0x000b0000,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030009,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030018,
0x00000000,
0xe1a00004,
0xeb000000,
0x0003000c,
0x00000000,
0xe1a00004,
0xeb000000,
0x0003000a,
0x00000000,
0xe2866004,
0x00000000,
0xe1a01000,
0xe3002000,
0x000c0200,
0xe3402000,
0x000c0200,
0xe300c000,
0x000c0200,
0xe340c000,
0x000c0200,
0x00000000,
0xe28d0000,
0x000b0000,
0x00000000,
0xe1a00006,
0x00000000,
0xe12fff3c,
0x00000000,
0xe2866000,
0x000b0000,
0x00000000,
0xe3a01000,
0x000b0000,
0x00000000,
0xe1a00004,
0xeb000000,
0x0003000e,
0xe3500000,
0x13a00001,
0x00000000,
0xe1a00004,
0xeb000000,
0x0003000e,
0x00000000,
0xe6ef0070,
0x00000000,
0xe6af0070,
0x00000000,
0xe1a00004,
0xeb000000,
0x0003000e,
0x00000000,
0xe6ff0070,
0x00000000,
0xe6bf0070,
0x00000000,
0xe1a00004,
0xeb000000,
0x0003000d,
0x00000000,
0xe1a00004,
0xeb000000,
0x0003000e,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030011,
0x00000000,
0xe1a00004,
0xeb000000,
0x0003000f,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030010,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030013,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030012,
0x00000000,
0xe1a02001,
0xe1a01004,
0x00000000,
0xe28d0000,
0x000b0000,
0x00000000,
0xe2866004,
0x00000000,
0xe1a00006,
0x00000000,
0xeb000000,
0x00030014,
0x00000000,
0xe1a02001,
0xe1a01004,
0x00000000,
0xe28d0000,
0x000b0000,
0x00000000,
0xe1a00006,
0x00000000,
0xeb000000,
0x00030015,
0x00000000,
0xe2866000,
0x000b0000,
0x00000000,
0xe3a01000,
0x000b0000,
0x00000000,
0xe1a03006,
0x00000000,
0xe28d3000,
0x000b0000,
0x00000000,
0xe1a02005,
0xe1a00004,
0xeb000000,
0x00030019,
0x00000000,
0xe8bd000f,
0x00000000,
0xe8bd0003,
0x00000000,
0xe300c000,
0x000c0200,
0xe340c000,
0x000c0200,
0xe12fff3c,
0x00000000,
0xe1a06000,
0xe3002000,
0x000c0200,
0xe3402000,
0x000c0200,
0xe3001000,
0x000c0200,
0xe3401000,
0x000c0200,
0xe1a00004,
0xeb000000,
0x00030001,
0xe5806000,
0xe3a00001,
0x00000000,
0xe1a03001,
0xe1a02000,
0x00000000,
0xe1a02001,
0xe1a01000,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030003,
0xe3a00001,
0x00000000,
0xe1a06000,
0xe1a05001,
0xe3002000,
0x000c0200,
0xe3402000,
0x000c0200,
0xe3a01000,
0xe1a00004,
0xeb000000,
0x00030001,
0xe5806000,
0xe5805004,
0xe3a00001,
0x00000000,
0xe1a06000,
0xe3002000,
0x000c0200,
0xe3402000,
0x000c0200,
0xe3a01000,
0xe1a00004,
0xeb000000,
0x00030001,
0xe5806000,
0xe3a00001,
0x00000000,
0xe3a00000,
0x00000000,
0xe1a01000,
0xe1a00004,
0xeb000000,
0x00030004,
0xe3a00001,
0x00000000,
0xe1a01000,
0x00000000,
0xe1a00004,
0xeb000000,
0x0003001a,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030005,
0x00000000,
0xe3a00001,
0x00000000,
0xe1a01000,
0xe1a00004,
0xeb000000,
0x00030006,
0xe3a00001,
0x00000000,
0xe1a03001,
0xe1a02000,
0x00000000,
0xe1a02001,
0xe1a01000,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030007,
0xe3a00001,
0x00000000,
0xe3001000,
0x000c0200,
0xe3401000,
0x000c0200,
0xe1a00004,
0xeb000000,
0x0003001b,
0xe3e01001,
0xe1a00004,
0xeb000000,
0x0003001c,
0x00000000,
0xe3a00000,
0x00000000,
0xe1a06000,
0xe3002000,
0x000c0200,
0xe3402000,
0x000c0200,
0xe3001000,
0x000c0200,
0xe3401000,
0x000c0200,
0xe1a00004,
0xeb000000,
0x00030001,
0xe5806000,
0x00000000,
0xe3a00001,
0x00000000,
0xe247d00c,
0xe8bd80f0,
0x00000000,
0xe24bd00c,
0xe8bd8870,
0x00000000
};

static const char *const globnames[] = {
  (const char *)0
};
static const char *const extnames[] = {
  "rawgeti",
  "push_cdata",
  "lua_remove",
  "lua_pushinteger",
  "lua_pushboolean",
  "push_int",
  "push_float",
  "lua_pushnumber",
  "lua_call",
  "check_typed_pointer",
  "check_struct",
  "lua_settop",
  "check_enum",
  "check_uint32",
  "check_int32",
  "check_uint64",
  "check_int64",
  "check_uintptr",
  "check_float",
  "check_double",
  "check_complex_double",
  "check_complex_float",
  "lua_gettop",
  "luaL_error",
  "check_typed_cfunction",
  "unpack_varargs_stack",
  "push_uint",
  "lua_pushvalue",
  "lua_setuservalue",
  (const char *)0
};

#define JUMP_SIZE 8

#define MIN_BRANCH ((INT32_MIN) >> 8)
#define MAX_BRANCH ((INT32_MAX) >> 8)
//arm pc offset 8 so comparing with next instruction is 4,
//unlike x86 which pass in the current instruction address+1 rather than the next instruction 
#define BRANCH_OFF 4	


#define ROUND_UP(x, align) (((int) (x) + (align - 1)) & ~(align - 1))
#ifdef TARGET_OS_IPHONE
#define  CK_ALGIN  0
#else
#define  CK_ALGIN  1
#endif
#define ALIGNED(x, align) (!CK_ALGIN||((int)(x) & (align - 1)) == 0)
#if defined(__ARM_PCS_VFP) || (GCC_VERSION==40500||defined(__clang__))&&!defined(__ARM_PCS) && !defined(__SOFTFP__) && !defined(__SOFTFP) && \
    defined(__VFP_FP__)
#define ARM_HF 1
#else
#define ARM_HF 0
#endif
#if ARM_HF&&!CK_ALGIN
#error "Unsupported unaligned stack for hard floating point"
#endif	

static void compile_extern_jump(struct jit* jit, lua_State* L, cfunction func, uint8_t* code)
{
    /* The jump code is the function pointer followed by a stub to call the
     * function pointer. The stub exists so we can jump to functions with an
     * offset greater than 32MB.
     *
     * Note we have to manually set this up since there are commands buffered
     * in the jit state.
     */
	
    *(cfunction*) code = func;
     //ldr pc, [pc - 12]
    *(uint32_t*) &code[4] = 0xE51FF00CU; 
	
}




void compile_globals(struct jit* jit, lua_State* L)
{
    (void) jit;
}

typedef struct stack_info{
	int extra;
	int int_off;
	int stack_off;
	int float_size;
#if ARM_HF
	int float_off; 
#endif	
} stack_info;
//vfp use back-filling rule for registers until a float value on stack
typedef struct reg_info{
	uint16_t exs;	
	union{
		uint8_t ints;
		uint8_t regs;
	};
#if ARM_HF
	uint8_t float_sealed;
	short floats;//each bit is a float: s0-s15 or v0-v7 or q0-q3
	uint8_t left_single;
	uint8_t highest_bit;
#endif
} reg_info;

#define MAX_REGS 4
#define MAX_FLOAT_REGS 16
#ifndef bool
#define bool uint8_t
#endif

#define has_bit(x,b) (((x)&(1<<(b)))!=0)
#define set_bit(x,b) (x=((x)|(1<<(b)))) 
#define FIX_ALIGN(x,al) \
	if(!ALIGNED((x),al)){\
		x=ROUND_UP(x,al);\
	}
static ALWAYS_INLINE bool is_float_sealed(reg_info* regs){
#if ARM_HF
return regs->float_sealed;
#else
return regs->regs>=MAX_REGS;
#endif
}
//return size need to put on stack
static ALWAYS_INLINE int add_int_reg(reg_info* regs){
	if(regs->regs<MAX_REGS){
		regs->regs++;
		return 0;
	}
	return 1;
		
}
//return size need to put on stack
static ALWAYS_INLINE int add_int64_reg(reg_info* regs){
	if(regs->regs<MAX_REGS-1){
		regs->regs=ROUND_UP(regs->regs,2)+2;
		return 0;
	}else if(regs->regs==MAX_REGS-1){
		regs->regs=MAX_REGS;
	}
		
	return 2;
}
static ALWAYS_INLINE bool is_float_type(int t){
    return t==FLOAT_TYPE||t==DOUBLE_TYPE;
}
static int hfa_size(lua_State* L,int idx, const struct ctype* ct,int* isfloat){
	struct ctype* mt;
	int type,ele_count,i,ct_usr;
	lua_getuservalue(L,idx);
	ct_usr=lua_absindex(L,-1);
    lua_rawgeti(L,ct_usr,1);
    mt=(struct ctype*)lua_touserdata(L,-1);
    if(mt==NULL||(mt->pointers&&!mt->is_array)||mt->is_reference||!is_float_type(mt->type)){
        lua_pop(L,2);
		if(ct->type==COMPLEX_DOUBLE_TYPE){
			if(isfloat) *isfloat=0;
			return 4;
		}else if(ct->type==COMPLEX_FLOAT_TYPE){
			if(isfloat) *isfloat=1;
			return 2;
		}
        return 0;
    }
	type=mt->type;
    ele_count=(int)(ct->base_size/mt->base_size);
    if(ele_count>4||ct->base_size%mt->base_size){
        lua_pop(L,2);
        return 0;
    }
	lua_pop(L,1);
    for (i = 2; i <=4 ; ++i) {
        lua_rawgeti(L,ct_usr,i);
		if(lua_isnil(L,-1)){//case have array member;
            lua_pop(L,1);
            break;
        }
        mt=(struct ctype*)lua_touserdata(L,-1);
        if(mt->type!=type||(mt->pointers&&!mt->is_array)||mt->is_reference||!is_float_type(mt->type)){
            lua_pop(L,2);
            return 0;
        }
        lua_pop(L,1);
    }
	if(isfloat){
		*isfloat=mt->type==FLOAT_TYPE;
	}
	lua_pop(L,1);
    return ele_count*(mt->type==FLOAT_TYPE?1:2);
}
#if ARM_HF
static void save_float_reg(struct jit* Dst,int reg,int size,stack_info* st){
	int sz;
	if(reg==-1) return;
	for(;size>0;size-=8){
		sz=size>8?8:size;
		switch(sz){
		case 4:
			switch(reg){
				case 0:
					break;
				case 1:
					break;
				case 2:
					break;
				case 3:
					break;
				case 4:
					break;
				case 5:
					break;
				case 6:
					break;
				case 7:
					break;
				case 8:
					break;
				case 9:
					break;
				case 10:
					break;
				case 11:
					break;
				case 12:
					break;
				case 13:
					break;
				case 14:
					break;
				case 15:
					break;
			}
			break;
		case 8:
			switch(reg>>1){
				case 0:
					break;
				case 1:
					break;
				case 2:
					break;
				case 3:
					break;
				case 4:
					break;
				case 5:
					break;
				case 6:
					break;
				case 7:
					break;
			}
			reg+=2;
			break;
		}
		st->float_size+=sz;
	}
}

//128 bit vector type is not supported by this
static int add_float_reg(reg_info* regs,int sz,int isfloat){

	if(is_float_sealed(regs)) return -1;
	int i,ret=-1;
	if(sz==1){
		if(regs->left_single){
			int n=regs->highest_bit;
			for(i=0;i<n;++i){
				if(!has_bit(regs->floats,i)){
					regs->left_single--;
					set_bit(regs->floats,i);
					ret=i;
				}
			}
		}else{
			ret=regs->highest_bit;
			set_bit(regs->floats,regs->highest_bit);
			++regs->highest_bit;
		}
	}else{
		if(regs->highest_bit>MAX_FLOAT_REGS-sz){
			regs->highest_bit=MAX_FLOAT_REGS;
		}else{
			if(!isfloat&&!ALIGNED(regs->highest_bit, 2)){
				regs->highest_bit++;
				regs->left_single++;
			}
			ret=regs->highest_bit;
			for(i=0;i<sz;++i){
				set_bit(regs->floats,regs->highest_bit++);
			}
		}
	}
	if(regs->highest_bit==MAX_FLOAT_REGS){
		regs->float_sealed=true;		
	}
	return ret;
}
#endif
static void load_reg(struct jit* Dst,int off,int size){
	if(size==16){
		dasm_put(Dst, 0, off);
	}else if(size==12){
		dasm_put(Dst, 4, off);
	}else if(size==8){
		dasm_put(Dst, 8, off);
	}else{
		dasm_put(Dst, 12, off);
	}
}
// arm store/load range for immediate value is only -256-255
static void load_stack(struct jit* Dst,stack_info* st,int size,int align){
	int off=st->stack_off;
	FIX_ALIGN(st->stack_off,align);
	if((off=st->stack_off-off)){
		dasm_put(Dst, 15, off);
	}
	if(size==16){
		dasm_put(Dst, 18);
	}else if(size==8){
		dasm_put(Dst, 20);
	}else{
		dasm_put(Dst, 22);
	}
	st->stack_off+=size;
}

static void load_int(struct jit* Dst,stack_info* st,int size,int align){
	FIX_ALIGN(st->int_off,align);
	if(st->int_off<0x40*ARM_HF+MAX_REGS*4&&(!st->stack_off||MAX_REGS*4+size<=0x40*ARM_HF+MAX_REGS*4)){
		load_reg(Dst,st->int_off+st->extra,size);
		st->int_off+=size;
	}else{
		st->int_off=0x40*ARM_HF+MAX_REGS*4;
		load_stack(Dst,st,size,align);
	}
	
}

static void load_float(struct jit* Dst,stack_info* st,int size,int vfp,int align){
	#if ARM_HF
	if(st->float_off<st->float_size){
		if(vfp){
			if(size==4){//float
			}else if(size==8){//double
			}
		}else load_reg(Dst,st->float_off+st->extra,size);
		st->float_off+=size;
	}else if(vfp){
		if(size==4){//float
		}else if(size==8){//double
			int off=st->stack_off;
			FIX_ALIGN(st->stack_off,align);
			if((off=st->stack_off-off)){
			}
		}
	}else{
		load_stack(Dst,st,size,align);
	}
	#else	
		load_int(Dst,st,size,align);
	#endif
}
#if ARM_HF
static void push_regs(lua_State* L,struct jit* Dst,int ct_usr,int nargs,stack_info* st){
	const struct ctype* mt;
	reg_info regs;int i;
	memset(&regs,0,sizeof(reg_info));
    for (i = 1; i <= nargs&&!is_float_sealed(&regs); ++i){
		lua_rawgeti(L, ct_usr, i);
		mt = (const struct ctype*) lua_touserdata(L, -1);
		if (!mt->pointers &&! mt->is_reference) {
			switch(mt->type){
				case COMPLEX_DOUBLE_TYPE:
					save_float_reg(Dst,add_float_reg(&regs,4,0),16,st);
					break;
				case DOUBLE_TYPE:
					save_float_reg(Dst,add_float_reg(&regs,2,0),8,st);
					break;
				case COMPLEX_FLOAT_TYPE:
					save_float_reg(Dst,add_float_reg(&regs,2,1),8,st);
					break;
				case FLOAT_TYPE:
					save_float_reg(Dst,add_float_reg(&regs,1,1),4,st);
					break;
				case STRUCT_TYPE:
					{
						int isfloat,hfasize=hfa_size(L,-1,mt,&isfloat);
						if(hfasize){
							save_float_reg(Dst,add_float_reg(&regs,hfasize,isfloat),4*hfasize,st);
							break;
						}
					}
				case UNION_TYPE:
					break;
				case INT64_TYPE:
					//add_int64_reg(&regs);
					break;
				default:
					//add_int_reg(&regs);//no need to check type support here
					break;
			}
		}
		lua_pop(L,1);
	}
	st->float_off+=st->int_off;
	st->int_off+=0x40;
}
#endif
cfunction compile_callback(lua_State* L, int fidx, int ct_usr, const struct ctype* ct)
{
    struct jit* Dst = get_jit(L);;
    int i, nargs, num_upvals, ref;
    const struct ctype* mt;
	stack_info st;

    int top = lua_gettop(L);

    ct_usr = lua_absindex(L, ct_usr);
    fidx = lua_absindex(L, fidx);
    nargs = (int) lua_rawlen(L, ct_usr);

    dasm_setup(Dst, build_actionlist);

    lua_newtable(L);
    lua_pushvalue(L, -1);
    ref = luaL_ref(L, LUA_REGISTRYINDEX);
    num_upvals = 0;

    if (ct->has_var_arg) {
        luaL_error(L, "can't create callbacks with varargs");
    }
	memset(&st,0,sizeof(stack_info));
	st.extra=0x10;
    /* prolog and get the upval table */
	dasm_put(Dst, 24);
#if ARM_HF
	push_regs(L,Dst,ct_usr,nargs,&st);
#endif	
    
    dasm_put(Dst, 27, (unsigned short)(L), (((unsigned int)(L))>>16), (unsigned short)(ref), (((unsigned int)(ref))>>16), (unsigned short)(LUA_REGISTRYINDEX), (((unsigned int)(LUA_REGISTRYINDEX))>>16));
	
    /* get the lua function */
    lua_pushvalue(L, fidx);
    lua_rawseti(L, -2, ++num_upvals);
	
	
    dasm_put(Dst, 45, num_upvals);

	// Complex type is return in the address stored in r0 for softfp
	lua_rawgeti(L, ct_usr, 0);
    mt = (const struct ctype*) lua_touserdata(L, -1);
	if(!mt->pointers && !mt->is_reference &&(mt->type==STRUCT_TYPE || mt->type==UNION_TYPE||
	(!ARM_HF&&(mt->type==COMPLEX_DOUBLE_TYPE||mt->type==COMPLEX_FLOAT_TYPE)))&&mt->base_size>4&&!(ARM_HF&&hfa_size(L,-1,mt,NULL))){
		st.int_off+=4;
	} 
	lua_pop(L,1);

	//whether 64 bit type requires 8 bytes alignment in stack is defined by compiler.android compiler reqiures only 4 byte alignment;
	//actually the stack it self may reqiures 8 bytes alignment
    for (i = 1; i <= nargs; i++) {
        lua_rawgeti(L, ct_usr, i);
        mt = (const struct ctype*) lua_touserdata(L, -1);

        if (mt->pointers || mt->is_reference) {
            lua_getuservalue(L, -1);
            lua_rawseti(L, -3, ++num_upvals); /* usr value */
            lua_rawseti(L, -2, ++num_upvals); /* mt */
			
            dasm_put(Dst, 52, num_upvals-1, i, (unsigned short)(mt), (((unsigned int)(mt))>>16));
            load_int(Dst,&st,4,4);
            dasm_put(Dst, 68);
        } else {
            switch (mt->type) {
			case STRUCT_TYPE:
			case UNION_TYPE:{
				#if ARM_HF
				int isfloat,hfasize=0;
				if(mt->type!=UNION_TYPE){
					hfasize=hfa_size(L,-1,mt,&isfloat);
				}
				#endif
				lua_getuservalue(L, -1);
				lua_rawseti(L, -3, ++num_upvals); /* usr value */
				lua_rawseti(L, -2, ++num_upvals); /* mt */
                dasm_put(Dst, 74, num_upvals-1, i, (unsigned short)(mt), (((unsigned int)(mt))>>16));
				#if ARM_HF
				if(hfasize){
					if(hfasize<=4)load_float(Dst,&st,4*hfasize,0,4*(2-isfloat));
					switch(hfasize){
						case 8:
							load_float(Dst,&st,16,0,8);
							load_float(Dst,&st,16,0,8);
							break;
						case 6:
							load_float(Dst,&st,16,0,8);
							load_float(Dst,&st,8,0,8);
							break;
						case 4:
							break;
						case 3:
							break;
						case 2:
							break;
						case 1:
							break;
					}
				}else
				#endif
				if(!mt->is_empty){
					int size=mt->base_size;
					if(size<=4){
						load_int(Dst,&st,4,4);
						dasm_put(Dst, 90);
					}else{
						size=ROUND_UP(size,4);
						if(mt->align_mask>4){//8 byte max alignment
							if(st.int_off<0x40*ARM_HF+MAX_REGS*4){FIX_ALIGN(st.int_off,8);} 
							else {FIX_ALIGN(st.stack_off,8);}
						}
						
						if(st.int_off<0x40*ARM_HF+MAX_REGS*4&&(!st.stack_off||st.int_off+size<=0x40*ARM_HF+MAX_REGS*4)){//to ensure consective memory for the struct
							dasm_put(Dst, 92, st.int_off+st.extra);
							st.int_off+=size;
						}else{
							st.int_off=0x40*ARM_HF+MAX_REGS*4;
							dasm_put(Dst, 95);
							st.stack_off+=size;
						}
						dasm_put(Dst, 97, (unsigned short)(mt->base_size), (((unsigned int)(mt->base_size))>>16), (unsigned short)(memcpy), (((unsigned int)(memcpy))>>16));
					}
				}
                dasm_put(Dst, 107);
				break;
			}
				
			case COMPLEX_DOUBLE_TYPE:
				
				lua_getuservalue(L, -1);
				lua_rawseti(L, -3, ++num_upvals); /* usr value */
				lua_rawseti(L, -2, ++num_upvals); /* mt */
                dasm_put(Dst, 112, num_upvals-1, i, (unsigned short)(mt), (((unsigned int)(mt))>>16));
                load_float(Dst,&st,16,0,8);
                dasm_put(Dst, 128);
				break;
			case COMPLEX_FLOAT_TYPE:
                lua_getuservalue(L, -1);
				lua_rawseti(L, -3, ++num_upvals); /* usr value */
				lua_rawseti(L, -2, ++num_upvals); /* mt */
                dasm_put(Dst, 134, num_upvals-1, i, (unsigned short)(mt), (((unsigned int)(mt))>>16));
				load_float(Dst,&st,8,0,4);
                dasm_put(Dst, 150);
				break;
            case INT64_TYPE:
				
			#if LUA_VERSION_NUM>=503
                lua_pop(L, 1);
				
            #if CK_ALGIN
				FIX_ALIGN(st.int_off,8);
				if(st.int_off<16){
					dasm_put(Dst, 156, st.int_off+st.extra);
					st.int_off+=8;
				}else{
					if(!ALIGNED(st.stack_off,8)){
						st.stack_off+=4;
						dasm_put(Dst, 160);
					}
					dasm_put(Dst, 162);
					st.stack_off+=8;
				}
			#else
				load_int(Dst,st,8,8);
			#endif
                dasm_put(Dst, 164);
			#else
                lua_rawseti(L, -2, ++num_upvals); /* mt */
				
                dasm_put(Dst, 168, (unsigned short)(mt), (((unsigned int)(mt))>>16));
                load_int(Dst,&st,8,8);
                dasm_put(Dst, 177);
			#endif
                break;

            case INTPTR_TYPE:
                lua_rawseti(L, -2, ++num_upvals); /* mt */
				
                dasm_put(Dst, 183, (unsigned short)(mt), (((unsigned int)(mt))>>16));
                load_int(Dst,&st,4,4);
                dasm_put(Dst, 192);
				
                break;

            case BOOL_TYPE:
                lua_pop(L, 1);
				
                load_int(Dst,&st,4,4);
                dasm_put(Dst, 198);
                break;

            case INT8_TYPE: // no need to narrow cause narrowed by caller
            case INT16_TYPE: // no need to narrow cause narrowed by caller
            case ENUM_TYPE:
            case INT32_TYPE:
                lua_pop(L, 1);
				
                load_int(Dst,&st,4,4);
                dasm_put(Dst, 202);
                break;

            case FLOAT_TYPE:
                lua_pop(L, 1);
                
                load_float(Dst,&st,4,ARM_HF,4);
                dasm_put(Dst, 206);
                break;

            case DOUBLE_TYPE:
                lua_pop(L, 1);
				
				#if ARM_HF
				load_float(Dst,&st,8,ARM_HF,8);
				#elif CK_ALGIN
				FIX_ALIGN(st.int_off,8);
				if(st.int_off<16){
					dasm_put(Dst, 210, st.int_off+st.extra);
					st.int_off+=8;
				}else{
					if(!ALIGNED(st.stack_off,8)){
						st.stack_off+=4;
						dasm_put(Dst, 214);
					}
					dasm_put(Dst, 216);
					st.stack_off+=8;
				}
				#else
				load_float(Dst,&st,8,ARM_HF,8);
				#endif	
                dasm_put(Dst, 218);
                break;
				
            default:
                luaL_error(L, "NYI: callback arg type");
            }
        }
    }

    lua_rawgeti(L, ct_usr, 0);
    mt = (const struct ctype*) lua_touserdata(L, -1);

    dasm_put(Dst, 222, ((mt->pointers || mt->is_reference || mt->type != VOID_TYPE) ? 1 : 0), nargs);
    

    if (mt->pointers || mt->is_reference) {
        lua_getuservalue(L, -1);
        lua_rawseti(L, -3, ++num_upvals); /* usr value */
        lua_rawseti(L, -2, ++num_upvals); /* mt */

        dasm_put(Dst, 230, num_upvals-1, (unsigned short)(mt), (((unsigned int)(mt))>>16));
        goto single_no_pop;
    } else {
        switch (mt->type) {
		case STRUCT_TYPE:
		case UNION_TYPE:{
			int isfloat,hfasize=0;
			if(mt->type!=UNION_TYPE){
				hfasize=hfa_size(L,-1,mt,&isfloat);
			}
			lua_getuservalue(L, -1);
			lua_rawseti(L, -3, ++num_upvals); /* usr value */
			lua_rawseti(L, -2, ++num_upvals); /* mt */
            dasm_put(Dst, 246, num_upvals-1, (unsigned short)(mt), (((unsigned int)(mt))>>16));
			#if ARM_HF
			if(hfasize>0){
				switch(hfasize){
					case 8:
						break;
					case 6:
						break;
					case 4:
						break;
					case 3:
						break;
					case 2:
						break;
					case 1:
						break;	
				}
			}else
			#endif
			if(!mt->is_empty){
				if(mt->base_size<=4){
					dasm_put(Dst, 267);
				}else{
					dasm_put(Dst, 269, st.extra+0x40*ARM_HF, (unsigned short)(mt->base_size), (((unsigned int)(mt->base_size))>>16), (unsigned short)(memcpy), (((unsigned int)(memcpy))>>16));
				}
			}
			break;
		}	
        case ENUM_TYPE:
            lua_getuservalue(L, -1);
            lua_rawseti(L, -3, ++num_upvals); /* usr value */
            lua_rawseti(L, -2, ++num_upvals); /* mt */

            dasm_put(Dst, 282, num_upvals-1, (unsigned short)(mt), (((unsigned int)(mt))>>16));

            goto single_no_pop;

        case VOID_TYPE:
            dasm_put(Dst, 298);
            lua_pop(L, 1);
            break;

        case BOOL_TYPE:
        case INT8_TYPE:// narrow it 
        case INT16_TYPE:// narrow it 
        case INT32_TYPE:
		    dasm_put(Dst, 303);
            if (mt->is_unsigned) {
                dasm_put(Dst, 305);
            } else {
                dasm_put(Dst, 309);
            }
			switch(mt->type){
				case BOOL_TYPE:
					dasm_put(Dst, 313);
					break;
				case INT8_TYPE:
					if (mt->is_unsigned) {
						dasm_put(Dst, 316);
					} else {
						dasm_put(Dst, 318);
					}
					break;
				case INT16_TYPE:
					if (mt->is_unsigned) {
						dasm_put(Dst, 320);
					} else {
						dasm_put(Dst, 322);
					}
					break;
			}
            goto single;

        case INT64_TYPE:
            dasm_put(Dst, 324);
            if (mt->is_unsigned) {
                dasm_put(Dst, 326);
            } else {
                dasm_put(Dst, 330);
            }
            goto dual;

        case INTPTR_TYPE:
            dasm_put(Dst, 334);
            goto single;

        case FLOAT_TYPE:
            dasm_put(Dst, 339);
			#if ARM_HF
			lua_pop(L, 1);
			#else
            goto single;
			#endif
			break;
        case DOUBLE_TYPE:
            dasm_put(Dst, 344);
			#if ARM_HF
			lua_pop(L, 1);
            #else
			goto dual;
			#endif
			break;
		case COMPLEX_DOUBLE_TYPE:
            lua_pop(L, 1);
			dasm_put(Dst, 349, st.extra);
			break;
		case COMPLEX_FLOAT_TYPE:
            lua_pop(L, 1);
			dasm_put(Dst, 360, st.extra);
			break;
			
        single:
            lua_pop(L, 1);
		single_no_pop:	
            dasm_put(Dst, 371);
			
            break;
		dual:
			dasm_put(Dst, 378);
			
            lua_pop(L, 1);
			break;
       

        
        default:
            luaL_error(L, "NYI: callback return type");
        }
    }
	
    dasm_put(Dst, 387);
	
    lua_pop(L, 1); /* upval table - already in registry */
    assert(lua_gettop(L) == top);

    {
        void* p;
        struct ctype ft;
        cfunction func;

        func = compile(Dst, L, NULL, ref);

        ft = *ct;
        ft.is_jitted = 1;
        p = push_cdata(L, ct_usr, &ft);
        *(cfunction*) p = func;

        assert(lua_gettop(L) == top + 1);

        return func;
    }
}

static ALWAYS_INLINE void save_int64_stack_align(struct jit* Dst,reg_info* regs,int align){
	if(align&&!ALIGNED(regs->exs,2)){
		regs->exs++;
		dasm_put(Dst, 389);
	}
	dasm_put(Dst, 391);
	regs->exs+=2;
}

static ALWAYS_INLINE void save_int64_align(struct jit* Dst,reg_info* regs,int align){
	if((align&&!ALIGNED(regs->ints,2))||(regs->ints==MAX_REGS-1&&regs->exs/*to ensure consective memory*/)){
		regs->ints++;
	}
	if(regs->ints<MAX_REGS){
		dasm_put(Dst, 393, ((regs->ints<<2)+0x40*ARM_HF));
		regs->ints+=2;
	}else{
		save_int64_stack_align(Dst,regs,align);
	}
	
}

static ALWAYS_INLINE  void save_int64(struct jit* Dst,reg_info* regs){
	save_int64_align(Dst,regs,1);
}

static ALWAYS_INLINE void save_int_stack_align(struct jit* Dst,reg_info* regs){
	dasm_put(Dst, 397);
	regs->exs++;
}

static ALWAYS_INLINE void save_int(struct jit* Dst,reg_info* regs){
	if(regs->ints<MAX_REGS){
		dasm_put(Dst, 399, ((regs->ints++<<2)+0x40*ARM_HF));
	}else{
		save_int_stack_align(Dst,regs);
	}
}

static void save_float(struct jit* Dst,reg_info* regs,int size,int isfloat){
#if ARM_HF
	if(!regs->float_sealed){
		int reg=add_float_reg(regs,size,isfloat);
		if(reg<0) goto SAVE_STACK;
		switch(size){
		case 8:
		case 6:
		case 4:
			goto sf_2;
		case 3:
		case 2:
			sf_2:
			break;
		case 1:
			break;
		}
		return;
	}
	SAVE_STACK:
	
	if(!isfloat&&!ALIGNED(regs->exs,2)){
		regs->exs++;
		dasm_put(Dst, 402);
	}
	switch(size){
		case 1:
			dasm_put(Dst, 404);
			break;
		case 2:
			dasm_put(Dst, 406);
			break;
		case 3:
			dasm_put(Dst, 408);
			break;
		case 4:
			dasm_put(Dst, 410);
			break;
		case 6:
			dasm_put(Dst, 412);
			break;
		case 8:
			dasm_put(Dst, 414);
			break;	
		
	}
	regs->exs+=size;
	
#else
	if(size==1){
		save_int(Dst,regs);
	}else if(size==2){
		save_int64_align(Dst,regs,!isfloat);
	}
#endif
}

static int calculate_stack(lua_State* L,int ct_usr,int nargs){
	const struct ctype* mt;
	reg_info regs;int i,stack=0;
	memset(&regs,0,sizeof(reg_info));
    for (i = 1; i <= nargs;++i){
		lua_rawgeti(L, ct_usr, i);
		mt = (const struct ctype*) lua_touserdata(L, -1);
		if (mt->pointers || mt->is_reference) {
			stack+=add_int_reg(&regs);
		}else{
			switch(mt->type){
				case COMPLEX_DOUBLE_TYPE:
					#if ARM_HF
					stack+=add_float_reg(&regs,4,0)<0?4:0;
					#else
					stack+=add_int64_reg(&regs);
					stack+=add_int64_reg(&regs);
					#endif
					FIX_ALIGN(stack,2);
					break;
				case DOUBLE_TYPE:
					#if ARM_HF
					stack+=add_float_reg(&regs,2,0)<0?2:0;
					#else
					stack+=add_int64_reg(&regs);
					#endif
					FIX_ALIGN(stack,2);
					break;
				case COMPLEX_FLOAT_TYPE:// Though complex alignment is 4, but vfp requires a sequence of regsiters
					#if ARM_HF
					stack+=add_float_reg(&regs,2,1)<0?2:0;
					#else
					stack+=add_int_reg(&regs);
					stack+=add_int_reg(&regs);
					#endif
					break;
				case FLOAT_TYPE:
					#if ARM_HF
					stack+=add_float_reg(&regs,1,1)<0?1:0;
					#else
					stack+=add_int_reg(&regs);
					#endif
					break;
				case INT64_TYPE:
					stack+=add_int64_reg(&regs);
					FIX_ALIGN(stack,2);
					break;
				case STRUCT_TYPE:{
					#if ARM_HF
					int isfloat;
					int hfasize=hfa_size(L,-1,mt,&isfloat);
                    if(hfasize>0){
						stack+=add_float_reg(&regs,2,0)<0?hfasize:0;
						if(!isfloat){
							FIX_ALIGN(stack,2);
						}
						break;
                    }
					#endif
				}
				case UNION_TYPE:{
					int intsize=(mt->base_size+3)>>2;
					if(mt->align_mask>4){//8-byte max alignment
						if(regs.ints<MAX_REGS){
							FIX_ALIGN(regs.ints,2);
						}else{
							FIX_ALIGN(stack,2);
						}
					}
					
					if(regs.ints+intsize<=MAX_REGS){
						regs.ints+=intsize;
					}else{
						stack+=regs.ints+intsize-MAX_REGS;
						regs.ints=MAX_REGS;
					}
					break;
				}
				default:
					stack+=add_int_reg(&regs);//no need to check type support here
			}
		}
		lua_pop(L,1);
	}
	FIX_ALIGN(stack,2);
	return (regs.ints
	#if ARM_HF
	/**/||regs.floats
	#endif
	)?0x40*ARM_HF+0x10+stack*4:0;
}

void compile_function(lua_State* L, cfunction func, int ct_usr, const struct ctype* ct)
{
    struct jit* Dst = get_jit(L);;
    int i, nargs, num_upvals,ret_by_addr, stack_size;
    const struct ctype* mt;
    void* p; reg_info regs;

    int top = lua_gettop(L);

    ct_usr = lua_absindex(L, ct_usr);
    nargs = (int) lua_rawlen(L, ct_usr);

    p = push_cdata(L, ct_usr, ct);
    *(cfunction*) p = func;
    num_upvals = 1;

    dasm_setup(Dst, build_actionlist);
#if defined __thumb__ //keep frame pointer
    dasm_put(Dst, 416);
#else
    dasm_put(Dst, 419);
#endif
#if CK_ALGIN
	dasm_put(Dst, 422);
#endif	
    dasm_put(Dst, 424);
    
    /* Reserve enough stack space for all of the arguments. For hard floating point,
	 * leave extra 64 bytes
	 */
	stack_size=calculate_stack(L,ct_usr,nargs);
	lua_rawgeti(L, ct_usr, 0);
    mt = (const struct ctype*) lua_touserdata(L, -1);
	
	// Complex types in softfp and structs/unions larger than 4-bytes are return in the address stored in r0
	ret_by_addr=!mt->pointers && !mt->is_reference &&(mt->type==STRUCT_TYPE || mt->type==UNION_TYPE||
	(!ARM_HF&&(mt->type==COMPLEX_DOUBLE_TYPE||mt->type==COMPLEX_FLOAT_TYPE)))&&mt->base_size>4&&!(ARM_HF&&hfa_size(L,-1,mt,NULL));
	lua_pop(L,1);
	if(ret_by_addr){
		if(stack_size==0)
			stack_size=0x40*ARM_HF+0x10;
		stack_size+=8;
	}
	if(stack_size>0){
		if(stack_size>=1<<12){
			dasm_put(Dst, 426, (unsigned short)(stack_size), (((unsigned int)(stack_size))>>16));
		}else{
			dasm_put(Dst, 432, stack_size);
		}
		if (ct->has_var_arg){
			dasm_put(Dst, 435, nargs, (unsigned short)("too few arguments"), (((unsigned int)("too few arguments"))>>16));
		}
		dasm_put(Dst, 452, 0x40*ARM_HF+0x10);
	} 
	
	memset(&regs,0,sizeof(reg_info));
	
    if (ret_by_addr) {	
		regs.ints++;
		dasm_put(Dst, 455, (unsigned short)(mt), (((unsigned int)(mt))>>16), 0x40*ARM_HF);
	}
	
	
    for (i = 1; i <= nargs; i++) {
        lua_rawgeti(L, ct_usr, i);
        mt = (const struct ctype*) lua_touserdata(L, -1);
		
        if (mt->pointers || mt->is_reference || mt->type == FUNCTION_PTR_TYPE || mt->type == ENUM_TYPE|| mt->type==STRUCT_TYPE || mt->type==UNION_TYPE) {
            lua_getuservalue(L, -1);
            num_upvals += 2;

            dasm_put(Dst, 466, (unsigned short)(mt), (((unsigned int)(mt))>>16), (unsigned short)(lua_upvalueindex(num_upvals)), (((unsigned int)(lua_upvalueindex(num_upvals)))>>16), i);
			
            if (mt->pointers || mt->is_reference) {
                dasm_put(Dst, 477);
            } else if (mt->type == FUNCTION_PTR_TYPE) {
                dasm_put(Dst, 481);
            } else if (mt->type == ENUM_TYPE) {
                dasm_put(Dst, 485);
            }else if(mt->type==STRUCT_TYPE || mt->type==UNION_TYPE){
				if(mt->is_empty) continue;
				dasm_put(Dst, 489);
				#if ARM_HF
				{
					int hfasize,isfloat;
					hfasize=hfa_size(L,-2,mt,&isfloat);
					if(hfasize){
						switch(hfasize){
							case 8:
								break;
							case 6:
								break;
							case 4:
								break;
							case 3:
								break;
							case 2:
								break;
							case 1:
								break;
								
						}
						save_float(Dst,&regs,hfasize,isfloat);
						continue;
					}
				}
				#endif
				
				if(mt->align_mask>4){//8 byte max alignment 
					if(regs.ints<4){
						FIX_ALIGN(regs.ints,2)
					}else if(!ALIGNED(regs.exs,2)){
						int diff=regs.exs;
						regs.exs+=4;
						dasm_put(Dst, 493);
					}
				}
				int size=ROUND_UP(mt->base_size,4)>>2;
				dasm_put(Dst, 495, (unsigned short)(mt->base_size), (((unsigned int)(mt->base_size))>>16), (unsigned short)(memcpy), (((unsigned int)(memcpy))>>16));
				if(regs.ints<MAX_REGS&&(!regs.exs||regs.ints+size<=MAX_REGS)){//to ensure consective memory for the struct
					dasm_put(Dst, 505, ((regs.ints<<2)+0x40*ARM_HF));
				}else{
					regs.ints=MAX_REGS;
					dasm_put(Dst, 508);
				}
				dasm_put(Dst, 510);
				if(regs.ints+size<=MAX_REGS){
					regs.ints+=size;
				}else{
					int ex=regs.ints+size-MAX_REGS;
					dasm_put(Dst, 512, (ex)<<2);
					regs.exs+=ex;
					regs.ints=MAX_REGS;
				}
				continue;
			}

            save_int(Dst,&regs);
        } else {
            lua_pop(L, 1);
            dasm_put(Dst, 515, i);

            switch (mt->type) {
			case BOOL_TYPE:
				dasm_put(Dst, 518);
                save_int(Dst,&regs);
                break;
            case INT8_TYPE:
                dasm_put(Dst, 524);
                if (mt->is_unsigned) {
                     dasm_put(Dst, 528);
                } else {
                     dasm_put(Dst, 530);
                }
                save_int(Dst,&regs);
                break;

            case INT16_TYPE:
                dasm_put(Dst, 532);
                if (mt->is_unsigned) {
                    dasm_put(Dst, 536);
                } else {
                    dasm_put(Dst, 538);
                }
                save_int(Dst,&regs);
                break;

            case INT32_TYPE:
                if (mt->is_unsigned) {
                    dasm_put(Dst, 540);
                } else {
                    dasm_put(Dst, 544);
                }
                save_int(Dst,&regs);
                break;
            case INTPTR_TYPE:
                dasm_put(Dst, 548);
                save_int(Dst,&regs);
				break;

            case INT64_TYPE:
                if (mt->is_unsigned) {
                    dasm_put(Dst, 552);
                } else {
                    dasm_put(Dst, 556);
                }
				save_int64(Dst,&regs);
                break;

            case DOUBLE_TYPE:
				dasm_put(Dst, 560);
				save_float(Dst,&regs,2,0);
                break;

            case FLOAT_TYPE:
				dasm_put(Dst, 564);
                save_float(Dst,&regs,1,1);
                break;
				
			case COMPLEX_DOUBLE_TYPE:
				#if ARM_HF
				save_float(Dst,&regs,4,0);
				#else
				FIX_ALIGN(regs.ints,2);
				dasm_put(Dst, 568);
				if(regs.ints<MAX_REGS&&(!regs.exs||regs.ints+4<=MAX_REGS)){
					dasm_put(Dst, 571, (regs.ints<<2));
				}else{
					regs.ints=MAX_REGS;
					if(!ALIGNED(regs.exs,2)){
						++regs.exs;
						dasm_put(Dst, 574);
					}
					dasm_put(Dst, 576);
				}
				dasm_put(Dst, 578);
				regs.ints+=4;
				goto FIX_REG;
				#endif
				break;
			case COMPLEX_FLOAT_TYPE:
				#if ARM_HF
				save_float(Dst,&regs,2,1);
				#else
				dasm_put(Dst, 581);
				if(regs.ints<MAX_REGS&&(!regs.exs||regs.ints+2<=MAX_REGS)){
					dasm_put(Dst, 584, (regs.ints<<2));
				}else{
					regs.ints=MAX_REGS;
					dasm_put(Dst, 587);
				}
				dasm_put(Dst, 589);
				regs.ints+=2;
				FIX_REG:
				if(regs.ints>MAX_REGS){
					dasm_put(Dst, 592, (regs.ints-4)<<2);
					regs.exs+=regs.ints-MAX_REGS;
					regs.ints=MAX_REGS;
				}
				#endif
                break;
            default:
                luaL_error(L, "NYI: call arg type");
            }
        }
    }

    if (ct->has_var_arg) {
		int offset=nargs+1;
        dasm_put(Dst, 595, offset);
		#if ARM_HF
		    if(regs.ints==4||regs.float_sealed){
				if(regs.ints<4&&regs.float_sealed){//some arg must be loaded to core registers.
				}
			} 
		#else
			if(regs.ints==4){
				dasm_put(Dst, 598);
			}
		#endif
		else{//no hard floating point in variadic procedure
			dasm_put(Dst, 600, ((regs.ints<<2)+ARM_HF*0x40));
		}
        
        dasm_put(Dst, 603);
		regs.ints=4;
    } 
	
	#if ARM_HF
	switch(ROUND_UP(regs.highest_bit,4)>>1){
		case 8 :
			break;
		case 7 :
			break;
		case 6 :
			break;
		case 5:
			break;
		case 4 :
			break;
		case 3 :
			break;
		case 2 :
			break;
		case 1 :
			break;
	}
	if(stack_size>0){
	}
	#endif
	
	//pop registers from stack,align 8 for some compiler
	assert(regs.ints<=4);
	switch(regs.ints){
	case 4:
	case 3:
		dasm_put(Dst, 608);
		break;
	case 2:
	case 1:
		dasm_put(Dst, 610);
		#if ARM_HF
		if(regs.float_sealed){
		}
		#endif
		break;
	default:
		#if ARM_HF
		if(regs.float_sealed){
		}
		#endif
		break;
	}
	
	dasm_put(Dst, 612, (unsigned short)(func), (((unsigned int)(func))>>16));

    lua_rawgeti(L, ct_usr, 0);
    mt = (const struct ctype*) lua_touserdata(L, -1);

    if (mt->pointers || mt->is_reference || mt->type==FUNCTION_PTR_TYPE) {
        lua_getuservalue(L, -1);
        num_upvals += 2;
        dasm_put(Dst, 618, (unsigned short)(mt), (((unsigned int)(mt))>>16), (unsigned short)(lua_upvalueindex(num_upvals)), (((unsigned int)(lua_upvalueindex(num_upvals)))>>16));

    } else {
        switch (mt->type) {
        case INT64_TYPE:
        #if LUA_VERSION_NUM>=503
             lua_pop(L, 1);
	    #if CK_ALGIN
            dasm_put(Dst, 633);
		#else
            dasm_put(Dst, 636);
		#endif		
            dasm_put(Dst, 639);
            break;
		#else
			num_upvals++;
            dasm_put(Dst, 644, (unsigned short)(mt), (((unsigned int)(mt))>>16));
            break;
        #endif
		
        case INTPTR_TYPE:
            num_upvals++;
            dasm_put(Dst, 658, (unsigned short)(mt), (((unsigned int)(mt))>>16));
            break;

        case VOID_TYPE:
            lua_pop(L, 1);
            dasm_put(Dst, 670);
            break;

        case BOOL_TYPE:
            lua_pop(L, 1);
            dasm_put(Dst, 672);
            break;

        case INT8_TYPE:
        case INT16_TYPE:
        case INT32_TYPE:
        case ENUM_TYPE:// value must be narrowed before callee return
            lua_pop(L, 1);
			
            dasm_put(Dst, 678);
            if (mt->is_unsigned) {
                dasm_put(Dst, 680);
            } else {
                dasm_put(Dst, 684);
            }
			
            dasm_put(Dst, 688);
            break;

        case FLOAT_TYPE:
            lua_pop(L, 1);
            dasm_put(Dst, 690);
            break;

        case DOUBLE_TYPE:
            lua_pop(L, 1);
        #if CK_ALGIN
            dasm_put(Dst, 696);
		#else
			dasm_put(Dst, 699);
		#endif	
            dasm_put(Dst, 702);
            break;
		case COMPLEX_DOUBLE_TYPE:
		case COMPLEX_FLOAT_TYPE:
		case STRUCT_TYPE:
		case UNION_TYPE:{
			lua_getuservalue(L,-1);
            num_upvals += 2;
			#if ARM_HF
			{
				int isfloat,hfasize=hfa_size(L,-2,mt,&isfloat);
				if(hfasize>0){
					switch(hfasize){
						case 8:
							break;
						case 6:
							break;
						case 4:
						case 3:
							break;
						case 2:
						case 1:
							break;
							
					}
					switch(hfasize){
						case 8:
							break;
						case 6:
							break;
						case 4:
							break;
						case 3:
							break;
						case 2:
							break;
						case 1:
							break;
							
					}
					break;
				}
			}
			
			#endif
			if(mt->base_size>4){
				// value are stored in return storage in r0 for softfp, set usr value here
				if(!lua_isnil(L,-1)){
					dasm_put(Dst, 707, (unsigned short)(lua_upvalueindex(num_upvals)), (((unsigned int)(lua_upvalueindex(num_upvals)))>>16));
				}
			}else if(mt->is_empty){
				dasm_put(Dst, 719);
				break;
			}else{
				dasm_put(Dst, 721, (unsigned short)(mt), (((unsigned int)(mt))>>16), (unsigned short)(lua_upvalueindex(num_upvals)), (((unsigned int)(lua_upvalueindex(num_upvals)))>>16));
			}
            dasm_put(Dst, 735);
			break;
		}	
        default:
            luaL_error(L, "NYI: call return type");
        }
    }

#ifdef __thumb__
    dasm_put(Dst, 737);
#else	
    dasm_put(Dst, 740);
#endif	
    assert(lua_gettop(L) == top + num_upvals);
    {
        cfunction f = compile(Dst, L, NULL, LUA_NOREF);
        /* add a callback as an upval so that the jitted code gets cleaned up when
         * the function gets gc'd */
        push_callback(L, f, func);
        lua_pushcclosure(L, (lua_CFunction) f, num_upvals+1);
    }
}


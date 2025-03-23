/*
 * This file is part of Cleanflight and Betaflight.
 *
 * Cleanflight and Betaflight are free software. You can redistribute
 * this software and/or modify this software under the terms of the
 * GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Cleanflight and Betaflight are distributed in the hope that they
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once                                                    // 防止头文件内容被多次包含

#include <stddef.h>
#include <stdint.h>

#define NOOP do {} while (0)                                    // 用作占位符  空循环什么也不执行，保证不会报错,记得  ';'  号

#define ARRAYLEN(x) (sizeof(x) / sizeof((x)[0]))                // 得到数组长度
#define ARRAYEND(x) (&(x)[ARRAYLEN(x)])                         // 得到数组最后一位的下一个地址, 使用时需注意不要越界, 仅适用于静态数组,写法向C++靠拢.

#define CONST_CAST(type, value) ((type)(value))                 // const 移除  将const type  变为 type  例如 : const int a;  CONST_CAST(int, a) consst int =-> int.

#define CONCAT_HELPER(x,y) x ## y                               // ## 号作为连接使用. 将x y 进行拼接,  CONCAT_HELPER(a,b) = ab
#define CONCAT(x,y) CONCAT_HELPER(x, y)                         // 拼接 x 和  y,
#define CONCAT2(_1,_2) CONCAT(_1, _2)
#define CONCAT3(_1,_2,_3)  CONCAT(CONCAT(_1, _2), _3)
#define CONCAT4(_1,_2,_3,_4)  CONCAT(CONCAT3(_1, _2, _3), _4)   // 同样的拼接逻辑

#define STR_HELPER(x) #x                                        // 它的主要作用是将宏参数转换为字符串字面量
#define STR(x) STR_HELPER(x)                                    // 将宏参数转换为字符串字面量

#define EXPAND_I(x) x                                           // 作为间接层，强制展开传入的参数
#define EXPAND(x) EXPAND_I(x)                                   // 通过间接调用 EXPAND_I，确保参数 x 被完全展开

// expand to t if bit is 1, f when bit is 0. Other bit values are not supported
#define PP_IIF(bit, t, f) PP_IIF_I(bit, t, f)                           // 将参数传递给 PP_IIF(bit, t, f)
#define PP_IIF_I(bit, t, f) PP_IIF_ ## bit(t, f)                        // bit作为参数, 若bit为0 则调用PP_IIF_0, 否则 PP_IIF_1
#define PP_IIF_0(t, f) f                                                // 使用方法  PP_IIF(0, 1, 2)  返回 2
#define PP_IIF_1(t, f) t                                                // 使用方法  PP_IIF(1, 1, 2)  返回 1


// Expand all arguments and call macro with them. When expansion of some argument contains ',', it will be passed as multiple arguments
// #define TAKE3(_1,_2,_3) CONCAT3(_1,_2,_3)
// #define MULTI2 A,B
// PP_CALL(TAKE3, MULTI2, C) expands to ABC
#define PP_CALL(macro, ...) macro(__VA_ARGS__)                  // 调用一个宏 macro， __VA_ARGS__为可变参数列表, 向一个宏传参



#if !defined(UNUSED)                                            // 是否定义了 UNUSED 宏， 使得定义未使用的参数，不报警告.
#define UNUSED(x) (void)(x) // Variables and parameters that are not used
#endif

#define DISCARD(x) (void)(x) // To explicitly ignore result of x (usually an I/O register access).  // 忽略某个值使用.和上一个类似.

#define STATIC_ASSERT(condition, name) _Static_assert((condition), #name)                       //  校验某个条件是否为真


#define BIT(x) (1 << (x))                                       // 将第x位变为1 用法: reg | BIT(3),  reg &= ~BIT(3),  if(reg & BIT(x))

/*
http://resnet.uoregon.edu/~gurney_j/jmpc/bitwise.html
*/
#define BITCOUNT(x) (((BX_(x)+(BX_(x)>>4)) & 0x0F0F0F0F) % 255)                                         //  得到为 1 的总数
#define BX_(x) ((x) - (((x)>>1)&0x77777777) - (((x)>>2)&0x33333333) - (((x)>>3)&0x11111111))            // 每个 4 位段的二进制值即为该 4 位中 1 的个数。


/*
* https://groups.google.com/forum/?hl=en#!msg/comp.lang.c/attFnqwhvGk/sGBKXvIkY3AJ
* Return (v ? floor(log2(v)) : 0) when 0 <= v < 1<<[8, 16, 32, 64].
* Inefficient algorithm, intended for compile-time constants.
*/
#define LOG2_8BIT(v)  (8 - 90/(((v)/4+14)|1) - 2/((v)/2+1))
#define LOG2_16BIT(v) (8*((v)>255) + LOG2_8BIT((v) >>8*((v)>255)))
#define LOG2_32BIT(v) (16*((v)>65535L) + LOG2_16BIT((v)*1L >>16*((v)>65535L)))
#define LOG2_64BIT(v) \
(32*((v)/2L>>31 > 0) \
+ LOG2_32BIT((v)*1L >>16*((v)/2L>>31 > 0) \
                        >>16*((v)/2L>>31 > 0)))
#define LOG2(v) LOG2_64BIT(v)                           //  计算以2为底的对数.

#if 0
// ISO C version, but no type checking
#define container_of(ptr, type, member) \
                ((type *) ((char *)(ptr) - offsetof(type, member)))
#else
// non ISO variant from linux kernel; checks ptr type, but triggers 'ISO C forbids braced-groups within expressions [-Wpedantic]'
//  __extension__ is here to disable this warning
/*
typeof( ((type *)0)->member )：获取成员 member 的类型。
const typeof(...) *__mptr = (ptr)：定义一个临时指针 __mptr，用于类型检查。
(char *)__mptr - offsetof(type, member)：从成员指针减去偏移量，得到结构体的起始地址。
(type *)：将结果强制转换为结构体指针。
__extension__：禁用 GCC 的警告（-Wpedantic），因为使用了非 ISO C 的语法（复合语句）。
*/
#define container_of(ptr, type, member)  ( __extension__ ({     \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );}))  // 通过结构体变量获取结构体指针.


static inline int16_t cmp16(uint16_t a, uint16_t b) { return (int16_t)(a-b); } // 返回是否相等,
static inline int32_t cmp32(uint32_t a, uint32_t b) { return (int32_t)(a-b); } // 返回是否相等

static inline uint32_t llog2(uint32_t n) { return 31 - __builtin_clz(n | 1); }  // 作用:计算⌊log2(n)⌋⌊log 2(n)⌋,即n的二进制表示中最高有效位的位置(从0开始计数)


// using memcpy_fn will force memcpy function call, instead of inlining it. In most cases function call takes fewer instructions
//  than inlined version (inlining is cheaper for very small moves < 8 bytes / 2 store instructions)
#if defined(UNIT_TEST) || defined(SIMULATOR_BUILD)                      // 如果是测试或仿真
// Call memcpy when building unittest - this is easier that asm symbol name mangling (symbols start with _underscore on win32)
#include <string.h>
// 定义为内联函数，减少函数出入栈的操作次数。
static inline void  memcpy_fn ( void * destination, const void * source, size_t num ) { memcpy(destination, source, num); }      // 同一函数名 memcpy_fn 在不同环境下无缝切换实现，提升代码可维护性
#else
void * memcpy_fn ( void * destination, const void * source, size_t num ) asm("memcpy");          // 内嵌汇编 memcpy
#endif


#endif

#if __GNUC__ > 6
#define FALLTHROUGH ;__attribute__ ((fallthrough))                      // 用于指示编译器忽略 switch语句没有break的警告
#else
#define FALLTHROUGH do {} while(0)
#endif

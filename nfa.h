/*************************************************
*            NFA引擎主模块                      *
*************************************************/

#ifndef NFA_H
#define NFA_H

/*该模块对外提供函数scanListWithCharPosi 。该函数输入比较多
struct regex_list * const plow 一串正则单元的开始结点
struct regex_list * const phigh 一串正则单元的结束结点
const char * text 指向文本
const int start_char 文本的从哪个字符开始匹配
int * presult 指向一个整数 保存当前状态匹配到了文本中的字符的偏移量
该函数返回值为是否匹配*/

BOOL scanListWithCharPosi(struct regex_list * const plow, struct regex_list * const phigh, const char * text, const int start_char, int * presult);

BOOL canGreedyBack(struct regex_list * p);	//判断该结点不满足要求时是否可以回退
BOOL greedyBack(struct regex_list ** p, int * i);	//贪婪回退 会修改p和i为回退后下一个结点和文本位置

#endif // NFA_H


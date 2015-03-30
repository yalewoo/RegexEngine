/*************************************************
*            单词匹配主模块                     *
*************************************************/

#ifndef MATCH_H
#define MATCH_H

/*该模块对外提供match函数，
输入为一个单元的正则表达式(例如 \w \d [sfsd]等 )
和 指向文本的指针以及字符开始的偏移量
返回值为是否匹配*/

BOOL match(const struct regex_list * p, const char * text, int i);	//判断某字符和表达式是否匹配总函数

BOOL matchPosi1(const char* pchar, const char* text, int i);    //判断^和$
BOOL match1(const char * pchar, const char ch);	//用于匹配.和其他普通字符
BOOL matchPosi2(const char* pchar, const char* text, int i);    //判断\b \B类型
BOOL match2(const char* pchar, const char ch);  //判断\w \s \d \W \S \B类型
BOOL matchRange(const char *pchar, const int len, char ch); //判断[x]类型

#endif // MATCH_H




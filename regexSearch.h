/*************************************************
*          正则表达式主模块                     *
*************************************************/

#ifndef REGEX_SEARCH_H
#define REGEX_SEARCH_H

/*该模块调用了词法分析（LEXICAL_ANALYSIS_H）模块单词匹配模块（MATCH_H）以及正则引擎NFA模块
该模块对外提供函数 BOOL regexSearch(char * regex_text, char * text, char * result)
输入值是指向正则表达式字符串的指针，指向带匹配文本字符串的指针，以及用来存放匹配结果的缓冲区指针
该函数的返回值是是否匹配成功。
此外，该模块还对外提供了charType()函数，被多个子模块调用，用来判断单词的所属分类
*/

#include "match.h"
#include "lexicalAnalysis.h"
#include "nfa.h"

BOOL regexSearch(char * regex_text, char * text, char * result);

int charType(const char *);
BOOL destroyList(struct regex_list * head);

#endif // REGEX_SEARCH_H



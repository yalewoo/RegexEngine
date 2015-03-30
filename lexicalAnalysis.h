/*************************************************
*            词法分析主模块                     *
*************************************************/

#ifndef LEXICAL_ANALYSIS_H
#define LEXICAL_ANALYSIS_H

/*该模块对外提供void lexicalAnalysis(char * arr, struct regex_list * head)函数
该函数输入为指向正则表达式字符串的指针 和 保存分词后信息的链表头结点
该函数会将分词的结果存入链表
该函数没有返回值*/

int lexicalAnalysis(char * arr, struct regex_list * head);	//把正则规则数组分解为一个个单元存到链表中

struct regex_list * saveToList(struct regex_list * head, struct regex_list * p, char * arr, int low, int high, int posi_start, int * group, int top);
void setRate(char* pchar, int rate[3]);
int charToInt(int i,char* pchar);

#endif // LEXICAL_ANALYSIS_H




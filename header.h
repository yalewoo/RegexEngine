
#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>



#define BOOL int
#define TRUE 1
#define FALSE 0

#define OVERFLOW -1



#define MAX_RESULT_LENGTH 1024      //匹配结果缓存区大小
#define FILE_NAME_MAX_LENGTH 40 	//文件名最大长度
#define BUFFER_LEN 1025		//正则表达式长度
#define UNIT_LEN 1025		//单元正则表达式最长长度
#define MAX_GROUP 1025 	//最大左括号数
#define MAX_BRANCH 1025	//一个括号中最大分支数
#define MAX_MATCHED_CHAR_POSI 1025 	//匹配多字符的单元或括号 匹配重复的最大次数 + 1 （多余的一个用来存放起始位置）

#define  UNMATCHED_BRACKET 10   //括号不匹配错误代码

struct regex_list
{
	char unit[UNIT_LEN];	//保存单位正则字符
	int posi_start; //分割元字符和出现次数的下标
	//unit[0]到unit[posi_start-1]是元字符
	int rate[3];	//三位数字。
	//第一位0表示出现次数是rate[1]到rate[2]的闭区间
	//第一位1表示出现次数是 rate[1]次及以上
	/*
	* 		编码为 1 0
	+ 		编码为 1 1
	? 		编码为 0 0 1
	{n}		编码为 0 n n
	{n,} 	编码为 1 n
	{n,m}	编码为 0 n m
	*/
	int group[MAX_GROUP];   //记录分组信息。保存该结点所属的所有分组 group[n]==1则属于第n组

	int is_posi;	//记录该元字符是不是匹配位置信息（零宽度）
	int is_back_ref;	//记录该元字符是不是分组后的后项引用

	struct regex_list * pred;
	struct regex_list * next;   //构成双向链表

	int low;
	int high;	//low到high闭区间是 该单元匹配到的字符的下标范围

	int matched_num;	//仅用于右括号和后项引用 记录该组匹配了几次 用于贪婪回退
	int matched_char_posi[MAX_MATCHED_CHAR_POSI][2];	//仅用于右括号和后项引用单元 记录每次匹配位置 第一次为[ matched_arr[0],matched_arr[1] ) 最后一次为 [matched_arr[matched_num-1], matched_arr[matched_num]) 。
	//第二维度 记录每次匹配是用的该分组内的哪个分支

	int branch_num;	//如果是左括号或头结点 记录分支符号|的数量
	struct regex_list * pbranch[MAX_BRANCH];	//如果是左括号或头结点 记录该组内的所有分支|的位置 从1开始

	int group_id;	//如果是括号记录该括号是第几个分组 或该单元所属的最小分组
	struct regex_list * pbracket;	//如果是括号 这个指针指向和该括号配对的另一个括号
};

#endif // HEADER_H



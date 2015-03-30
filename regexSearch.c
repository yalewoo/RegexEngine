#include "header.h"

extern int lexicalAnalysis(char * arr, struct regex_list * head);	//分词模块。把正则规则数组分解为一个个单元存到链表中
extern BOOL scanListWithCharPosi(struct regex_list * const plow, struct regex_list * const phigh, const char * text, const int start_char, int * presult);


/*
* 函数名称：charType
* 函数功能：返回一个字符的类型
* 输入参数：指向字符的指针
* 返回值：该字符的类型
*/
/*各种字符和类型的编码如下
编码分为两位 第一位表示所在的组
1组 元字符
    11 第1组第一个 .
    12 第1组第2种 ^和$
    13 \b \B
    14 \w \s \d \W \S \D
    15 [x] [agd]
2组 重复次数
    21 * ? +
    22 {n}类型
3组 分支
    31 |
4组 分组
    41 （
    42 ）
    43 后向引用 \1 \2 \3 \4 \5 \6 \7 \8 \9
其他普通字符编号为1 */
int charType(const char * ch)
{
	switch (*ch)
	{
	case '.': return 11;
	case '^':;
	case '$': return 12;
	case '\\':
	{
		if (*(ch + 1) >= '0' && *(ch + 1) <= '9')
			return 43;	//后向引用中代表某个分组
		else if (*(ch + 1) == 'b' || *(ch + 1) == 'B')
			return 13;	//元字符中的位置 \b \B
		else
			return 14;	//元字符中的转义\w \s \d \W \S \D
	}
	case '[': return 15;
	case '*':;
	case '?':;
	case '+': return 21;
	case '{': return 22;
	case '|': return 31;
	case '(': return 41;
	case ')': return 42;
	default: return 1;	//单纯的文本字符
	}
}

/*
* 函数名称：destroyList
* 函数功能：销毁链表 不销毁头结点
* 输入参数：链表头指针
* 返回值：成功返回TRUE 失败返回FALSE
*/
static BOOL destroyList(struct regex_list * head)
{
	struct regex_list * p = head->next;
	struct regex_list * q = p;
	while (p->next != NULL)
	{
		q = p->next;
		free(p);
		p = q;
	}
	return TRUE;
}

/*
* 函数名称：regexSearch
* 函数功能：该函数完成正则和文本匹配的所有过程
* 输入参数：regex_text指向的是正则表达式字符串，text指向文本字符串。line是输出文件的序号
* 返回值：是否匹配 或 错误码 0 不匹配 1 匹配
*/
int regexSearch(char * regex_text, char * text, char * result)
{
	int error_index = 1;
	int i;
	int j;
	int start_char;
	int end_char = 0;
	struct regex_list head;
	struct regex_list tail;
	//struct regex_list * p;
	//struct regex_list * q;

	BOOL found = 0;	//查找是否成功标志


	//int back_ref_group_changed = 0;


	head.next = &tail;
	head.pred = NULL;
	tail.pred = &head;
	tail.next = NULL;
	head.branch_num = 0;
	for (i = 0; i < MAX_GROUP; ++i)	//
	{
		head.group[i] = 0;
	}
	head.is_posi = 0;
	head.is_back_ref = 0;
	//head看做整体的左括号 为了递归的时候方便
	head.unit[0] = '(';
	head.unit[1] = '\0';
	head.group[0] = 1;
	head.group_id = 0;
	head.pbranch[0] = &head;
	head.pbranch[1] = &tail;
	head.pbracket = &tail;
	tail.unit[0] = ')';
	tail.unit[1] = '\0';
	tail.pbracket = &head;
	tail.rate[0] = 0;
	tail.rate[1] = 1;
	tail.rate[2] = 1;

	if ((error_index = lexicalAnalysis(regex_text, &head)) != 1)
        return error_index;
	//q = head.next;
	//p = head.next;


	//处理列表中的正则单元

	//输出相关信息
	/*for (i = 0; p->next != NULL; ++i)
	{
		printf("%d\t:%s\t", i, p->unit);
		//printf("posi %d\t", p->posi_start);
		printf("group:");
		for (j = 0; j < 10; ++j)
		{
			printf("%d ", p->group[j]);
		}
		printf("group_id: %d", p->group_id);
		//printf("is_posi: %d\t", p->is_posi);
		//printf("is_back_ref: %d\t", p->is_back_ref);
		printf("rate: %d %d %d", p->rate[0], p->rate[1], p->rate[2]);

		printf("\n");
		p = p->next;
	}
	printf("there is %d |\n", head.branch_num);
	if (head.branch_num != 0)
	{
		printf("%s\n", head.pbranch[1]->unit);
	}*/

	//匹配总循环
	for (start_char = 0; text[start_char] != '\0'; ++start_char)
	{
		if ((found = scanListWithCharPosi(&head, &tail, text, start_char, &end_char)) == TRUE)
		{
			break;
		}
	}

	if (text[start_char] != '\0')	//匹配成功
	{
	    j = 0;


		for (i = start_char; i < end_char; ++i)
		{

            result[j] = text[i];
            j++;
		}
		result[j] = '\0';
		return TRUE;
	}
	else	//匹配失败
	{
		return FALSE;
	}


	destroyList(&head);

}

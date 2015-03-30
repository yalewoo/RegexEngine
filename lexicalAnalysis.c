#include "header.h"

extern int charType(const char *);


/*
* 函数名称：charToInt
* 函数功能：将字符串读为整数 返回
* 输入参数：输入位数和字符串指针
* 返回值：整数
*/
static int charToInt(int i,char* pchar)
{
    int rate = 0;
    for (;pchar[i] <= '9' && pchar[i] >= '0';i++)
        rate = rate*10 + (pchar[i] - '0');
    return rate;
}

/*
* 函数名称：setRate
* 函数功能：将pchar指向的字符串（可能的重复次数）编码为3位数字 保存到rate数组。
* 输入参数：指向重复次数的指针 用于保存结构的rate数组
* 返回值：无
*/
/*重复次数和3位数字的编码关系如下
第一位0表示出现次数是rate[1]到rate[2]的闭区间
第一位1表示出现次数是 rate[1]次及以上
例如
	* 		编码为 1 0
	+ 		编码为 1 1
	? 		编码为 0 0 1
	{n}		编码为 0 n n
	{n,} 	编码为 1 n
	{n,m}	编码为 0 n m*/
static void setRate(char* pchar, int rate[3])
{
    int a=0,b=0,n=0;
    rate[2] = 0;
    if (pchar[n]=='\0')  rate[0] = 0,rate[1] = 1,rate[2] = 1;
    if (pchar[n]=='*' && pchar[1]=='\0')  rate[0] = 1,rate[1] = 0;  //*
    if (pchar[n]=='+' && pchar[1]=='\0')  rate[0] = 1,rate[1] = 1;  //+
    if (pchar[n]=='?' && pchar[1]=='\0')  rate[0] = 0,rate[1] = 0,rate[2] = 1;      //?
    if (pchar[n]=='{')
        {
            n++;
            for (;pchar[n] <= '9' && pchar[n] >= '0';)
            {
                n++,a++;
            }
                if (pchar[n]=='}') rate[0] = 0,rate[1]=charToInt(n-a,pchar),rate[2] = rate[1];
                if (pchar[n]==',')
                 {
                     n++;
                     if (pchar[n]=='}') rate[0] = 1,rate[1]=charToInt(n-a-1,pchar);
                     else
                     {
                         for (;pchar[n] <= '9' && pchar[n] >= '0';)
                         {
                             n++,b++;
                         }
                             if (pchar[n]=='}')  rate[0] = 0,rate[1]=charToInt(n-a-b-1,pchar),rate[2] = charToInt(n-b,pchar);
                     }
                 }

        }

}

/*
* 函数名称：saveToList
* 函数功能：将一个正则单元的所有信息保存到链表结点 供词法分析函数调用
* 输入参数：
* struct regex_list * head 链表头结点
* struct regex_list * p 保存当前正则单元的结点
* char * arr 正则表达式数组
* int low 该单元在数组中起始偏移量
* int high 该单元在数组中结尾偏移量
* int posi_start 将元字符和重复次数信息分开的偏移量
* int * group 用于处理分组的栈数组
* int top 栈数组的栈顶位置
* 返回值：返回保存这些单元正则的结点指针
*/
static struct regex_list * saveToList(struct regex_list * head, struct regex_list * p, char * arr, int low, int high, int posi_start, int * group, int top)
{
	struct regex_list * q;
	struct regex_list * p_to_find_bracket;	//用来寻找和右括号配对的左括号
	int i;
	if ((q = (struct regex_list *)malloc(sizeof(struct regex_list))) == NULL)
		exit(OVERFLOW);

	memset(q, 0, sizeof(struct regex_list));

	//构成双向链表
	q->next = p->next;
	p->next->pred = q;
	p->next = q;
	q->pred = p;

	//初始化
	q->branch_num = 0;
	for (i = 0; i < MAX_GROUP; ++i)	//
	{
		q->group[i] = 0;
	}
	q->is_posi = 0;
	q->is_back_ref = 0;
	q->pbranch[0] = q;
	q->matched_num = 0;
	//设置char unit[UNIT_LEN];	//保存单位正则字符
	strncpy(q->unit, arr + low, high - low + 1);
	q->unit[high - low + 1] = '\0';

	//设置int posi_start; //分割元字符和出现次数的下标
	q->posi_start = posi_start - low;

	//设置group
	for (i = 0; i <= top; ++i)
	{
		q->group[(group[i])] = 1;
	}

	//设置rate[3]记录重复次数
	setRate(q->unit + q->posi_start, q->rate);

	//设置是否是位置字符
	if (q->unit[0] == '^' || q->unit[0] == '$')
	{
		q->is_posi = 1;
	}
	if ((q->unit[0] == '\\') && ((q->unit[1] == 'b') || (q->unit[1] == 'B')))
	{
		q->is_posi = 1;
	}

	//设置是否是后向引用
	if (q->unit[0] == '\\' && ((q->unit[1] >= '1') && (q->unit[1] <= '9')))
	{
		q->is_back_ref = 1;
	}

	//设置group_id为所在的分组中的最大者 右括号另行处理
	q->group_id = group[top];

	//处理括号
	if (q->unit[0] == ')')
	{
		q->group_id = group[top + 1];

		//设置括号对之间互相指向
		p_to_find_bracket = head->next;
		while (p_to_find_bracket->unit[0] != '(' || p_to_find_bracket->group_id != q->group_id)
		{
			p_to_find_bracket = p_to_find_bracket->next;
		}
		//现在p_to_find_bracket指向配对的左括号
		p_to_find_bracket->pbracket = q;
		q->pbracket = p_to_find_bracket;

		//将左括号的pbranch最后一个元素指向右括号
		p_to_find_bracket->pbranch[p_to_find_bracket->branch_num + 1] = q;
	}

	//处理分支符号
	if (q->unit[0] == '|')
	{
		if (q->group_id == 0)
		{
			p_to_find_bracket = head;
			p_to_find_bracket->pbranch[p_to_find_bracket->branch_num + 2] = q->next;
		}
		else
		{
			p_to_find_bracket = head->next;

			while (p_to_find_bracket->unit[0] != '(' || p_to_find_bracket->group_id != q->group_id)
			{
				p_to_find_bracket = p_to_find_bracket->next;
			}
		}
		//现在p_to_find_bracket指向|所在分组的左括号
		++p_to_find_bracket->branch_num;
		p_to_find_bracket->pbranch[p_to_find_bracket->branch_num] = q;


	}



	return q;
}

/*
* 函数名称：lexicalAnalysis
* 函数功能：分词。将结果放进链表
* 输入参数：指向正则表达式字符串的指针 和 保存分词后信息的链表头结点
* 返回值：可能的错误码（不包括0） 正确返回1 。
*/
int lexicalAnalysis(char * arr, struct regex_list * head)
{
	int low = 0;
	int high;
	struct regex_list * p;
	//struct regex_list * q;
	int group[10] = { 0 };
	int top = 0;	//group的栈顶指针
	int group_id = 0;	//当前左括号出现次数
	int posi_start = 0;	//分割元字符和重复次数
	int tmp;
	p = head;
	//q = p;
	high = low;
	while (arr[high] != '\0')
	{
		tmp = charType(&arr[low]);
		switch (tmp)
		{
		case 15:	//low和high指向[abc]中的[
		{
			for (high = high + 2; arr[high] != ']'; ++high)
			{
				;
			}
			//现在high指向]
			--high;	//继续执行下去case 14中会将high增加1 因此这里先减去1
			//没有break
		}
		case 43:;	// \1 \2 \3 \4 \5 \6 \7 \8 \9
		case 14: 	//high指向\w \s \d \W \S \D中的\ 。

			high = high + 1;
			//没有break
		case 1:;
		case 11:	//处理匹配文本的元字符后面可能有的重复次数
		{
			posi_start = high + 1;
			if (arr[high + 1] != '\0' && charType(&arr[high + 1]) == 21)
			{
				high = high + 1;
			}
			else if (arr[high + 1] != '\0' && arr[high + 1] == '{')
			{
				for (high = high + 2; arr[high] != '}'; ++high)
				{
					;
				}
			}
			else
			{
				high = high;

			}
			break;
		}
		case 31:;
		case 12:	//^和$
		{
			posi_start = high + 1;
			high = low;
			break;
		}

		case 13:	//\b \B
		{
			posi_start = high + 2;
			high = low + 1;
			break;
		}
		case 41:	// (
		{
			//++group;
			++group_id;
			++top;
			group[top] = group_id;	//左括号进栈

			posi_start = high + 1;
			high = low;
			break;
		}
		case 42:	//)
		{
			//--group;
			--top;	//遇见右括号 出栈

			posi_start = high + 1;
			if (arr[high + 1] != '\0' && charType(&arr[high + 1]) == 21)
			{
				high = high + 1;
			}
			else if (arr[high + 1] != '\0' && arr[high + 1] == '{')
			{
				for (high = high + 2; arr[high] != '}'; ++high)
				{
					;
				}
			}
			else
			{
				;

			}
			break;
		}
		default:
		{
			break;
		}
		}
		if (top == -1)
        {
            break;
        }
		//这个时候区间[low, high]是一个匹配单元
		p = saveToList(head, p, arr, low, high, posi_start, group, top);
		low = high + 1;
		high = low;
	}
	if (top != 0)
    {
        return UNMATCHED_BRACKET;
    }
	return TRUE;
}



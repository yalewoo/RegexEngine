#include "header.h"

extern int charType(const char *);
extern BOOL match(const struct regex_list * p, const char * text, int i);


/*
* 函数名称：canGreedyBack
* 函数功能：判断当前结点不匹配时是否可以回退。 如果当前结点所在分组已经匹配过多次超过最低要求 不回退
* 输入参数：正则表达式结点
* 返回值：可以回退返回TRUE  否则返回FALSE
*/
static BOOL canGreedyBack(struct regex_list * p)
{
	struct regex_list * pclose_brace;
	struct regex_list * p_to_find_bracket;
	int back_group_id;
	int k;

	if (p->unit[0] == '(')
	{
		pclose_brace = p->pbracket;
		if (pclose_brace->next == NULL)	//尾结点
		{
			return FALSE;
		}
		back_group_id = p->group_id;
		--back_group_id;
	}
	else
	{
		pclose_brace = p;
		back_group_id = p->group_id;
	}
	for (k = back_group_id; k > 0; --k)
	{
		if (p->group[k] == 1)
		{
			p_to_find_bracket = pclose_brace->next;
			while (p_to_find_bracket->next != NULL && (p_to_find_bracket->unit[0] != ')' || p_to_find_bracket->group_id != k))
			{
				p_to_find_bracket = p_to_find_bracket->next;
			}
			if (p_to_find_bracket->matched_num >= p_to_find_bracket->rate[1])
			{
				return FALSE;	//不需要回退 所属括号重复次数已满足要求
			}
		}
	}


	return TRUE;
}

/*
* 函数名称：greedyBack
* 函数功能：进行贪婪回退
* 输入参数：当前结点指针的地址（会修改该指针） 当前文本匹配位置指针（会修改）
* 返回值：回退成功返回TRUE 否则返回FALSE
*/
static BOOL greedyBack(struct regex_list ** p, int * i)	//贪婪回退 会修改p和i为回退后下一个结点和文本位置
{
	struct regex_list * q;
	int ok = 1;
	int succ = 0;
	q = *p;
	q = q->pred;

	while (q->pred != NULL && q->unit[0] != '|')
	{
		ok = 1;
		succ = 0;
		if (q->unit[0] == ')')
		{
			if (q->matched_num > q->rate[1])
			{
				--q->matched_num;
				--q->pbracket->matched_num;
				*i = q->matched_char_posi[q->matched_num][0];
				*p = q->next;
				q = *p;
				break;
			}
			else if (q->pbracket->branch_num == 0)
			{
				q->low = 0;
				q->high = 0;
				q->matched_num = 0;
				q->pbracket->matched_num = 0;
				q = q->pred;
				continue;
			}
			else
			{

				*p = q->pbracket->pbranch[q->matched_char_posi[q->matched_num][1]]->pred;
				q->matched_num = 0;
				q->pbracket->matched_num = 0;
				q = *p;
				continue;
			}

			// else
			// {
			// 	ok = 0;
			// 	break;
			// }
		}
		else if (q->unit[0] == '|')
		{

		}
		else if (q->unit[0] == '(')
		{
			q = q->pred;
			continue;
		}
		else if (q->is_back_ref == 1)
		{
			if (q->matched_num > q->rate[1])
			{
				--q->matched_num;
				*i = q->matched_char_posi[q->matched_num][0];
				*p = q->next;
				q = *p;
				break;
			}
			else
			{
				q->low = 0;
				q->high = 0;
				q = q->pred;
				continue;
			}
		}
		else
		{
			if (q->high - q->low > q->rate[1])
			{
				--(q->high);
				*i = q->high;	//i回退
				*p = q->next;	//p接着下一个
				q = *p;
				succ = 1;
				break;
			}
			else
			{
				q->low = 0;
				q->high = 0;
				q = q->pred;
				continue;
			}
		}
	}
	if (succ == 1)
	{
		*p = q;
		return TRUE;
	}
	else if (q->pred == NULL || q->unit[0] == '|')	//回退失败
	{
		return FALSE;
	}
	else if (ok == 0)
	{
		return FALSE;
	}
	else	//回退成功
	{
		*p = q;
		return TRUE;
	}
}

/* 函数功能 进行NFA引擎扫描匹配
输入比较多
struct regex_list * const plow 一串正则单元的开始结点
struct regex_list * const phigh 一串正则单元的结束结点
const char * text 指向文本
const int start_char 文本的从哪个字符开始匹配
int * presult 指向一个整数 保存当前状态匹配到了文本中的字符的偏移量
该函数返回值为是否匹配*/
/*输出presult指向的int数值为匹配的[start, high)中的high
**输入为正则串 (plow, phigh)开区间 其中不含括号和分支
**返回值是是否匹配成功
*/
BOOL scanListWithCharPosi(struct regex_list * const plow, struct regex_list * const phigh, const char * text, const int start_char, int * presult)
{
	int i = start_char;
	int j;
	struct regex_list * p;
	const struct regex_list * q;
	struct regex_list * pclose_brace = NULL;	//用于指向右括号
	//int back_group_id;	//记录一个结点所属的上一层括号id
	struct regex_list * p_to_find_bracket;	//用来寻找一个结点所属的上一层括号的右括号
	int tmp;
	//	int k;	//用于累加计数
	int low_tmp;

	int back_ref_low;	//保存某个分组匹配的内容开头位置
	int back_ref_high;	//保存某个分组匹配的内容末尾位置

	p = plow;
	while (p != phigh->next && p != NULL)
	{
		if (p->unit[0] == '(')
		{
			for (tmp = 0; tmp <= p->branch_num; ++tmp)
			{
				if (scanListWithCharPosi(p->pbranch[tmp]->next, p->pbranch[tmp + 1]->pred, text, i, presult) == TRUE)
				{
					break;
				}
			}
			if (tmp <= p->branch_num)	//匹配成功
			{
				//k = 1;
				pclose_brace = p->pbracket;
				pclose_brace->matched_num = 1;
				pclose_brace->matched_char_posi[0][0] = i;
				pclose_brace->matched_char_posi[1][0] = *presult;
				pclose_brace->matched_char_posi[1][1] = tmp + 1;
				pclose_brace->low = i;
				pclose_brace->high = *presult;

				p->matched_num = 1;
				p->matched_char_posi[0][0] = i;
				p->matched_char_posi[1][0] = *presult;
				p->matched_char_posi[1][1] = tmp + 1;
				p->low = i;
				p->high = *presult;

				while (pclose_brace->rate[0] == 1 || ((pclose_brace->rate[0] == 0) && (pclose_brace->matched_num < pclose_brace->rate[2])))	//j<=rate[2]保证了匹配个数不超过要求的最大个数
				{
					for (tmp = 0; tmp <= p->branch_num; ++tmp)
					{
						low_tmp = p->pbranch[tmp + 1]->pred->low;
						if (scanListWithCharPosi(p->pbranch[tmp]->next, p->pbranch[tmp + 1]->pred, text, pclose_brace->matched_char_posi[pclose_brace->matched_num][0], presult) == TRUE)
						{
							if (*presult == pclose_brace->high)
							{
								p->pbranch[tmp + 1]->pred->low = low_tmp;
								continue;
							}
							else
							{
								break;
							}
						}
					}

					if (tmp <= p->branch_num)	//匹配成功
					{
						pclose_brace->matched_char_posi[++pclose_brace->matched_num][0] = *presult;
						pclose_brace->low = pclose_brace->matched_char_posi[pclose_brace->matched_num - 1][0];
						pclose_brace->high = pclose_brace->matched_char_posi[pclose_brace->matched_num][0];

						p->matched_char_posi[++p->matched_num][0] = *presult;
						p->low = p->matched_char_posi[p->matched_num - 1][0];
						p->high = p->matched_char_posi[p->matched_num][0];
					}
					else
					{
						break;
					}
				}
				if (pclose_brace->matched_num < pclose_brace->rate[1])	//小于最低要求数量
				{
					//括号重复次数小于括号要求数量
					if (canGreedyBack(p) == FALSE)
					{
						return FALSE;
					}
					else
					{
						//贪婪回退代码
						if (greedyBack(&p, &i) == FALSE)
						{
							return FALSE;
						}
						else
						{
							continue;
						}

					}


				}
				else	//匹配成功 匹配了pclose_brance->matched_num次
				{
					i = *presult;
					p = pclose_brace->next;
					continue;
				}

				/*
				p->pbracket->low = i;
				i = *presult;
				p->pbracket->high = i;
				p = p->pbracket->next;
				continue;*/
			}
			else if (p->pbracket->rate[1] == 0)	//如果可以匹配0次
			{
				p->pbracket->low = i;
				i = *presult;
				p->pbracket->high = i;
				p = p->pbracket->next;
				continue;
			}
			else	//匹配失败
			{
				//括号中的内容匹配失败
				if (canGreedyBack(p) == FALSE)
				{
					return FALSE;
				}
				else
				{
					//贪婪回退代码
					if (greedyBack(&p, &i) == FALSE)
					{
						return FALSE;
					}
					else
					{
						continue;
					}
				}
			}
		}
		else if (p->unit[0] == ')')		//只有贪婪回退会回到这里
		{

		    if (p->pbracket->next->unit[0] != '(')
            {
                p->low = p->pbracket->next->low;
            }
            else
            {
                p->low = p->pbracket->next->matched_char_posi[0][0];
            }


			p->high = p->pred->high;
			p = p->next;
			continue;
		}
		else if (p->unit[0] == '|')
		{
			p_to_find_bracket = p->next;
			while (p_to_find_bracket->next != NULL && (p_to_find_bracket->unit[0] != ')' || p_to_find_bracket->group_id != p->group_id))
			{
				p_to_find_bracket = p_to_find_bracket->next;
			}
			p_to_find_bracket->matched_char_posi[p_to_find_bracket->matched_num][0] = p->pred->high;
			p_to_find_bracket->pbracket->matched_char_posi[p_to_find_bracket->pbracket->matched_num][0] = p->pred->high;
			p = p_to_find_bracket->next;
			continue;
		}
		else
		{
			if (p->is_back_ref == 0)	//不是后项引用
			{
				if (match(p, text, i) == TRUE)
				{
					p->low = i;
					if (p->is_posi == 1)
					{
						p->high = i;
						p = p->next;
					}
					else
					{
						j = 1;

						//下面这个循环处理 可能重复多次的表达式 即后面跟了* + ? {n}等
						while (p->rate[0] == 1 || ((p->rate[0] == 0) && (j <= p->rate[2])))	//j<=rate[2]保证了匹配个数不超过要求的最大个数
						{
							if (match(p, text, i + j) == FALSE)
							{
								p->high = i + j;
								++j;	//这里j自增1是为了while循环结束后j-- 让不匹配退出和达到最大退出 两种情况下的j都变为匹配个数
								break;
							}
							++j;	//匹配的情况下j不断递增 一直到不匹配或达到最大次数
						}
						--j;	//这时候前闭后开区间[i, i+j)范围的文本匹配。共j个。
						p->high = i + j;
						if (j < p->rate[1])	//小于最低要求个数 该单元与该字符位置不匹配
						{
							//字符单元匹配次数小于要求
							if (canGreedyBack(p) == FALSE)
							{
								return FALSE;
							}
							else
							{
								//贪婪回退代码
								if (greedyBack(&p, &i) == FALSE)
								{
									return FALSE;
								}
								else
								{
									continue;
								}

							}
						}
						else	//匹配成功的情况 匹配了j个字符
						{
							i = i + j;	//i指向下一个需要检测的字符
							p = p->next;	//p指向下一个规则
							continue;
						}
					}
				}
				else if (p->rate[1] == 0)	//相当于该位置匹配0次 相当于0宽度
				{
					p->low = i;
					p->high = i;	//匹配零次 [i,i)空区间
					p = p->next;	//下一个规则
					continue;		//i不变 继续将该字符和下一个规则匹配
				}
				else	//不匹配 和多字符匹配失败的代码相同
				{
					//字符单元匹配失败
					if (canGreedyBack(p) == FALSE)
					{
						return FALSE;
					}
					else
					{
						//贪婪回退代码
						if (greedyBack(&p, &i) == FALSE)
						{
							return FALSE;
						}
						else
						{
							continue;
						}
					}


				}
			}
			else	//是后项引用 比如\2
			{
				for (q = plow; q->pred != NULL; q = q->pred)
				{
					;
				}	//现在q指向头结点head
				while (q->unit[0] != ')' || q->group_id != (p->unit[1] - '0'))
				{
					q = q->next;
				}	//现在q指向对应组的右括号
				back_ref_low = q->low;
				back_ref_high = q->high;	//这是该组最后一次匹配的内容区间

				if (strncmp(text + back_ref_low, text + i, back_ref_high - back_ref_low) == 0)	//匹配成功
				{
					p->matched_num = 1;
					p->matched_char_posi[0][0] = i;
					p->matched_char_posi[1][0] = i + back_ref_high - back_ref_low;
					p->low = i;
					p->high = p->matched_char_posi[1][0];


					i = i + back_ref_high - back_ref_low;


					while (p->rate[0] == 1 || ((p->rate[0] == 0) && (p->matched_num < p->rate[2])))	//保证了匹配个数不超过要求的最大个数
					{
						if (strncmp(text + back_ref_low, text + i, back_ref_high - back_ref_low) == 0)
						{
							++p->matched_num;
							p->matched_char_posi[p->matched_num][0] = p->matched_char_posi[p->matched_num - 1][0] + back_ref_high - back_ref_low;
							p->low = p->matched_char_posi[p->matched_num - 1][0];
							p->high = p->matched_char_posi[p->matched_num][0];
							i = i + back_ref_high - back_ref_low;
						}
						else
						{
							break;
						}
					}
					if (p->matched_num < p->rate[1])
					{
						if (canGreedyBack(p) == FALSE)
						{
							return FALSE;
						}
						else
						{
							//贪婪回退代码
							if (greedyBack(&p, &i) == FALSE)
							{
								return FALSE;
							}
							else
							{
								continue;
							}
						}
					}
					else	//匹配成功
					{
						p = p->next;
						continue;
					}
				}
				else	//匹配失败
				{
					//贪婪回退。寻找p结点之前的匹配好的 有没有匹配多个字符 且可以减小一个字符的
					if (canGreedyBack(p) == FALSE)
					{
						return FALSE;
					}
					else
					{
						//贪婪回退代码
						if (greedyBack(&p, &i) == FALSE)
						{
							return FALSE;
						}
						else
						{
							continue;
						}
					}
				}

			}
		}	//end 不是括号和分支符号

	}
	*presult = i;
	return TRUE;

}


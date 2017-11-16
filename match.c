#include "header.h"

extern int charType(const char *);


//判断表示位置的长度为1的（^ $）是否和文本位置匹配
static BOOL matchPosi1(const char* pchar, const char* text, int i)
{
	switch (pchar[0])
	{
	case '^':   if (i == 0 || text[i - 1] == '\n')   return TRUE; else    return  FALSE;
	case '$':   if (text[i] == '\0' || text[i] == '\n')   return TRUE; else    return  FALSE;
	default: return FALSE;
	}
}



//1位的字符是否和一个字符匹配 处理所有普通字符和元字符.
static BOOL match1(const char * pchar, const char ch)
{
	if (pchar[0] == '.')
	{
		if (ch == '\n' || ch == '\0')
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	else
	{
		if (ch == pchar[0])
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
}

//2位的元字符是否匹配 \w 等
static BOOL match2(const char * pchar, const char ch)
{
	if (pchar[0] == '\\')
	{
		switch (pchar[1])
		{
		case 'w':   if ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))   return TRUE; else    return  FALSE;
		case 's':   if ((ch == ' ') || (ch == '\t') || (ch == '\n'))   return TRUE; else    return  FALSE;
		case 'd':   if (ch >= '0' && ch <= '9') return  TRUE;    else    return  FALSE;
		case 'W':   if ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))   return FALSE; else    return  TRUE;
		case 'S':   if ((ch == ' ') || (ch == '\t') || (ch == '\n'))   return FALSE; else    return  TRUE;
		case 'D':   if (ch >= '0' && ch <= '9') return  FALSE;   else    return  TRUE;
		default: return pchar[1] == ch ? TRUE : FALSE;
		}
	}
	return FALSE;
}

//判断表示位置的长度为2的（\b \B）是否和文本位置匹配
static BOOL matchPosi2(const char* pchar, const char* text, int i)
{
	if (pchar[0] == '\\')
	{
		switch (pchar[1])
		{
			case 'b':   if ( (i == 0 && match2("\\w",text[i]))
                   || ( match2("\\W",text[i-1]) && match2("\\w",text[i]) )
                   || ( match2("\\W",text[i+1]) && match2("\\w",text[i]) )
                   || ( text[i + 1] == '\0' && match2("\\w",text[i])) )   return TRUE; else    return  FALSE;
		case 'B':   if ( (i == 0 && match2("\\w",text[i]))
                   || ( match2("\\W",text[i-1]) && match2("\\w",text[i]) )
                   || ( match2("\\W",text[i+1]) && match2("\\w",text[i]) )
                   || ( text[i + 1] == '\0' && match2("\\w",text[i])) )   return FALSE; else    return  TRUE;
		default: return FALSE;
		}
	}
	return FALSE;
}

//表示范围的[x]是否和字符匹配
static BOOL matchRange(const char *pchar, const int len, char ch)
{
	char dic[UNIT_LEN] = { 0 };
	int k = 0;
	for (int i = 1; i < len-1; ++i)
	{
		if ((i == 1 || i+1 == len-1) && pchar[i] == '-')
			dic[k++] = pchar[i];
		else if (pchar[i] != '-')
			dic[k++] = pchar[i];
		else
		{
			++i;
			for (char c = pchar[i - 2] + 1; c <= pchar[i]; ++c)
				dic[k++] = c;
		}
	}


	for(int i = 0, k = strlen(dic); i < k; ++i)
	{
		if(ch == dic[i])
			return TRUE;

	}
	return FALSE;
}

/*
* 函数名称：match
* 函数功能：判断一个结点是否和文本位置匹配
* 输入参数：正则表达式结点 文本指针 文本偏移量
* 返回值：匹配返回TRUE  不匹配返回FALSE
*/
BOOL match(const struct regex_list * p, const char * text, int i)
{
	switch (charType(p->unit))
	{
	case 1:;
	case 11:
	{
		return match1(p->unit, text[i]);
	}
	case 14:
	{
		return match2(p->unit, text[i]);
	}
	case 15:
	{
		if (p->unit[1] != '^')
		{
			return matchRange(p->unit, p->posi_start, text[i]);
		}
		else
		{
			return (!matchRange(p->unit, p->posi_start, text[i]));
		}
	}
	case 12:
	{
		return matchPosi1(p->unit, text, i);
	}
	case 13:
	{
		return matchPosi2(p->unit, text, i);
	}
	default:
	{
		break;
	}
	}
	return FALSE;
}

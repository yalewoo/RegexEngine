#include "header.h"

#include "regexSearch.h"

#include <time.h>


BOOL saveToFile(char * result, int index);  //按照题目要求输出到文件


int main(int argc, char * argv[])
{
    clock_t begin, duration;

    int error_index = 1;
    char result[MAX_RESULT_LENGTH]; //存放每次匹配到的文本
	//FILE * fp_regex;    //打开regex.txt的文件指针
	FILE * fp_text;     //打开textN.txt的文件指针
	char filename[FILE_NAME_MAX_LENGTH];    //存放文件名字
	char regex_text[BUFFER_LEN];    //存放正则表达式 每次存放regex.txt的一行
	int ch;     //用于读取文件内容
	char * text;    //用于指向文本字符串 textN.txt的全部内容
	long char_num = 0;  //用于统计extN.txt长度
	//int index = 1;  //用于计数第几个文件 依次打开text1.txt text2.txt ..等
	int i = 0;  //用于读取文件内容的计数变量

    begin = clock();

	/*//打开regex.txt
	if ((fp_regex = fopen("regex.txt", "r")) == NULL)
	{
		printf("can't find the file\n");
		printf("press enter..\n");
		getchar();
		return 0;
	}
	for (index = 1; (fgets(regex_text, BUFFER_LEN, fp_regex)) != NULL; ++index)
	{
		for (i = 0; regex_text[i] != '\0'; ++i)
		{
			;
		}
		if (regex_text[i - 1] == '\n')
		{
			regex_text[i - 1] = '\0';
		}
		//现在regex_text存放一行正则表达式 以\0结束

        //打开对应的textN.txt
		sprintf(filename, "text%d.txt", index);*/
    if (argc != 1)
    {
        strcpy(filename, argv[1]);
    }
    else
    {
        printf("input text filename:\n");
        scanf("%s", filename);
    }
    if ((fp_text = fopen(filename, "r")) == NULL)
    {
        printf("can not open %s\n", filename);
        printf("press enter..\n");
        getchar();
        return 0;
    }
    //统计textN.txt长度 分配相应大小内存
    for (char_num = 0; (ch = fgetc(fp_text)) != EOF; ++char_num)
    {
        ;
    }
    fseek(fp_text, 0, SEEK_SET);
    text = (char *)malloc((char_num + 1)*sizeof(char));

    for (i = 0; (ch = fgetc(fp_text)) != EOF; ++i)
    {
        text[i] = ch;
    }
    //文本读入text 关闭textN.txt
    fclose(fp_text);
    text[i] = '\0';

    while (TRUE)
    {
        printf("# to exit. please input regex: ");
        scanf("%s", regex_text);


        //intf("regex.txt line%d:%s\n", index, regex_text);
        //intf("%s:%s\n", filename, text);

        //regexSearch函数 将regex_text的规则 在文本text中找到首先匹配的部分 输出到result缓冲区
        if ((error_index = regexSearch(regex_text, text, result)) == TRUE)
        {
            //结果保存到文件
            /*if (saveToFile(result, index) == FALSE)
            {
                printf("can not create file output%d.txt", index);

            }
            else
            {*/
                printf("result: %s\n\n", result);
            //}
        }
        else
        {

            if (error_index == 0)
            {
                result[0] = '\0';
                /*if (saveToFile(result, index) == FALSE)
                {
                   // printf("can not create file output%d.txt", index);

                }
                else
                {*/
                    printf("result: no result!\n\n");
                //}
            }
            else
            {
                switch (error_index)
                {
                case UNMATCHED_BRACKET:
                    {
                        printf("wrong regex: no matched brackets!\n\n");
                        break;
                    }
                default:
                    {
                        printf("wrong regex\n\n");
                        break;
                    }
                }
            }


        }
    }

    //释放本次保存textN.txt内容的缓冲区
    free(text);


    //程序结束
    duration = clock() - begin;
    printf("run time %ld ms \n", duration*1000/CLOCKS_PER_SEC);

	printf("press enter to exit..\n");
	getchar();

	return 0;
}


BOOL saveToFile(char * result, int index)
{
	char filename[FILE_NAME_MAX_LENGTH];
	int i;
	FILE * fp;
	sprintf(filename, "output%d.txt", index);
	if ((fp = fopen(filename, "w")) == NULL)
		return FALSE;
	for (i = 0; result[i] != '\0'; ++i)
	{
		putc(result[i], fp);
	}
	fclose(fp);
	return TRUE;
}



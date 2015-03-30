###1、支持所有元字符 . \w \s \d \W \S \D ^ $ \b \B [x] [^x]
例如： [a-f4-7]\w\d\s\W\D\S 可以匹配6d3（换行）#!
		a$\s \babc匹配a（换行）（空格）abc会输出a（换行）（空格）abc

###2、支持任意元字符和重复量词（* + ? {n} {n,m}{n,}）组合 
例如： [a-d]*\d{3}-\d{7}匹配sdfabcd321-123456789得到abcd321-1234567

###3、	支持元字符和重复量词的一重贪婪回退
\d+.+12 匹配adsfs123abc3abc12ashdfi会得到123abc3abc12

###4、	支持分支符号
a+|c 匹配aaaad会得到aaaa 匹配dc会得到c

###5、	分组和后向引用
分组和后项引用： (\d+)\1匹配781541544得到154154

###6、	分组和分支的结合
(ab|cd)(ef|gh)\1\2匹配111abghabghijo得到abghabgh

###7、	分组、分支和重复量词的结合
 (ab|cd){2,3}\1匹配ergafabcdabcdabrg得到abcdab
	   (a|p)*apapp匹配apaapappppappapapappaapp得到apaapappppappapapapp
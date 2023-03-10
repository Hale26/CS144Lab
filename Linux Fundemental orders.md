## Linux基本命令

#### 1. cp命令

该命令的功能是将给出的文件或目录拷贝到另一文件或目录中，功能十分强大。

语法： `cp [选项] 源文件或目录 目标文件或目录`

 

#### 2. mv命令

用户可以使用mv命令来为文件或目录改名或将文件由一个目录移入另一个目录中。

语法：`mv [选项] 源文件或目录 目标文件或目录`

 

#### 3. rm命令

用户可以用rm命令删除不需要的文件或者目录。

语法：`rm [选项] 文件`

**如果没有使用- r选项，则rm不会删除目录。**

 

#### 4. mkdir和rmdir

语法：`mkdir [选项] dir-name`

语法：`rmdir [选项] dir-name`

 

#### 5. cd 命令

功能：改变工作目录。

语法：`cd [directory]`

 

#### 6. ls 命令

ls是英文单词list的简写，其功能为列出目录的内容。该命令类似于DOS下的dir命令。

语法：ls [选项][目录或是文件]

 

#### 7. su命令

这个命令非常重要。它可以让一个普通用户拥有超级用户或其他用户的权限，也可以让超级用户以普通用户的身份做一些事情。普通用户使用这个命令时必须有超级用户或其他用户的口令。

该命令的一般形式为： su [选项][使用者帐号]

 

#### 8. ps命令

显示系统中执行的程序。

语法：`ps [选项]`

 

#### 9. kill命令

删除执行中的程序

语法：`kill [选项] PID`

 

#### 10. grep命令

搜寻输出的特定文字

语法 `grep 字符串`

例：

ps aux | grep matlab

kill PID

 

#### 11. echo命令

echo命令的功能是在显示器上显示一段文字，一般起到一个提示的作用。

该命令的一般格式为： `echo [ -n ] 字符串`

 

#### 12. clear命令

clear命令的功能是清除屏幕上的信息，它类似于DOS中的 cls命令。清屏后，提示符移动到屏幕左上角。
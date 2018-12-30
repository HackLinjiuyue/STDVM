# STDVM
A general VM which use byte code to run
一个通用的字节码解释器。
使用方式(how to use)：

终端(shell) ./stdvmx.x 脚本路径(script path)

字节码由编译器产生。编译器将可以编译多个语言。
byte codes are built by general compiler which can compile many languages.

字节码(byte codes)：
关键字 std

数据类型：int float bool map list dict NULL NaN

语句指令
d(g) 定义变量，加g为全局
so xxx 标准流输出
si 标准流输入->std变量内
lp 后缀表达串 循环
edl 结束循环
if 后缀表达串 判断
edi 结束判断
es 否则
cl 参数,参数,…… 调用函数
fi xxx 文件输入(字符串路径)->std
fo xxx 文件输出(字符串路径)
brk 跳出循环
rt 返回函数值

运算
规定：所有在运算中的数都是浮点数，变量可为整数
@为特殊指令组\[后缀 含义]:
t 类型
ti 值转换整数
ts 转换为字符串
td 转换为浮点数
i 向下取整
p 修改为变量引用
@ 调用函数
本身为字典/列表成员访问符

. 取字典/列表引用符

+-*/% ** 基本算数运算

= 赋值

: 构造映射

[]构造列表

{}构造字典

|| && > < == >= <= != 基本逻辑运算


对象构造：统一使用dict
函数构造：
d(g) funcname { "code" xx : "args" \[ xx xx …… ] : } =

类构造：
d(g) classname { "method1" { "code" xx : "args" xx : } : …… }

# BASIC编译器	

### 测试方法

在 Linux 环境下：

```
cd test
python3 TestScript.py
```

### 测试情况

| 测试点名称    | 通过情况 | 返回值   | 时钟周期 |
| ------------- | -------- | -------- | -------- |
| array_1.txt   | PASS     | 28       | 307      |
| array_2.txt   | PASS     | 184      | 890      |
| array_3.txt   | PASS     | 8        | 3149     |
| array_4.txt   | PASS     | 126      | 2039     |
| array_5.txt   | PASS     | 25       | 110      |
|               |          |          |          |
| basic_1.txt   | PASS     | 0        | 9        |
| basic_2.txt   | PASS     | 5        | 11       |
| basic_3.txt   | PASS     | 4        | 13       |
| basic_4.txt   | PASS     | 3        | 15       |
| basic_5.txt   | PASS     | 4        | 15       |
| basic_6.txt   | PASS     | 5        | 22       |
| basic_7.txt   | PASS     | 3        | 23       |
| basic_8.txt   | PASS     | 10       | 81       |
| basic_9.txt   | PASS     | 6        | 115      |
| basic_10.txt  | PASS     | 30       | 15       |
|               |          |          |          |
| control_1.txt | FAIL     | 0        | 超时     |
| control_2.txt | PASS     | 6        | 19       |
| control_3.txt | PASS     | 0        | 83       |
| control_4.txt | PASS     | 16       | 60       |
| control_5.txt | PASS     | 10       | 1457     |
|               |          |          |          |
| op_1.txt      | PASS     | 67       | 18       |
| op_2.txt      | PASS     | 3        | 514      |
| op_3.txt      | PASS     | 53       | 33       |
| op_4.txt      | PASS     | 1944     | 24       |
| op_5.txt      | PASS     | 19       | 191      |
| op_6.txt      | PASS     | 0        | 31       |
| op_7.txt      | PASS     | 4        | 62       |
| op_8.txt      | PASS     | 1        | 24       |
| op_9.txt      | PASS     | 4        | 40       |
| op_10.txt     | PASS     | 12       | 35       |
|               |          |          |          |
| sema_1.txt    | PASS     | 编译错误 | --       |
| sema_2.txt    | PASS     | 4        | 27       |
| sema_3.txt    | PASS     | 0        | 16       |
| sema_4.txt    | PASS     | 编译错误 | --       |
| sema_5.txt    | PASS     | 编译错误 | --       |
| sema_6.txt    | PASS     | 编译错误 | --       |
| sema_7.txt    | PASS     | 编译错误 | --       |
| sema_8.txt    | FAIL     | 0        | 97       |
| sema_9.txt    | PASS     | 编译错误 | --       |
| sema_10.txt   | PASS     | 编译错误 | --       |
| sema_11.txt   | PASS     | 1        | 16       |
| sema_12.txt   | PASS     | 5        | 15       |
| sema_13.txt   | PASS     | 编译错误 | --       |
| sema_14.txt   | PASS     | 编译错误 | --       |
| sema_15.txt   | PASS     | 编译错误 | --       |
| sema_16.txt   | PASS     | 编译错误 | --       |
| sema_17.txt   | PASS     | 0        | 9        |
| sema_18.txt   | PASS     | 编译错误 | --       |
| sema_19.txt   | PASS     | 编译错误 | --       |
| sema_20.txt   | PASS     | 编译错误 | --       |

### 实现细节

以下是编译器的中间编译文件：

- 初始代码：

```
1 REM array_5.txt
2 REM input: 6 2  3
3 REM return value: 25
4 INPUT N
5 INPUT i, j
6 LET arr =  INT[N]
7 LET arr[i / (j + 2 / 5 - i * 3)+ (j - (i  * j)/j)] = i +  j *  3 - i * j
8 REM arr[1] = 5
9 LET arr[i * j - i] = i * 2 + j  * 3
10 REM arr[4] = 13
14 LET arr[2] = 7
16 EXIT arr[i] + arr[i * 2] + arr[j - i]
```

- Token 形式

```
[NUM, 1][LINE_END, 1]
[NUM, 2][LINE_END, 2]
[NUM, 3][LINE_END, 3]
[NUM, 4][INPUT][ID, N][LINE_END, 4]
[NUM, 5][INPUT][ID, i][COMMA][ID, j][LINE_END, 5]
[NUM, 6][LET][ID, arr][ASSIGN][INT][LBrackets][ID, N][RBrackets][LINE_END, 6]
[NUM, 7][LET][ID, arr][LBrackets][ID, i][DIV][LParentheses][ID, j][ADD][NUM, 2][DIV][NUM, 5][SUB][ID, i][MUL][NUM, 3][RParentheses][ADD][LParentheses][ID, j][SUB][LParentheses][ID, i][MUL][ID, j][RParentheses][DIV][ID, j][RParentheses][RBrackets][ASSIGN][ID, i][ADD][ID, j][MUL][NUM, 3][SUB][ID, i][MUL][ID, j][LINE_END, 7]
[NUM, 8][LINE_END, 8]
[NUM, 9][LET][ID, arr][LBrackets][ID, i][MUL][ID, j][SUB][ID, i][RBrackets][ASSIGN][ID, i][MUL][NUM, 2][ADD][ID, j][MUL][NUM, 3][LINE_END, 9]
[NUM, 10][LINE_END, 10]
[NUM, 14][LET][ID, arr][LBrackets][NUM, 2][RBrackets][ASSIGN][NUM, 7][LINE_END, 11]
[NUM, 16][EXIT][ID, arr][LBrackets][ID, i][RBrackets][ADD][ID, arr][LBrackets][ID, i][MUL][NUM, 2][RBrackets][ADD][ID, arr][LBrackets][ID, j][SUB][ID, i][RBrackets][LINE_END, 12]
```

- AST 树

```
- Program: 
	4 - InputStmt: 
		- Identifier: N
	5 - InputStmt: 
		- Identifier: i
		- Identifier: j
	6 - LetStmt: 
		- Identifier: arr
		- IntExpr: 
			- Identifier: N
	7 - LetStmt: 
		- ArrayExpr: arr
			- Binary: +
				- Binary: /
					- Identifier: i
					- Binary: -
						- Binary: +
							- Identifier: j
							- Binary: /
								- Number: 2
								- Number: 5
						- Binary: *
							- Identifier: i
							- Number: 3
				- Binary: -
					- Identifier: j
					- Binary: /
						- Binary: *
							- Identifier: i
							- Identifier: j
						- Identifier: j
		- Binary: -
			- Binary: +
				- Identifier: i
				- Binary: *
					- Identifier: j
					- Number: 3
			- Binary: *
				- Identifier: i
				- Identifier: j
	9 - LetStmt: 
		- ArrayExpr: arr
			- Binary: -
				- Binary: *
					- Identifier: i
					- Identifier: j
				- Identifier: i
		- Binary: +
			- Binary: *
				- Identifier: i
				- Number: 2
			- Binary: *
				- Identifier: j
				- Number: 3
	14 - LetStmt: 
		- ArrayExpr: arr
			- Number: 2
		- Number: 7
	16 - ExitStmt: 
		- Binary: +
			- Binary: +
				- ArrayExpr: arr
					- Identifier: i
				- ArrayExpr: arr
					- Binary: *
						- Identifier: i
						- Number: 2
			- ArrayExpr: arr
				- Binary: -
					- Identifier: j
					- Identifier: i
```

- 数组，变量和常量

```
= 2 2 | 
= 3 3 | 
= 4 4 | 
= 5 5 | 
= 7 7 | 
N 0 | 
arr 1 | -1 
i 0 | 
j 0 | 
```

- 中间形式的代码 - 四元式

```
 4 
INPUT	---	---	[N]	
----------------------
 5 
INPUT	---	---	[i]	
INPUT	---	---	[j]	
----------------------
 6 
ADD	{s0}	 0 	{1}	
ADD	[N]	 0 	{2}	
STORE	{s0}	{2}	 0 	
ADD	{s0}	 4 	{s0}	
MUL	 4 	{2}	{2}	
ADD	{s0}	{2}	{s0}	
LET	{1}	---	[arr]	
----------------------
 7 
MUL	[i]	[j]	{7}	
DIV	{7}	[j]	{6}	
SUB	[j]	{6}	{5}	
MUL	[i]	 3 	{10}	
DIV	 2 	 5 	{12}	
ADD	[j]	{12}	{11}	
SUB	{11}	{10}	{9}	
DIV	[i]	{9}	{8}	
ADD	{8}	{5}	{4}	
ADD	{4}	 0 	{3}	
ADD	{3}	 1 	{13}	
MUL	{13}	 4 	{13}	
MUL	[i]	[j]	{15}	
MUL	[j]	 3 	{17}	
ADD	[i]	{17}	{16}	
SUB	{16}	{15}	{14}	
LET	{14}	---	[arr]{13}	
----------------------
 9 
MUL	[i]	[j]	{20}	
SUB	{20}	[i]	{19}	
ADD	{19}	 0 	{18}	
ADD	{18}	 1 	{21}	
MUL	{21}	 4 	{21}	
MUL	[j]	 3 	{23}	
MUL	[i]	 2 	{24}	
ADD	{24}	{23}	{22}	
LET	{22}	---	[arr]{21}	
----------------------
 14 
ADD	 2 	 0 	{25}	
ADD	{25}	 1 	{26}	
MUL	{26}	 4 	{26}	
LET	 7 	---	[arr]{26}	
----------------------
 16 
SUB	[j]	[i]	{29}	
ADD	{29}	 0 	{28}	
ADD	{28}	 1 	{30}	
MUL	{30}	 4 	{30}	
MUL	[i]	 2 	{33}	
ADD	{33}	 0 	{32}	
ADD	{32}	 1 	{34}	
MUL	{34}	 4 	{34}	
ADD	[i]	 0 	{35}	
ADD	{35}	 1 	{36}	
MUL	{36}	 4 	{36}	
ADD	[arr]{36}	[arr]{34}	{31}	
ADD	{31}	[arr]{30}	{27}	
EXIT	{27}	---	---	
```

- 寄存器分配

```
[5] 
[5] [6] 
[5] [6] [7] 
new regID = 11
[6] [7] [11] [5] 
new regID = 12
[6] [7] [11] [5] [12] 
[6] [7] [11] [5] [12] [13] 
[5] [14] [6] [7] 
new regID = 11
[5] [14] [6] [11] [7] 
new regID = 12
[5] [14] [6] [11] [12] [7] 
new regID = 13
[5] [14] [11] [12] [7] [13] [6] 
[5] [14] [11] [12] [7] [13] [6] [15] 
new regID = 16
[5] [14] [11] [12] [7] [13] [6] [15] [16] 
[5] [14] [11] [12] [7] [13] [6] [15] [16] [17] 
[5] [14] [11] [12] [7] [13] [6] [15] [16] [17] [18] 
new regID = 19
[5] [14] [11] [12] [13] [6] [15] [16] [17] [18] [19] [7] 
new regID = 20
[5] [14] [11] [12] [13] [6] [15] [16] [17] [18] [19] [7] [20] 
new regID = 21
[5] [14] [11] [12] [13] [15] [16] [17] [18] [19] [7] [20] [21] [6] 
new regID = 22
[5] [14] [11] [12] [13] [15] [16] [17] [18] [19] [7] [20] [21] [6] [22] 
new regID = 23
[5] [14] [11] [12] [13] [15] [16] [17] [18] [19] [7] [20] [21] [6] [22] [23] 
new regID = 24
[5] [14] [11] [12] [13] [15] [16] [17] [18] [19] [7] [20] [21] [6] [22] [23] [24] 
new regID = 25
[5] [14] [11] [12] [13] [15] [16] [17] [18] [19] [7] [20] [21] [6] [22] [23] [24] [25] 
[5] [14] [11] [12] [13] [15] [16] [17] [18] [19] [20] [21] [22] [23] [24] [25] [26] [6] [7] 
new regID = 27
[5] [14] [11] [12] [13] [16] [17] [18] [19] [20] [21] [22] [23] [24] [25] [26] [6] [27] [7] [15] 
new regID = 28
[5] [14] [11] [12] [13] [16] [17] [18] [19] [20] [21] [22] [23] [24] [25] [26] [27] [7] [15] [28] [6] 
new regID = 29
[5] [14] [11] [12] [13] [16] [17] [18] [19] [20] [21] [22] [23] [24] [25] [26] [27] [7] [15] [28] [6] [29] 
new regID = 30
[5] [11] [12] [13] [16] [17] [18] [19] [20] [21] [22] [23] [24] [25] [26] [27] [7] [15] [28] [6] [29] [30] [14] 
[5] [14] [6] [7] 
new regID = 11
[5] [14] [7] [11] [6] 
new regID = 12
[5] [14] [7] [11] [6] [12] 
new regID = 13
[5] [14] [7] [11] [6] [12] [13] 
new regID = 15
[5] [14] [7] [11] [6] [12] [13] [15] 
[5] [14] [11] [6] [12] [13] [15] [16] [7] 
[5] [14] [11] [6] [12] [13] [15] [16] [7] [17] 
new regID = 18
[5] [14] [11] [12] [13] [15] [16] [7] [17] [18] [6] 
[5] [14] [11] [12] [13] [15] [16] [7] [17] [18] [6] [19] 
new regID = 20
[5] [14] [11] [12] [13] [15] [16] [7] [17] [18] [6] [19] [20] 
new regID = 21
[5] [11] [12] [13] [15] [16] [7] [17] [18] [6] [19] [20] [21] [14] 
[5] [7] [6] [14] 
[5] [7] [6] [14] [11] 
new regID = 12
[5] [7] [6] [14] [11] [12] 
new regID = 13
[5] [7] [6] [14] [11] [12] [13] 
[5] [7] [6] [14] [11] [12] [13] [15] 
[5] [7] [6] [11] [12] [13] [15] [16] [14] 
[5] [14] [7] [6] 
new regID = 11
[5] [14] [7] [6] [11] 
new regID = 12
[5] [14] [7] [6] [11] [12] 
new regID = 13
[5] [14] [7] [6] [11] [12] [13] 
[5] [14] [7] [11] [12] [13] [15] [6] 
[5] [14] [7] [11] [12] [13] [15] [6] [16] 
new regID = 17
[5] [14] [7] [11] [12] [13] [15] [6] [16] [17] 
new regID = 18
[5] [14] [7] [11] [12] [13] [15] [6] [16] [17] [18] 
new regID = 19
[5] [14] [7] [11] [12] [13] [16] [17] [18] [19] [15] [6] 
new regID = 20
[5] [14] [7] [11] [12] [13] [16] [17] [18] [19] [15] [6] [20] 
new regID = 21
[5] [7] [11] [12] [13] [16] [17] [18] [19] [6] [20] [21] [15] [14] 
[5] [7] [11] [12] [13] [16] [17] [18] [19] [6] [20] [21] [15] [22] [14] 
[5] [7] [11] [12] [13] [16] [17] [18] [19] [6] [20] [21] [15] [22] [14] [23] 
new regID = 24
[5] [7] [11] [12] [13] [16] [17] [18] [19] [6] [20] [21] [15] [24] [14] 
[5] [7] [11] [12] [13] [16] [17] [18] [19] [6] [20] [21] [15] [24] [14] [22] 
new regID = 23
```

- .dump 文件

```
0000 LUI  	rd_3	rs1_0	rs2_0	imm_65536	000101B7 
0004 LUI  	rd_2	rs1_0	rs2_0	imm_131072	00020137 
0008 ADDI 	rd_9	rs1_2	rs2_0	imm_80	05010493 
000C ADDI 	rd_8	rs1_9	rs2_0	imm_64	04048413 
0010 LW   	rd_5	rs1_2	rs2_0	imm_0	00012283 
0014 INPUT	rd_5	rs1_0	rs2_0	imm_0	000002FF 
0018 LW   	rd_6	rs1_2	rs2_0	imm_8	00812303 
001C INPUT	rd_6	rs1_0	rs2_0	imm_0	0000037F 
0020 LW   	rd_7	rs1_2	rs2_0	imm_12	00C12383 
0024 INPUT	rd_7	rs1_0	rs2_0	imm_0	000003FF 
0028 ADD  	rd_11	rs1_8	rs2_0	imm_0	000405B3 
002C ADD  	rd_12	rs1_5	rs2_0	imm_0	00028633 
0030 SW   	rd_0	rs1_8	rs2_12	imm_0	00C42023 
0034 LW   	rd_13	rs1_3	rs2_0	imm_8	0081A683 
0038 ADD  	rd_8	rs1_8	rs2_13	imm_0	00D40433 
003C MUL  	rd_12	rs1_13	rs2_12	imm_0	02C68633 
0040 ADD  	rd_8	rs1_8	rs2_12	imm_0	00C40433 
0044 LW   	rd_14	rs1_2	rs2_0	imm_4	00412703 
0048 ADD  	rd_14	rs1_11	rs2_0	imm_0	00058733 
004C MUL  	rd_11	rs1_6	rs2_7	imm_0	027305B3 
0050 DIV  	rd_12	rs1_11	rs2_7	imm_0	0275C633 
0054 SUB  	rd_13	rs1_7	rs2_12	imm_0	40C386B3 
0058 LW   	rd_15	rs1_3	rs2_0	imm_4	0041A783 
005C MUL  	rd_16	rs1_6	rs2_15	imm_0	02F30833 
0060 LW   	rd_17	rs1_3	rs2_0	imm_0	0001A883 
0064 LW   	rd_18	rs1_3	rs2_0	imm_12	00C1A903 
0068 DIV  	rd_19	rs1_17	rs2_18	imm_0	0328C9B3 
006C ADD  	rd_20	rs1_7	rs2_19	imm_0	01338A33 
0070 SUB  	rd_21	rs1_20	rs2_16	imm_0	410A0AB3 
0074 DIV  	rd_22	rs1_6	rs2_21	imm_0	03534B33 
0078 ADD  	rd_23	rs1_22	rs2_13	imm_0	00DB0BB3 
007C ADD  	rd_24	rs1_23	rs2_0	imm_0	000B8C33 
0080 ADD  	rd_25	rs1_24	rs2_1	imm_0	001C0CB3 
0084 LW   	rd_26	rs1_3	rs2_0	imm_8	0081AD03 
0088 MUL  	rd_25	rs1_25	rs2_26	imm_0	03AC8CB3 
008C MUL  	rd_27	rs1_6	rs2_7	imm_0	02730DB3 
0090 MUL  	rd_28	rs1_7	rs2_15	imm_0	02F38E33 
0094 ADD  	rd_29	rs1_6	rs2_28	imm_0	01C30EB3 
0098 SUB  	rd_30	rs1_29	rs2_27	imm_0	41BE8F33 
009C ADD  	rd_25	rs1_14	rs2_25	imm_0	01970CB3 
00A0 LW   	rd_31	rs1_25	rs2_0	imm_0	000CAF83 
00A4 ADD  	rd_31	rs1_30	rs2_0	imm_0	000F0FB3 
00A8 SW   	rd_0	rs1_25	rs2_31	imm_0	01FCA023 
00AC MUL  	rd_11	rs1_6	rs2_7	imm_0	027305B3 
00B0 SUB  	rd_12	rs1_11	rs2_6	imm_0	40658633 
00B4 ADD  	rd_13	rs1_12	rs2_0	imm_0	000606B3 
00B8 ADD  	rd_15	rs1_13	rs2_1	imm_0	001687B3 
00BC LW   	rd_16	rs1_3	rs2_0	imm_8	0081A803 
00C0 MUL  	rd_15	rs1_15	rs2_16	imm_0	030787B3 
00C4 LW   	rd_17	rs1_3	rs2_0	imm_4	0041A883 
00C8 MUL  	rd_18	rs1_7	rs2_17	imm_0	03138933 
00CC LW   	rd_19	rs1_3	rs2_0	imm_0	0001A983 
00D0 MUL  	rd_20	rs1_6	rs2_19	imm_0	03330A33 
00D4 ADD  	rd_21	rs1_20	rs2_18	imm_0	012A0AB3 
00D8 ADD  	rd_15	rs1_14	rs2_15	imm_0	00F707B3 
00DC LW   	rd_22	rs1_15	rs2_0	imm_0	0007AB03 
00E0 ADD  	rd_22	rs1_21	rs2_0	imm_0	000A8B33 
00E4 SW   	rd_0	rs1_15	rs2_22	imm_0	0167A023 
00E8 LW   	rd_11	rs1_3	rs2_0	imm_0	0001A583 
00EC ADD  	rd_12	rs1_11	rs2_0	imm_0	00058633 
00F0 ADD  	rd_13	rs1_12	rs2_1	imm_0	001606B3 
00F4 LW   	rd_15	rs1_3	rs2_0	imm_8	0081A783 
00F8 MUL  	rd_13	rs1_13	rs2_15	imm_0	02F686B3 
00FC LW   	rd_16	rs1_3	rs2_0	imm_16	0101A803 
0100 ADD  	rd_13	rs1_14	rs2_13	imm_0	00D706B3 
0104 LW   	rd_17	rs1_13	rs2_0	imm_0	0006A883 
0108 ADD  	rd_17	rs1_16	rs2_0	imm_0	000808B3 
010C SW   	rd_0	rs1_13	rs2_17	imm_0	0116A023 
0110 SUB  	rd_11	rs1_7	rs2_6	imm_0	406385B3 
0114 ADD  	rd_12	rs1_11	rs2_0	imm_0	00058633 
0118 ADD  	rd_13	rs1_12	rs2_1	imm_0	001606B3 
011C LW   	rd_15	rs1_3	rs2_0	imm_8	0081A783 
0120 MUL  	rd_13	rs1_13	rs2_15	imm_0	02F686B3 
0124 LW   	rd_16	rs1_3	rs2_0	imm_0	0001A803 
0128 MUL  	rd_17	rs1_6	rs2_16	imm_0	030308B3 
012C ADD  	rd_18	rs1_17	rs2_0	imm_0	00088933 
0130 ADD  	rd_19	rs1_18	rs2_1	imm_0	001909B3 
0134 MUL  	rd_19	rs1_19	rs2_15	imm_0	02F989B3 
0138 ADD  	rd_20	rs1_6	rs2_0	imm_0	00030A33 
013C ADD  	rd_21	rs1_20	rs2_1	imm_0	001A0AB3 
0140 MUL  	rd_21	rs1_21	rs2_15	imm_0	02FA8AB3 
0144 ADD  	rd_21	rs1_14	rs2_21	imm_0	01570AB3 
0148 LW   	rd_22	rs1_21	rs2_0	imm_0	000AAB03 
014C ADD  	rd_19	rs1_14	rs2_19	imm_0	013709B3 
0150 LW   	rd_23	rs1_19	rs2_0	imm_0	0009AB83 
0154 ADD  	rd_24	rs1_22	rs2_23	imm_0	017B0C33 
0158 SW   	rd_0	rs1_21	rs2_22	imm_0	016AA023 
015C SW   	rd_0	rs1_19	rs2_23	imm_0	0179A023 
0160 ADD  	rd_13	rs1_14	rs2_13	imm_0	00D706B3 
0164 LW   	rd_22	rs1_13	rs2_0	imm_0	0006AB03 
0168 ADD  	rd_23	rs1_24	rs2_22	imm_0	016C0BB3 
016C SW   	rd_0	rs1_13	rs2_22	imm_0	0166A023 
0170 ADDI 	rd_10	rs1_23	rs2_0	imm_0	000B8513 
0174 ADDI 	rd_10	rs1_0	rs2_0	imm_255	0FF00513 
0178 ADDI 	rd_10	rs1_0	rs2_0	imm_0	00000513 
017C ADDI 	rd_10	rs1_0	rs2_0	imm_255	0FF00513 
```

- 机器码

```
@00000000
B7 01 01 00 37 01 02 00 93 04 01 05 13 84 04 04 
83 22 01 00 FF 02 00 00 03 23 81 00 7F 03 00 00 
83 23 C1 00 FF 03 00 00 B3 05 04 00 33 86 02 00 
23 20 C4 00 83 A6 81 00 33 04 D4 00 33 86 C6 02 
33 04 C4 00 03 27 41 00 33 87 05 00 B3 05 73 02 
33 C6 75 02 B3 86 C3 40 83 A7 41 00 33 08 F3 02 
83 A8 01 00 03 A9 C1 00 B3 C9 28 03 33 8A 33 01 
B3 0A 0A 41 33 4B 53 03 B3 0B DB 00 33 8C 0B 00 
B3 0C 1C 00 03 AD 81 00 B3 8C AC 03 B3 0D 73 02 
33 8E F3 02 B3 0E C3 01 33 8F BE 41 B3 0C 97 01 
83 AF 0C 00 B3 0F 0F 00 23 A0 FC 01 B3 05 73 02 
33 86 65 40 B3 06 06 00 B3 87 16 00 03 A8 81 00 
B3 87 07 03 83 A8 41 00 33 89 13 03 83 A9 01 00 
33 0A 33 03 B3 0A 2A 01 B3 07 F7 00 03 AB 07 00 
33 8B 0A 00 23 A0 67 01 83 A5 01 00 33 86 05 00 
B3 06 16 00 83 A7 81 00 B3 86 F6 02 03 A8 01 01 
B3 06 D7 00 83 A8 06 00 B3 08 08 00 23 A0 16 01 
B3 85 63 40 33 86 05 00 B3 06 16 00 83 A7 81 00 
B3 86 F6 02 03 A8 01 00 B3 08 03 03 33 89 08 00 
B3 09 19 00 B3 89 F9 02 33 0A 03 00 B3 0A 1A 00 
B3 8A FA 02 B3 0A 57 01 03 AB 0A 00 B3 09 37 01 
83 AB 09 00 33 0C 7B 01 23 A0 6A 01 23 A0 79 01 
B3 06 D7 00 03 AB 06 00 B3 0B 6C 01 23 A0 66 01 
13 85 0B 00 13 05 F0 0F 13 05 00 00 13 05 F0 0F 

@00010000
02 00 00 00 03 00 00 00 04 00 00 00 05 00 00 00 
07 00 00 00 

```

### 遗留问题

- 优化不足，对于数组变量使用后立即存储，造成了较多的内存读写。运行速度不够快。
- For 循环实现不够美观，在特殊情况下可能会出现BUG，比如 sema_8 这个测试点中就是由于 For 循环的某些跳转问题导致了一些常量没有及时加载而产生了错误。如需修改，可能需要大量改动源代码，故暂时放弃。


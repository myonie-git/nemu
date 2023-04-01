# RISC-V "MC" Memristor Crossbar Extension

## 1. Introduction

本项目旨在基于RISC-V构建一套可以高效支持忆阻器交叉阵列计算的指令集架构，简称RV64ME。RV64ME扩展是基于RV64I，RV64M两个基本RISCV指令集架构并结合了RV64V指令集架构的部分设计思想来进行设计的。RV64ME提供了利用忆阻器交叉阵列进行并行存内计算的指令集架构。RV64ME同时提供了对Resnet、MobileNet等传统ANN神经网络以及Spiking Resnet等新型SNN神经网络的推理指令的支持。

> TODO :

## 2. RV64MC programmers' model

### 2.1 概述

在本指令集架构中，我们使用忆阻器交叉阵列用于进行并行的数据处理，并希望利用忆阻器交叉阵列来代替传统的向量体积结构。但由于忆阻器交叉阵列中仍然需要利用向量寄存器来进行许多操作（如：bit lines 和 word lines的选通、数据的输入/读取等等），因此，我们保留了一部分向量体系结构的内容，保留的具体内容将在本章的剩余部分中进行介绍。

由于目前对于忆阻器的工艺水平有限，同时如果忆阻器交叉阵列过大，那么忆阻器的漏电流在提升的同时，还会造成忆阻器交叉阵列的效率过低。因此，我们仍然将忆阻器作为一种特殊的计算单元来进行使用。在进行计算时，我们仍然采用载入-存储体系结构的方式来进行计算。

### 2.2 寄存器

在本架构中，除去RV64I中的特权指令寄存器外，还增加了以下几个寄存器：

- vgpr[32]: 向量寄存器，用于存储向量数据
- mpr[4]: 忆阻器位置寄存器，用于存储忆阻器的位置信息
- ml: 忆阻器长度寄存器，用于存储忆阻器的长度信息
- mctype: 忆阻器元素类型寄存器，用于存储忆阻器中元素的类型信息以及向量寄存器中的元素类型
- vl: 向量长度寄存器，用于存储向量的长度信息
- vstart: 向量起始寄存器，用于存储向量的起始位置信息
- p: 谓词寄存器，用于存储谓词信息

### 2.3 忆阻器交叉阵列以及向量寄存器长度设置

- 忆阻器交叉阵列大小设置：在本指令集中，我们设定忆阻器交叉阵列的大小为XLEN\*XLEN，XLEN参数可以在config.json文件中进行设置(为了设计方便，同时考虑到Resnet神经网络的最大卷积核为7*7，以及加法的便捷性，我们暂定XLEN=64，在之后的设计中，会进一步进行修改)
- 向量寄存器长度：由于在本指令集架构中，向量主要用于对忆阻器进行载入、忆阻器的行列选通、忆阻器计算结构的读取。因此，我们设置向量寄存器的长度与忆阻器相同，即XLEN*64位，可以存储XLEN个数字。

> TODO：是否需要设置长度为XLEN*1的寄存器用于wordlines的选通？

### 2.4 谓词寄存器 p

谓词寄存器沿用RV64V中关于谓词寄存器中的定义，其长度为XLEN位，谓词寄存器在本架构中的主要用途是[忆阻器加法 madd](#331-忆阻器加法指令-madd)的快速导入。具体的使用样例可以参照[残差连接 Residual](#44-残差链接residual)。

> TODO：去看看量化优化方法是怎么写的

### 2.5 小数计算

本架构计划实现定点计算而非浮点计算，为了保证神经网络的计算在可计算范围内，因此，我们对神经网络采用量化处理。

> TODO：目前先采用整数进行计算

### 2.6 忆阻器位置寄存器mgpr

忆阻器位置寄存器用于记录一组需要计算的数据在忆阻器中的位置，忆阻器寄存器长度为64位，每32位分别记录数据在忆阻器中的起始点的x坐标，y坐标以及长，宽。同时，我们设置了一系列[忆阻器位置寄存器设置指令](#353-忆阻器位置寄存器设置指令-seti)以对其进行操作。

忆阻器位置寄存器暂时设置为4个，分别命名为m0,m1,m2,m3。

> 参数寄存器的设置，也可以与lif参数寄存器共用一套编码。

### 2.7 忆阻器长度寄存器ml

忆阻器长度寄存器与vl相似，用于记录在忆阻器中进行计算的元素的长以及高。

### 2.8 忆阻器元素类型寄存器mctype

忆阻器长度寄存器，用于记录忆阻器中进行操作的元素类型,未来可能也会计算长度以及范围等等。目前取值设置与vsew相同。

| mctype | 元素类型 |
| ----- | -------- |
| 0     | 8        |
| 1     | 16       |
| 2     | 32       |
| 3     | 64       |

其余编码保留，用于日后进行扩展

### 2.9 向量长度寄存器vl

向量长度寄存器用于记录向量寄存器中的向量的长度，向量长度寄存器的长度为64位，其取值范围为0~XLEN-1。

> TODO: 暂定向量寄存器长度为64位

### 2.10 向量起始寄存器vstart

向量起始寄存器vstart用于记录向量寄存器中的起始位置，向量起始寄存器的长度为64位，其取值范围为0~XLEN-1。


## 3. RV64MC Instruction Set

> 目前的所有指令： mld, mst, madd, msub, ewmul, mdot, vgtm, guv, set, lut, lif ...

### 3.1 概述

> TODO：待完成

#### 3.1.1 指令格式

本指令集架构的指令格式分为两种：一种是RV64V中的保留指令，另一种则是RV64MC的相关指令

#### 3.1.1.1 RV64V指令编码

RV64V指令编码分为两部分：算术指令以及载入存储指令。

- 算术指令使用了OP-V的opcode编码，指令格式如下：

  <div align=center>
    <img src=OP-V.jpg style="zoom:60%;"/>
    </div>

  本指令集架构中删除了所有的算术指令，均使用忆阻器的相关指令进行替代。

  另外，本指令集架构中还增加了以下两条指令；

  - vgtm: funct3 = OPIVV/OPIVX/OPIVI; funct6 = 001000
  - guv:  funct3 = OPIVV/OPIVX/OPIVI; funct6 = 001101
- 导入和存储指令使用了OP-FP的opcode编码，与浮点运算共用同一个opcode，指令格式如下：

  <div align=center>
    <img src=vector-load-store-instr.jpg style="zoom:60%;"/>
    </div>

#### 3.1.1.2 RV64MC指令编码

RV64MC使用了OP-MC(0001011)的opcode编码空间，指令格式与RV64V基本相同，funct3提供了以下几种基本的编码：

| funct3 | 编 |   | 码 | 备注               |
| ------ | -- | - | -- | ------------------ |
| OPICC  | 0  | 0 | 0  |                    |
| OPFCC  | 0  | 0 | 1  |                    |
| OPICV  | 0  | 1 | 0  |                    |
| OPICI  | 0  | 1 | 1  |                    |
| OPICX  | 1  | 0 | 0  |                    |
| OPFCV  | 1  | 0 | 1  | TODO               |
| OPFCF  | 1  | 1 | 0  | TOOD               |
| OPSET  | 1  | 1 | 1  | 用于设置相关寄存器 |

- I, F：表示指令类型（整数/浮点）
- C, V, X, I, F: 表示数据类型（忆阻器交叉阵列位置寄存器/向量寄存器/通用寄存器/立即数/浮点）

具体的指令格式编码请见指令说明


### 3.2 忆阻器载入指令

#### 3.2.1 忆阻器载入指令mld

- 指令格式：mld ms1, vs1
- 指令功能：将vs1向量寄存器中的数据导入到忆阻器m0的位置中
- 指令编码：TODO()
    - opcode: OP-MC
    - funct3: OPICV, OPICI, OPICX
    - funct6: 000010
    

- 指令说明：导入向量的长度由忆阻器位置寄存器决定，导入向量的行选通由谓词寄存器决定。

### 3.3 忆阻器计算指令

> TODO: 忆阻器计算的向量对齐方式还未确定

#### 3.3.1 忆阻器加法指令 madd

- 指令格式：madd vd, ms1, ms2
- 指令功能：将位于忆阻器中ms1和ms2位置的数据相加，并将结果保存到vd中
- 指令编码：TODO()
    - opcode: OP-MC
    - funct3: OPICC, OPFCC
    - funct6: 000000

- 指令说明：ms1与ms2要求位置以及列宽相同，在进行运算时，硬件会对此进行检查，若二者不相同，则会掉入trap中进行处理。

> TODO: 本指令集架构尚未实现特权指令架构

#### 3.3.2 忆阻器减法指令 msub

- 指令格式：msub vd, ms1, ms2
- 指令功能：将位于忆阻器中ms1和ms2位置的数据相减，并将结果保存到vd中
- 指令编码：TODO()
    - opcode: OP-MC
    - funct3: OPICC, OPFCC
    = funct6: 000001
- 指令说明：ms1与ms2要求位置以及列宽相同，在进行运算时，硬件会对此进行检查，若二者不相同，则会掉入trap中进行处理。

> TODO: 本指令集架构尚未实现特权指令架构

#### 3.3.3 忆阻器按元素乘法指令 mewmul

- 指令格式：mewmul vd, ms1, vs1
- 指令功能：将ms1与vs1进行elemenwise的乘法，并将计算结果保存到vs1中
- 指令编码：TODO()
    - opcode:OP-MC
    - funct3: OPICV, OPICI, OPICX
    - funct6: 000010
- 指令说明：计算的长度由忆阻器位置寄存器设置；如果m0的宽度不为1，那么会掉入trap；向量的选通由谓词寄存器决定

#### 3.3.4 忆阻器点积指令 mdot

- 指令格式：mdot, vd, ms1, vs1
- 指令功能：计算ms1与vs1的点击
- 指令编码：TODO()
    - opcode: OP-MC
    - funct3: OPICV, OPICI, OPICX
    - funct6: 000000
- 指令说明：计算的长度由忆阻器位置寄存器设置；如果忆阻器寄存器中的行长度大于可用的行长度，那么就会掉入trap；向量的选通由谓词寄存器决定

### 3.4 SNN相关指令

> TODO: 先实现ANN指令

#### 3.4.1 神经元指令 lif

### 3.5 其他指令

#### 3.5.1 向量寄存器合并指令 vgtm

- 指令格式：vgtm vd, vs1, vs2
- 指令功能：比较vs1和vs2每个向量的大小，并把二者中元素较大的数据保存到vd中
- 指令编码：
    - opcode: OP-V
    - funct3: OPIVV, OPIVX, OPIVI
    - funct6: 001000

  vgtm提供OPIVV，OPIVX， OPIVI三种编码方式：

  <div align=center>
    <img src=OP-V.jpg style="zoom:60%;"/>
    </div>
    
- 指令说明：本指令主要用于最大池化层的相关操作，具体操作如下图所示：

<div align=center>
<img src=vgtm.png style="zoom:60%;"/>
</div>

> TODO：vgtm或许可以用之后提出的谓词寄存器取代？ 不可以，因为我们没有提供专门的向量寄存器计算指令？
> TODO: 或许可以用memristor来进行计算？不可以，我们并没有设置vpne等之类的谓词寄存器指令

#### 3.5.2 单位向量寄存器生成指令 guv

- 指令格式： guv vd, rs1
- 指令功能：生成单位向量，将rs1的单位向量导入到vd中
- 指令编码：

    - opcode: OP-V
    - funct3: OPIVV, OPIVX, OPIVV
    - funct6: 001101

- 指令说明：生成向量的长度由setvl决定

> TODO：指令编码存在部分空间没有利用上

#### 3.5.3 忆阻器位置寄存器设置指令 

- 指令格式：set{x,i}{x,i} rd, {rs1, imm}, {rs2, imm}
- 指令功能：设置忆阻器位置寄存器的x坐标/y坐标/列宽度/行长度设置为rs2
- 指令编码：TODO()
    - opcode: OP-MC
    - funct3: OPSET
    - funct6: funct4 + funct2
        - funct4: 指令编码，具体指令编码见[RV64MC-Instr.xlsx](./RV64MC-Instr.xlsx)
        - funct2 : 指令类型
            - 00: src1:imm, src2: imm
            - 01: src1:imm, src2: reg
            - 10: src1:reg, src2: imm
            - 11: src1:reg, src2: reg

#### 3.5.4 查找表指令 lut

- 指令格式： lut vd, vs1, fun_op

_ 指令功能：对fun_op功能对应的函数将fun_op(vs1)的输出结果保存在vd中。

- 指令编码：
    - opcode: OP-V
    - funct3: OPIVI, OPIVX
    - funct6: 010101
- 指令说明：fun_op代表所需要函数查找表的opcode，暂定relu为0

### 3.6 RV64V 保留指令

#### 3.6.1 RV64V保留指令内容

> 由于忆阻器交叉阵列中需要使用向量寄存器来读取和载入数据。因此，我们保留了RV64V中关于向量读取和载入的相关指令。

在本指令集架构中，我们需要使用向量寄存器来进行忆阻器交叉阵列的的导入以及导出操作，但同时，由于我们几乎不使用向量扩展来进行相关计算，更无法做到对向量进行赋值的相关操作。因此，我们暂定仅使用向量载入/存储和跨距载入/存储两个指令来进行相关操作。

- vld:  向量导入
- vlds: 跨距导入
- vldx: 索引导入：不实现
- vsd:  向量存储
- vsds: 跨距存储
- vsdx: 索引存储：不实现

与RV64V不同，我们在这里选择将向量寄存器的导入以及存储设置为固定长度的读取，同时取消其中的nf字段。

> TODO: 谓词寄存器的相关设置，暂不设置，仅提供通过寄存器和立即数导入谓词寄存器

> **由于指令load-store指令，我们暂且不实现相关的csr寄存器，如sew，lmul等等，因此vld，vlds指令用不上这些csr指令**

> TODO: 暂定无论多少位，都只有XLEN个元素，正好一一对应导入的位置，这样在架构上也会节省很多的空间，**暂时**不使用动态的范围。

#### 3.6.2 保留指令

> TODO: 先按照32个指令进行设计，之后再改这个东西

具体指令编码如下所示：

<div align=center>
<img src=vector-load-store-instr.jpg style="zoom:90%;"/>
</div>

各个字段的功能如下：

<div align=center>
<img src=vector-load-store-field.jpg style="zoom:60%;"/>
</div>

其中，MEW指令用于编码128位以及其以上的内存编码，保留。

#### 3.6.3 width字段

与RV64F指令共用opcode，二者通过width字段进行区分，如下图所示：

<div align=center>
<img src=vector-load-store-width.jpg style="zoom:60%;"/>
</div>

> TODO: 具体说明待完善

#### 3.6.4 mop 字段

<div align=center>
<img src=vector-load-store-mop.jpg style="zoom:60%;"/>
</div>

> TODO: 具体说明待完善

#### 3.6.5 lumop \& sumop 字段

<div align=center>
<img src=vector-load-store-lumop.jpg style="zoom:60%;"/>
</div>

> TODO: 具体说明待完善

#### 3.6.6 nf 字段

<div align=center>
<img src=vector-load-store-nf.jpg style="zoom:60%;"/>
</div>

nf字段用于表示nfileds编码，用于对要加载和存储的向量寄存器的数量进行编码。仅支持NFIELDS值为1、2、4、8，保留其他值。

> TODO: 本项目中，我们暂时不使用动态的范围，因此，我们暂时不使用nf字段，而是直接将nf字段设置为0，即只能导入一个向量寄存器。

#### 3.6.7 其他指令

> TODO: 可以设置一个v(i) = f(i) 的相关指令？暂不实现，因为用不上

## 4. Implementation of Neural Network Operator

> TODO: 有很多错误，需要进一步完善

### 4.1 卷积Conv

```assembly
conv:
#the size of conv stores at a0
#the number channel of conv stores at a1
#the address of conv stores at a2
#the size of memristor and vector register is 32
#the address of conv results stores at a3
    mul a0, a0, a0
    setvl t0, a0
    li t2, 32 #loop_max
    li t1, 0 #loop_count 
    mv t3, a2 #address
    mv t4, a1 #channel
    guv v2, f0 #vadd.vs v2, v0, 0
  
conv_store_loop:  
    vld v1, t3
    add t3, t3, a0
    setaxi m0, t1 #\
    setayi m0, 0  # |
    setsxi m0, 1  # | one instr
    setsyi m0, t0 #/
    msd v1, m0
    addi t1, t1, 1
    blt t1, t2, conv_store_loop
    blt t1, a1, conv_store_loop # TODO
  
conv_cal:
    mdot mt0, m0
    rd v2, mt0
    #mdot v3, m0
    #vadd.vv v2, v2, v3
    sub t4, t4, t0
    bge x0,t4, next_conv #已经结束计算 t4<=0 下一组数据
    #没有结束，卷积核过大
    sub t6, a0, t4 #t6: offset of address 
    add t3, a2, t6
    setvl t0, t4 #t4：由于卷积核过大，未导入memristor中数组的长度
    li t1, 0

    j conv_store_loop

next_conv:
    #判定是否结束

    vsd v2, a3
    add a3, a3, t1
    sub a1, a1, t1
    bge x0, a1, conv_end
    #进行下一轮卷积
    li t1, 0
    guv v2, f0 #vadd.vs v2, v0, 0
conv_end:
    ret
```

### 4.2 最大池化层Max-Pooling

```assembly
max_pooling:
#the address of data stores at a0
#the size of kernel stores at a1
#the number of channel stores at a2
#the address of results stores at a3
    mul a1, a1, a1
    setvl t0, a2
    mv t3, a0 #临时存储地址
    li t1, 0
max_loop:
    li t2, 0
    vlds v1, t3, a1
merge_loop:
    vlds v2, t3, a1 
    vgtm v1, v1, v2
    addi t3, t3, 4#这里不应该是+4，而是数据的长度
    addi t2, t2, 1
    bge a1, t2, merge_loop
    #判断是否还有channel没有处理
    add t1, t1, t0
    bge t1, a2, max_end
    muli t4, t0, 4 #t4 = 4 * t0
    muli t5, t4, a1 #地址=原地址 + 4 * channel数量 * kernel size
    add t3, t3, t4 #更新地址
    add a3, a3, t4 #更新存储地址
    sub t6, a2, t1 #t6:还没有处理的channel
    setvl t0, t6
    j max_loop
max_end:
    ret
```

### 4.3 平均池化层Avg-Pooling

```assembly
avg_pool:
#the address of data stores at a0
#the size of kernel stores at a1
#the number of channel stores at a2
#the address of results stores at a3
    mul a1, a1, a1
    setvl t0, a2
    li t2, 32#loop_max
    li t1, 0 #loop_count
    mv t3, a0 #address
    mv t4, a2 #channel
    guv v2, f0 #vadd.vs v2, v0, 0
avg_store_loop:
    vld v1, t3
    add t3, t3, a1
    setaxi m0, t1 # \ 
    setayi m0, 0  # |
    setsxi m0, 1  # |one instr
    setsyi m0, t0 #/
    msd v1, m0
    addi t1, t1, 1
    blt t1, t2, avg_store_loop
    blt t1, a2, conv_store_loop
avg_cal:
    faddi f1, f0, 1
    fdiv f1, f1, a1
    guv v4, f1
    mdot mt0, mt0
    rd v2, mt0
    sub t4, t4, t0
    bge x0, t4, next_avg #已经结束计算
    #没有结束，卷积核过大
    sub t6, a1, t4
    add t3, a0, t6
    setvl t0, t4
    li t1, 0
    j avg_store_loop
next_avg:
    vsd v2, a3
    add a3, a3, t1
    sub a2, a2, t1
    bge x0, a2, avg_end
    li t1, 0
    guv v2, f0
avg_end:
    ret
```

### 4.4 残差链接Residual

```assembly
resnet: 
#the address of data1 stores at a0
#the address of data2 stores at a1
#the size of data stores at a2
#the results of data stores at a3
    setvl t0, a2
    li t1, 16 #loop_max
    li t2, 0  #loop_count 
    mv t3, a0 #address1
    mv t4, a1 #address2
    mv t5, a3 #results
    mul s1, t0, 4

resnet_store_loop:
    mv p0 0xaaaaaaaa
    vlds v1, t3, s1 #s1 = t0 * 4
    mv p0 0x99999999
    vlds v1, t4, s1
    mv p0 0xffffffff
    addi t3, t3, 4
    addi t4, t4, 4
    setaxi m0, t1
    setayi m0, 0
    setsxi m0, 1
    setsyi m0, t0
    addi t2, t2, 1
    blt t2, t1, resnet_store_loop
    blt t2, a2, conv_store_loop #TODO
resnet_cal: 
    mv t6, 0
resnet_cal_loop:
    setaxi m0, 0
    setayi m0, t6
    setsxi m0, t2
    setsyi m0, 1
    addi t6, t6, 1
    setaxi m1, 0
    setayi m1, t6
    setsxi m0, t2
    setsyi m0, 1
    madd v2, m0, m1
    vsds v2, t5, s1
    addi t5, t5, 4
    add t6, t6, a0
    blt t6, t1, resnet_cal_loop
    blt t6, a2, resnet_cal_loop
    #如果完成本次计算，预处理下一次导入
    sub a2, a2, t6
    add t3, t3, t6
    add t4, t4, t6
    #这里只考虑了输入数据大小可以整除的情况
    blt x0, a2, resnet_store_loop
resnet_end:
    ret
```

### 4.5 全连接层fc

> TODO()

### 4.6 批量归一化bn

> TODO()

### 4.7 查找表指令lut

```assembly
lut:
#the address of data stores at a0
#the number of vector stores at a1
#the address of results stores at a2
#the function code of lut stores at a3  
    setvl t0, a1
lut_loop:
    vld v1, a0
    add a0, a0, t0
    sub a1, a1, t0
    lut v2, v1, a3 #a3 is the function code of relu
    vsd v2, a2
    add a2, a2, t0
    blt x0, a1, lut_loop
    ret
```

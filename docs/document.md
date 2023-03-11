
#  RISC-V "MC" Memristor Crossbar Extension


## 1. Introduction

本项目旨在基于RISC-V构建一套可以高效支持忆阻器交叉阵列计算的指令集架构，简称RV64ME。RV64ME扩展是基于RV64I，RV64M两个基本RISCV指令集架构并结合了RV64V指令集架构的部分设计思想来进行设计的。RV64ME提供了利用忆阻器交叉阵列进行并行存内计算的指令集架构。RV64ME同时提供了对Resnet、MobileNet等传统ANN神经网络以及Spiking Resnet等新型SNN神经网络的推理指令的支持。

> TODO :

## 2. RV64ME programmers' model

### 2.1 概述

在本指令集架构中，我们使用忆阻器交叉阵列用于进行并行的数据处理，并希望利用忆阻器交叉阵列来代替传统的向量体积结构。但由于忆阻器交叉阵列中仍然需要利用向量寄存器来进行许多操作（如：bit lines 和 word lines的选通、数据的输入/读取等等），因此，我们保留了一部分向量体系结构的内容，保留的具体内容将在本章的剩余部分中进行介绍。

由于目前对于忆阻器的工艺水平有限，同时如果忆阻器交叉阵列过大，那么忆阻器的漏电流在提升的同时，还会造成忆阻器交叉阵列的效率过低。因此，我们仍然将忆阻器作为一种特殊的计算单元来进行使用。在进行计算时，我们仍然采用读取-载入-计算的方式来进行计算。

>  TODO: 忘记了读取-载入-计算这种计算结构叫什么了，回去查一下...


### 2.2 忆阻器交叉阵列以及向量寄存器长度设置


- 忆阻器交叉阵列大小设置：在本指令集中，我们设定忆阻器交叉阵列的大小为XLEN\*XLEN，XLEN参数可以在config.json文件中进行设置(为了设计方便，同时考虑到Resnet神经网络的最大卷积核为7*7，以及加法的便捷性，我们暂定XLEN=64，在之后的设计中，会进一步进行修改)

- 向量寄存器长度：由于在本指令集架构中，向量主要用于对忆阻器进行载入、忆阻器的行列选通、忆阻器计算结构的读取。因此，我们设置向量寄存器的长度与忆阻器相同，即XLEN*64位，可以存储XLEN个数字。

> TODO：是否需要设置长度为XLEN*1的寄存器用于wordlines的选通？

### 2.3 谓词寄存器

谓词寄存器沿用RV64V中关于谓词寄存器中的定义，其长度为XLEN位，谓词寄存器在本架构中的主要用途是[忆阻器加法 madd](#331-忆阻器加法指令-madd)的快速导入。具体的使用样例可以参照[残差连接 Residual](#44-残差链接residual)。


> TODO：去看看量化优化方法是怎么写的

### 2.4 小数计算

本架构计划实现定点计算而非浮点计算，为了保证神经网络的计算在可计算范围内，因此，我们对神经网络采用量化处理。

> TODO：目前先采用整数进行计算

### 2.5 忆阻器位置寄存器

忆阻器位置寄存器用于记录一组需要计算的数据在忆阻器中的位置，忆阻器寄存器长度为64位，每16位分别记录数据在忆阻器中的起始点的x坐标，y坐标以及长，宽。同时，我们设置了一系列[忆阻器位置寄存器设置指令](#25-忆阻器位置寄存器)以对其进行操作。

忆阻器位置寄存器共有四个，分别命名为m0,m1,m2,m3。

> TODO: 忆阻器寄存器是否可以与通用寄存器共用？但还需要考虑到lif
参数寄存器的设置，也可以与lif参数寄存器共用一套编码。

### 2.6 Tradeoff between RV64V and RV64ME

> TODO: 本架构在RV64V中保留的内容以及删除的内容

## 3. RV64ME Instruction Set

### 3.1 概述

> TODO：待完成

### 3.2 忆阻器载入指令

#### 3.2.1 忆阻器载入指令mld

- 指令格式：mld ms1, vs1

- 指令功能：将vs1向量寄存器中的数据导入到忆阻器m0的位置中

- 指令编码：TODO()

- 指令说明：导入向量的长度由忆阻器位置寄存器决定，导入向量的行选通由谓词寄存器决定。

> TODO: 指令空间并未利用完整，可以进一步考察一下

> TODO: 需要考虑一下行选通的问题？- 谓词寄存器解决/忆阻器位置寄存器做成一个类似于位置寄存器的东西？


### 3.3 忆阻器计算指令

> TODO: 忆阻器计算的向量对其方式还未确定

#### 3.3.1 忆阻器加法指令 madd

- 指令格式：madd vd, ms1, ms2

- 指令功能：将位于忆阻器中ms1和ms2位置的数据相加，并将结果保存到vd中

- 指令编码：TODO()

- 指令说明：ms1与ms2要求位置以及列宽相同，在进行运算时，硬件会对此进行检查，若二者不相同，则会掉入trap中进行处理。

> TODO: 本指令集架构尚未实现特权指令架构


#### 3.3.2 忆阻器减法指令 msub

- 指令格式：msub vd, ms1, ms2

- 指令功能：将位于忆阻器中ms1和ms2位置的数据相减，并将结果保存到vd中

- 指令编码：TODO()

- 指令说明：ms1与ms2要求位置以及列宽相同，在进行运算时，硬件会对此进行检查，若二者不相同，则会掉入trap中进行处理。

> TODO: 本指令集架构尚未实现特权指令架构

#### 3.3.3 忆阻器按元素乘法指令 ewmul

- 指令格式：ewmul vd, ms1, vs1

- 指令功能：将ms1与vs1进行elemenwise的乘法，并将计算结果保存到vs1中

- 指令编码：TODO()

- 指令说明：计算的长度由忆阻器位置寄存器设置；如果m0的宽度不为1，那么会掉入trap；向量的选通由谓词寄存器决定

#### 3.3.4 忆阻器点积指令 mdot

- 指令格式：mdot, vd, ms1, vs1

- 指令功能：计算ms1与vs1的点击

- 指令编码：TODO()

- 指令说明：计算的长度由忆阻器位置寄存器设置；如果忆阻器寄存器中的行长度大于可用的行长度，那么就会掉入trap；向量的选通由谓词寄存器决定

### 3.4 SNN相关指令

> TODO: 先实现ANN指令

#### 3.4.1 神经元指令 lif

### 3.5 其他指令

#### 3.5.1 向量寄存器合并指令 vgtm 

- 指令格式：vgtm vd, vs1, vs2

- 指令功能：比较vs1和vs2每个向量的大小，并把二者中元素较大的数据保存到vd中

- 指令编码：TODO()

- 指令说明：本指令主要用于最大池化层的相关操作，具体操作如下图所示：

<div align=center>
<img src=vgtm.png style="zoom:40%;"/>
</div>

> TODO：vgtm或许可以用之后提出的谓词寄存器取代？ 不可以，因为我们没有提供专门的向量寄存器计算指令？
> TODO: 或许可以用memristor来进行计算？不可以，我们并没有设置vpne等之类的谓词寄存器指令

#### 3.5.2 单位向量寄存器生成指令 luv

- 指令格式： luv vd, rs1

- 指令功能：生成单位向量，将rs1的单位向量导入到vd中

- 指令编码：TODO()

- 指令说明：生成向量的长度由setvl决定

#### 3.5.3 忆阻器位置寄存器设置指令 seti

- 指令格式：setaxi/setayi/setsxi/setsyi md, rs1, rs2

- 指令功能：设置忆阻器位置寄存器的x坐标/y坐标/列宽度/行长度设置为rs2

- 指令编码：伪指令

- 指令说明：这几条指令只是为了方便编译以及汇编语言的编写，实际上，这几条指令可以合并为一条普通指令：

    ```
    setaxi m0, t0, 1 \
    setayi m0, t0, 1  | => li m0, 0x0001000100010001 
    setsxi m0, t0, 1  |  
    setsyi m0, t0, 1 /
    ```

#### 3.5.4 忆阻器位置寄存器设置指令 set

> TODO:这里的指令方式还需要再考虑一下

### 3.6 RV64V 保留指令

#### 3.6.1 向量寄存器载入指令

vld, vlds, vldx

- vld:  向量导入

- vlds: 跨距导入

- vldx: 索引导入

> TODO: 看一下量化优化方法那本书是怎么写的

#### 3.6.2 向量寄存器读取指令

- vsd:  向量存储

- vsds: 跨距存储

- vsdx: 索引存储

> TODO: 看一下量化优化方法那本书是怎么写的

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

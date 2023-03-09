#  RISC-V "MC" Memristor Crossbar Extension

## 1. Introduction

> TODO 

本项目旨在基于RISC-V构建一套可以高效支持忆阻器交叉阵列计算的指令集架构，简称RV64ME。RV64ME扩展是基于RV64I，RV64M两个基本RISCV指令集架构并结合了RV64V指令集架构的部分设计思想来进行设计的。RV64ME提供了利用忆阻器交叉阵列进行并行存内计算的指令集架构。

## 2. RV64ME programmers' model

### 2.1 忆阻器以及向量寄存器设置

> TODO

### 2.2 谓词寄存器

> TODO

### 2.3 小数激素那

> TODO

### 2.4 忆阻器位置寄存器

> TODO


## 3. RV64ME Instruction Set

### 3.1 忆阻器载入指令

#### 3.1.1 忆阻器载入指令mld

> TODO

### 3.2 忆阻器计算指令

#### 3.2.1 忆阻器加法指令 madd

#### 3.2.2 忆阻器减法指令 msub

#### 3.2.3 忆阻器按元素乘法指令 ewmul

#### 3.2.4 忆阻器点积指令 mdot

### 3.3 SNN相关指令

#### 3.3.1 神经元指令 lif

### 3.4 其他指令

> TODO

## 4. Implementation Of Neural Network Operator

> TODO

### 4.1 卷积Conv

### 4.2 最大池化层Max-Pooling

### 4.3 平均池化层Avg-Pooling

### 4.4 残差链接Residual

### 4.5 全连接层fc

### 4.6 批量归一化bn


## What is VPaxos?

VPaxos is an implementation of Paxos algorithm, strictly designed in accordance with **TLA+ Specification**.
<br>
<br>
More details at [Build Paxos In Practice](http://vectordb.io)


## Features
1. 严格遵从 **Paxos TLA+ Specification** 进行实现，保证逻辑正确性
2. 异步网络通信架构，完美适应分布式系统环境
3. 清晰的类设计与层次划分，通用的组件化设计

## Notes
>vpaxos不是一个用于生产环境的项目，它只是我自己用来研究如何实现Paxos算法的项目，可以说只是一个实验场。Paxos的精髓在于单值的确定（Single Value Paxos）。在这个实验场里，我尝试了Paxos的多种不同写法。我觉得把Single Value Paxos研究透，是很有价值的，这是分布式系统的基础。而在生产环境中，我们可以做一个raft的实现。vpaxos里面很多的组件（ThreadPool，AsyncCall，Node，Env，Timer）是通用的，可以毫无改动的用于另一个生产环境中使用的项目：**vraft:** [https://github.com/vectordb-io/vraft](https://github.com/vectordb-io/vraft)
>
>关于TLA+的细节，可以参考我的系列文章[《TLA+学习笔记》](http://vectordb.io)
>
>关于VPaxos的细节，可以参考我的系列文章[《自己动手实现Paxos》](http://vectordb.io)

## Acknowledgments

>吃水不忘挖井人。
>
>一些设计灵感来自于两个开源项目：leveldb，phxpaxos。另外，对lamport老爷子无比崇敬。感谢他搞出了TLA+，让程序员不在困惑于变化万千的分布式系统与算法。
>
>虽然leveldb问世已有10多年了，LSM-Tree结构也颇为简单，但我至今仍觉得它值得深入研究，具有长久价值。里面很多类的设计（xx, xx\_impl），层次的划分（env），组件的实现（cache, bloom），编码风格（玄门正宗的C++），都极其经典。如果头脑中有这样一个经典的代码模板，自己再写码时会大为受用。敲起键盘来，也更加虎虎生风 ---- 葵花在手，江山我有。^_^
>
>phxpaxos是腾讯开源的一个paxos库，最大的价值在于已经大规模应用于线上，这和那些胡乱吹嘘 “比xx性能高出xx倍” 的项目不可同日而语。虽然我不完全认同phxpaxos的设计实现，里面似乎也有一些subtle的代码，但我仍从中学到了很多东西。phxpaxos不失为一个值得研究的优质开源项目。
>
>最后要提一下lamport老爷子。对于搞分布式的同学来说，他真是神一样的存在。在TLA+ Toolbox中，一个简单的Single Value Paxos流程（3个ballot）就跑出了几十万个不同的状态。这种变化，人脑是无法把握的。但有了TLA+，有了形式化，我们写起代码来，也会更有信心。
>最后上两张lamport老爷子的帅图，这是他视频课程最后一节的截图。看着他远去的背影，感慨万千。正所谓：事了拂衣去，深藏功与名。
>
>
><img src="http://vectordb.io/images/lamport1.jpeg" width="30%">
><img src="http://vectordb.io/images/lamport2.jpeg" width="26%">

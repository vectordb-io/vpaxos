
## What is VPaxos?

VPaxos is an implementation of Paxos algorithm, strictly designed in accordance with **TLA+ Specification**.
<br>
<br>
More details at [Build Paxos In Practice](http://vectordb.io)


## Features
1. 严格遵从 **Paxos TLA+ Specification** 进行实现，保证逻辑正确性
2. 异步网络通信架构，完美适应分布式系统环境
3. 清晰的类设计与层次划分，通用的组件化设计

## note
vpaxos不是一个用于生产环境的项目，它只是为了用来研究如何实现一个Paxos算法。但里面很多的组件（ThreadPool，AsyncCall，Node，Env，Timer）是通用的，可以毫无改动的用于另一个生产环境中使用的项目：**vraft:** [https://github.com/vectordb-io/vraft](https://github.com/vectordb-io/vraft)

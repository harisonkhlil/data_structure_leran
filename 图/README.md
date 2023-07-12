# 图

图(graph) 是一种非线性的数据结构, 由顶点(vertex), 和边(edge)组成. 以下示例展示了一个包含 5 个顶点和 7 条边的图:

$$
\begin{aligned}
  V &= {1, 2, 3, 4, 5} \newline
  E &= {(1,2), (1, 3), (2, 3), (2, 4), (2, 5), (4, 5)} \newline
  G &= {V, E}
\end{aligned}
$$

![](https://raw.githubusercontent.com/harisonkhlil/oss/main/uPic/zcTBZo.png)

**相较于线性关系的链表和和分治关系的树,图的自由度更高,从而更加复杂**

## 图常见类型

根据有无方向可以分为:

- 无向图: 边表示两个顶点之间的双向关系
- 有向图: 边具有方向性, 两个节点之间的关系是独立地

![](https://raw.githubusercontent.com/harisonkhlil/oss/main/uPic/xuNA3J.png)

根据顶点是否连通可以分为:

- 连通图: 从某个顶点出发, 可以到达其余任意顶点
- 非连通图: 从某个顶点出发, 至少有一个顶点无法到达

![](https://raw.githubusercontent.com/harisonkhlil/oss/main/uPic/CRzs2J.png)

还可以根据 **权重** 变量, 得到 `有权图`

![](https://raw.githubusercontent.com/harisonkhlil/oss/main/uPic/56ygj7.png)

## 图的常用术语

`邻接(adjacency)`: 当两顶点之间存在边相连时, 称这两顶点 **邻接**

`路径(Path)`: 丛顶点 A 到顶点 B 经过的边构成的序列被称为从 A 到 B 的 **路径**

`度(degree)`: 表示一个顶点拥有的边数, 对于有向图，「入度 In-Degree」表示有多少条边指向该顶点，「出度 Out-Degree」表示有多少条边从该顶点指出。

## 图的表示

图的常用表示方法包括 `邻接矩阵` 和 `邻接表`. 以使用无向图进行举例:

### 邻接矩阵

设凸的顶点数量为 $n$, `邻接矩阵` 使用一个 $n \times n$ 大小的矩阵来表示图, 每行(列)代表一个顶点, 矩阵元素代表边, 用 0 或 1 代表两个顶点之间是否存在边.

如下图, 设邻接矩阵为 $M$, 顶点列表为 $V$, 那么矩阵元素 $M[i][j] = 1$ 代表顶点 $V[i]$ 到顶点 $V[j]$ 之间存在边, 反之则两个顶点之间没有边.

![](https://raw.githubusercontent.com/harisonkhlil/oss/main/uPic/SNiwxz.png)

邻接矩阵具有:

- 顶点不能与自身相连，因此邻接矩阵主对角线元素没有意义。
- 对于无向图，两个方向的边等价，此时邻接矩阵关于主对角线对称。
- 将邻接矩阵的元素从 1, 0 替换为 `权重`, 可以表示为 **有权图**

使用邻接矩阵表示图时，我们可以直接访问矩阵元素以获取边，因此增删查操作的效率很高，时间复杂度均为 $O(1)$ ,但是空间复杂度为 $O(n^2)$ ,内存占用较多

### 邻接表

![](https://raw.githubusercontent.com/harisonkhlil/oss/main/uPic/SDxFEh.png)

邻接表仅存储实际存在的边, 而边的总数通常要远远小于 $n^2$ ,因此更加节省空间, but, 在邻接表中需要通过遍历链表来查找边, 因此其时间效率不如邻接矩阵

观察上图可发现，邻接表结构与哈希表中的「链地址法」非常相似，因此我们也可以采用类似方法来优化效率。例如，当链表较长时，可以将链表转化为 AVL 树或红黑树，从而将时间效率从 $O(n)$ 优化至 $O(\log{n})$ 还可以通过中序遍历获取有序序列；此外，还可以将链表转换为哈希表，将时间复杂度降低至 $O(1)$
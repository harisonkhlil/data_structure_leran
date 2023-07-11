# 遍历寻找最大的 k 个元素

给定一个长度为 n 的无序数组 `nums`, 请返回数组中前 k 大的元素

## 遍历选择

进行 k 次遍历, 分别在每轮提取比 k 大的元素, 时间复杂度是 $O(nk)$ . 此方法仅仅适用于 $k \ll n$ 的情况, 如果两者比较接近, 那么时间复杂度就趋近于 $O(n^2)$ , 效率过于低下.


![](https://raw.githubusercontent.com/harisonkhlil/oss/main/uPic/omvsUB.png)

## 排序

对数组进行排序, 并返回最右边的 k 个元素, 时间复杂度为 $O(n\log{n})$, 但是我们超额完成任务了, 我们不需要排序啊

## 堆

流程如下:

1. 初始化一个小顶堆, 其堆顶元素最小
2. 先将数组的前 k 个元素依次入堆
3. 从第 k+1 个元素开始, 若当前元素大于堆顶元素, 则将堆顶元素出堆, 并将当前元素入堆
4. 遍历完成后, 堆中保存的就是答案

```cpp
priority_queue<int, vector<int>, greater<int>> topKheap(vector<int> &nums, int k) {
  // 初始化一个小顶堆
  priority_queue<int, vector<int>, greater<int>> heap;
  // 将数组的前 k 个元素入堆
  for (int i = 0; i < k; ++i) {
    heap.push(nums[i]);
  }
  // 从第 k+1 个元素开始，保持堆的长度为 k
  for (int i = k; i < nums.size(); ++i) {
    fi (nums[i] > heap.top()) {
      heap.pop();
      heap.push(nums[i]); // 入堆的实现不用管
    }
  }
  return heap;
}  
```
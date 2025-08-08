#include <iostream>

// ----------------------------------------------------------------
// 1. Expression基类模板 (CRTP)
//    - 提供 self() 方法来向下转型到实际的表达式类型。
//    - 重载 operator double()，这是惰性求值的触发点。
// ----------------------------------------------------------------
template<typename E>
struct Expression {
    // 隐式类型转换，当需要最终结果时被调用
    operator double() const {
        return static_cast<const E&>(*this).eval();
    }

    // CRTP辅助函数，用于访问派生类的成员
    const E& self() const {
        return static_cast<const E&>(*this);
    }
};

// ----------------------------------------------------------------
// 2. 字面量 (Literal) - 表达式树的叶子节点
//    - 继承自Expression，并传入自己作为模板参数。
//    - eval() 方法直接返回它持有的值。
// ----------------------------------------------------------------
class Lit : public Expression<Lit> {
    double value;
public:
    explicit Lit(double val) : value(val) {}
    double eval() const {
        return value;
    }
};

// ----------------------------------------------------------------
// 3. 操作符结构体 - 定义具体计算
// ----------------------------------------------------------------
struct Add {
    static double apply(double a, double b) { return a + b; }
};

struct Subtract {
    static double apply(double a, double b) { return a - b; }
};

struct Multiply {
    static double apply(double a, double b) { return a * b; }
};

struct Divide {
    static double apply(double a, double b) { return a / b; }
};

// ----------------------------------------------------------------
// 4. 二元操作表达式 - 表达式树的中间节点
//    - 模板参数：操作(Op), 左操作数(LHS), 右操作数(RHS)。
//    - 持有对左右子表达式的 const 引用，避免拷贝。
//    - eval() 方法递归地调用子表达式的 eval()，并使用Op::apply执行计算。
// ----------------------------------------------------------------
template<typename Op, typename LHS, typename RHS>
class BinaryExpr : public Expression<BinaryExpr<Op, LHS, RHS>> {
    const LHS& lhs;
    const RHS& rhs;
public:
    BinaryExpr(const LHS& l, const RHS& r) : lhs(l), rhs(r) {}

    double eval() const {
        // 递归求值
        return Op::apply(lhs.self().eval(), rhs.self().eval());
    }
};

// ----------------------------------------------------------------
// 5. 全局运算符重载
//    - 这些函数不立即计算。
//    - 它们返回一个新的 BinaryExpr 类型，构建了表达式树。
// ----------------------------------------------------------------
template<typename L, typename R>
BinaryExpr<Add, L, R> operator+(const Expression<L>& lhs, const Expression<R>& rhs) {
    return BinaryExpr<Add, L, R>(lhs.self(), rhs.self());
}

template<typename L, typename R>
BinaryExpr<Subtract, L, R> operator-(const Expression<L>& lhs, const Expression<R>& rhs) {
    return BinaryExpr<Subtract, L, R>(lhs.self(), rhs.self());
}

template<typename L, typename R>
BinaryExpr<Multiply, L, R> operator*(const Expression<L>& lhs, const Expression<R>& rhs) {
    return BinaryExpr<Multiply, L, R>(lhs.self(), rhs.self());
}

template<typename L, typename R>
BinaryExpr<Divide, L, R> operator/(const Expression<L>& lhs, const Expression<R>& rhs) {
    return BinaryExpr<Divide, L, R>(lhs.self(), rhs.self());
}

// ----------------------------------------------------------------
// 6. Main函数 - 演示
// ----------------------------------------------------------------
int main() {
    std::cout << "--- C++ Expression Templates Demo ---" << std::endl;

    // 构建表达式 "1+1-1*2+3/4" 的C++等价形式。
    // 注意：C++的运算符优先级规则在这里适用，决定了表达式树的结构。
    // 表达式树的构建是编译时发生的，没有动态内存分配。
    auto expr = Lit(1) + Lit(1) - Lit(1) * Lit(2) + Lit(3) / Lit(4);

    // 在这一步，`expr`的类型是一个非常复杂的嵌套模板类型，例如：
    // BinaryExpr<Add, BinaryExpr<Subtract, ...>, BinaryExpr<Divide, ...>>
    // 此时，没有任何计算发生。
    std::cout << "Expression object created. No calculation has been performed yet." << std::endl;

    // 触发计算：
    // 当我们将 `expr` 赋值给一个 `double` 时，`Expression::operator double()` 被调用。
    // 这个操作符接着调用 `eval()`，`eval()` 会递归地遍历整个表达式树，
    // 执行所有必要的计算并返回最终结果。
    std::cout << "Triggering calculation by assigning the expression to a double..." << std::endl;
    double result = expr; // <-- 惰性求值在此处被触发

    // 预期结果: 1 + 1 - (1 * 2) + (3 / 4) = 2 - 2 + 0.75 = 0.75
    std::cout << "Calculation complete." << std::endl;
    std::cout << "Result of 1+1-1*2+3/4 is: " << result << std::endl;

    return 0;
}

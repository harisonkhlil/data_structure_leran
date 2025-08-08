#include <iostream>
#include <type_traits>

// ----------------------------------------------------------------
// Expression基类模板 (CRTP) - 已适配 constexpr
// ----------------------------------------------------------------
template<typename E>
struct Expression {
    // 这个eval()现在是constexpr，允许在编译期调用
    constexpr double eval() const {
        return static_cast<const E&>(*this).eval();
    }

    // CRTP辅助函数
    constexpr const E& self() const {
        return static_cast<const E&>(*this);
    }

    // 保留运行时的隐式转换，以便仍然可以轻松打印结果
    operator double() const {
        return this->eval();
    }
};

// ----------------------------------------------------------------
// 字面量 (Literal) - 已适配 constexpr
// ----------------------------------------------------------------
class Lit : public Expression<Lit> {
    double value;
public:
    // 构造函数现在是constexpr
    constexpr explicit Lit(double val) : value(val) {}

    // eval()现在是constexpr
    constexpr double eval() const {
        return value;
    }
};

// ----------------------------------------------------------------
// 操作符结构体 - 已适配 constexpr
// ----------------------------------------------------------------
struct Add {
    static constexpr double apply(double a, double b) { return a + b; }
};

struct Subtract {
    static constexpr double apply(double a, double b) { return a - b; }
};

struct Multiply {
    static constexpr double apply(double a, double b) { return a * b; }
};

struct Divide {
    static constexpr double apply(double a, double b) { return a / b; }
};

// ----------------------------------------------------------------
// 二元操作表达式 - 已适配 constexpr
// ----------------------------------------------------------------
template<typename Op, typename LHS, typename RHS>
class BinaryExpr : public Expression<BinaryExpr<Op, LHS, RHS>> {
    const LHS lhs; // 注意：从引用改为值传递，以更好地兼容constexpr
    const RHS rhs;
public:
    // 构造函数现在是constexpr
    constexpr BinaryExpr(const LHS& l, const RHS& r) : lhs(l), rhs(r) {}

    // eval()现在是constexpr，并递归地调用子表达式的eval
    constexpr double eval() const {
        return Op::apply(lhs.eval(), rhs.eval());
    }
};

// ----------------------------------------------------------------
// 全局运算符重载 - 已适配 constexpr
// ----------------------------------------------------------------
template<typename L, typename R>
constexpr BinaryExpr<Add, L, R> operator+(const Expression<L>& lhs, const Expression<R>& rhs) {
    return BinaryExpr<Add, L, R>(lhs.self(), rhs.self());
}

template<typename L, typename R>
constexpr BinaryExpr<Subtract, L, R> operator-(const Expression<L>& lhs, const Expression<R>& rhs) {
    return BinaryExpr<Subtract, L, R>(lhs.self(), rhs.self());
}

template<typename L, typename R>
constexpr BinaryExpr<Multiply, L, R> operator*(const Expression<L>& lhs, const Expression<R>& rhs) {
    return BinaryExpr<Multiply, L, R>(lhs.self(), rhs.self());
}

template<typename L, typename R>
constexpr BinaryExpr<Divide, L, R> operator/(const Expression<L>& lhs, const Expression<R>& rhs) {
    return BinaryExpr<Divide, L, R>(lhs.self(), rhs.self());
}

// ----------------------------------------------------------------
// Main函数 - 演示编译期计算
// ----------------------------------------------------------------
int main() {
    std::cout << "--- C++ Compile-Time Calculation Demo ---" << std::endl;

    // 1. 创建一个constexpr表达式对象。
    //    这个对象的完整类型在编译时是已知的。
    constexpr auto expr = Lit(1) + Lit(1) - Lit(1) * Lit(2) + Lit(3) / Lit(4);

    // 2. 强制在编译期求值。
    //    通过将结果赋给一个constexpr变量，编译器必须在编译时计算出`expr.eval()`的值。
    constexpr double compile_time_result = expr.eval();

    // 3. 在编译时验证结果。
    //    如果计算结果不为0.75，代码将无法通过编译。
    //    这证明了计算是在编译期完成的。
    static_assert(compile_time_result == 0.75, "Compile-time calculation failed!");

    std::cout << "static_assert passed! The expression was successfully evaluated at compile-time." << std::endl;

    // 我们仍然可以在运行时使用它，例如打印结果。
    // 这里的`compile_time_result`已经被计算出来，直接嵌入到代码中，就像一个普通的常量。
    std::cout << "Result of 1+1-1*2+3/4 is: " << compile_time_result << std::endl;

    // 为了对比，我们也可以在运行时求值，结果是一样的。
    double runtime_result = expr; // 触发 operator double()
    std::cout << "The same expression evaluated at runtime gives: " << runtime_result << std::endl;

    return 0;
}

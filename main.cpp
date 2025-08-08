#include <iostream>
#include <utility>
#include <type_traits>

// ----------------------------------------------------------------
// 1. Expression 基类 (转换为经典CRTP)
// ----------------------------------------------------------------
template <typename T, typename E>
struct Expression {
  using value_type = T;

  // 使用 static_cast 的经典CRTP self() 方法
  constexpr const E& self() const {
    return static_cast<const E&>(*this);
  }

  // 为支持移动语义，提供非const版本
  constexpr E& self() {
    return static_cast<E&>(*this);
  }

  // eval现在通过self()调用派生类的实现
  constexpr T eval() const & {
    return self().eval();
  }
   constexpr T eval() && {
    return std::move(self()).eval();
  }

  // 隐式转换
  constexpr operator T() const & {
    return self().eval();
  }
  constexpr operator T() && {
    return std::move(self()).eval();
  }
};

// ----------------------------------------------------------------
// 2. 您的 Lit 类 (增加了右值eval)
// ----------------------------------------------------------------
template <typename T>
class Lit : public Expression<T, Lit<T>> {
  T value;

public:
  template <typename U>
  constexpr explicit Lit(U&& val) : value(std::forward<U>(val)) {}

  // Lit的eval实现
  constexpr T eval() const & { return value; }
  constexpr T eval() && { return std::move(value); }
};

// ----------------------------------------------------------------
// 3. 您的操作符结构体 (无需修改)
// ----------------------------------------------------------------
struct Add {
  template <typename T>
  static constexpr T apply(T a, T b) { return a + b; }
};
struct Subtract {
  template <typename T>
  static constexpr T apply(T a, T b) { return a - b; }
};
struct Multiply {
  template <typename T>
  static constexpr T apply(T a, T b) { return a * b; }
};
struct Divide {
  template <typename T>
  static constexpr T apply(T a, T b) { return a / b; }
};

// ----------------------------------------------------------------
// 4. 您的 BinaryExpr 类 (增加右值eval以实现移动语义)
// ----------------------------------------------------------------
template <typename T, typename Op, typename LHS, typename RHS>
class BinaryExpr : public Expression<T, BinaryExpr<T, Op, LHS, RHS>> {
  LHS lhs;
  RHS rhs;

public:
  template<typename FwdL, typename FwdR>
  constexpr BinaryExpr(FwdL&& l, FwdR&& r)
      : lhs(std::forward<FwdL>(l)), rhs(std::forward<FwdR>(r)) {}

  // 左值版本eval
  constexpr T eval() const & {
    return Op::template apply<T>(lhs.eval(), rhs.eval());
  }

  // 右值版本eval，传播移动语义
  constexpr T eval() && {
    return Op::template apply<T>(std::move(lhs).eval(), std::move(rhs).eval());
  }
};

// ----------------------------------------------------------------
// 5. 您的宏定义 (无需修改)
// ----------------------------------------------------------------
#define DEFINE_BINARY_OPERATOR(op, OpName)                                     \
  template <typename LHS, typename RHS>                                        \
  constexpr auto operator op(LHS&& lhs, RHS&& rhs) {                           \
    using T = typename std::decay_t<LHS>::value_type;                          \
    using L = std::decay_t<LHS>;                                               \
    using R = std::decay_t<RHS>;                                               \
    return BinaryExpr<T, OpName, L, R>(std::forward<LHS>(lhs),                 \
                                       std::forward<RHS>(rhs));                \
  }

DEFINE_BINARY_OPERATOR(+, Add)
DEFINE_BINARY_OPERATOR(-, Subtract)
DEFINE_BINARY_OPERATOR(*, Multiply)
DEFINE_BINARY_OPERATOR(/, Divide)

// ----------------------------------------------------------------
// 6. 我添加的 main 函数用于测试
// ----------------------------------------------------------------
int main() {
    std::cout << "--- Testing Your Expression Template Code (Classic CRTP) ---\n" << std::endl;

    // --- 示例1: 使用 float 类型 ---
    std::cout << "--- Float Example ---" << std::endl;
    constexpr float float_result = Lit<float>(1.0f) + Lit<float>(1.0f) - Lit<float>(1.0f) * Lit<float>(2.0f) + Lit<float>(3.0f) / Lit<float>(4.0f);
    static_assert(float_result == 0.75f, "Float calculation failed!");
    std::cout << "Expression: 1+1-1*2+3/4" << std::endl;
    std::cout << "Result with <float>: " << float_result << "\n" << std::endl;

    // --- 示例2: 使用 int 类型 ---
    std::cout << "--- Integer Example ---" << std::endl;
    constexpr int int_result = Lit<int>(1) + Lit<int>(1) - Lit<int>(1) * Lit<int>(2) + Lit<int>(3) / Lit<int>(4);
    static_assert(int_result == 0, "Integer calculation failed!");
    std::cout << "Expression: 1+1-1*2+3/4" << std::endl;
    std::cout << "Result with <int>: " << int_result << " (Note: 3/4 is 0 in integer math)" << std::endl;

    // --- 示例3: 验证移动语义 ---
    auto expr = Lit<int>(10) + Lit<int>(5);
    int moved_result = std::move(expr); // 这会调用右值版本的eval
    std::cout << "\nResult from moved expression: " << moved_result << std::endl;
    static_assert((Lit<int>(10) + Lit<int>(5)).eval() == 15, "Move test failed");


    return 0;
}

import random

from tester import Testcase


class Test(Testcase):
    UpperBound = 256

    def eval(self, expr: str) -> int:
        stack = []
        for c in expr:
            if c == '&':
                x = stack.pop()
                y = stack.pop()
                stack.append(x and y)
            elif c == '|':
                x = stack.pop()
                y = stack.pop()
                stack.append(x or y)
            elif c == '~':
                x = stack.pop()
                stack.append(not x)
            else:
                stack.append(c == '1')
        return int(stack.pop())

    def rand_testcase(self) -> tuple:
        # 0: false, 1: true
        # &: and, |: or, ~: not
        # expr should in the form of reverse polish notation
        # e.g. 1 0 | 1 ~ &
        #    = (1 | 0) & (~1)
        max_steps = random.randint(3, self.UpperBound)
        expr = []

        for _ in range(max_steps):
            op = random.choice('&|~')
            if op == '~':
                if expr:
                    expr.append(op)
                else:
                    expr.append(str(random.randint(0, 1)))
                    expr.append(op)
            else:
                if expr:
                    expr.append(str(random.randint(0, 1)))
                    expr.append(op)
                else:
                    expr += [str(random.randint(0, 1)),
                             str(random.randint(0, 1)), op]
        input_ = ''.join(expr)
        return input_, str(self.eval(input_))


if __name__ == '__main__':
    Test.parse_args()

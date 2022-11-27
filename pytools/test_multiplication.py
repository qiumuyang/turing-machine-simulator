from tester import Testcase


class Test(Testcase):
    OperandBound = 16
    ResultBound = 128

    def rand_testcase(self) -> tuple:
        import random

        x = random.randint(1, self.OperandBound)
        y = random.randint(1, self.OperandBound)
        x_str = '1' * x
        y_str = '1' * y
        if random.uniform(0, 1) < 0.5:
            diff = random.randint(-x * y, x * y)
        else:
            diff = 0
        mul_str = '1' * (x * y + diff)
        mul_str = mul_str[: self.ResultBound]
        return (
            f'{x_str}x{y_str}={mul_str}',
            'true' if x * y == len(mul_str) else 'false',
        )


if __name__ == '__main__':
    Test.parse_args()

from tester import Testcase


class Test(Testcase):
    UpperBound = 512

    def _gcd(self, x: int, y: int) -> int:
        while y:
            x, y = y, x % y
        return x

    def rand_testcase(self) -> tuple:
        import random

        x = random.randint(0, self.UpperBound)
        y = random.randint(0, self.UpperBound)
        input_ = '1' * x + '0' + '1' * y
        output = '1' * self._gcd(x, y)
        return input_, output


if __name__ == '__main__':
    Test.parse_args()

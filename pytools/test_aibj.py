from tester import Testcase


class Test(Testcase):
    def rand_testcase(self) -> tuple:
        import random

        if random.uniform(0, 1) < 0.5:
            x = [random.randint(0, 5) for _ in range(4)]
            symbol = [random.choice('ab') for _ in range(4)]
            if (
                all(_ > 0 for _ in x)
                and x[0] == x[2]
                and x[1] == x[3]
                and ''.join(symbol) == 'abab'
            ):
                return ''.join(c * n for c, n in zip(symbol, x)), 'true'
            else:
                return ''.join(c * n for c, n in zip(symbol, x)), 'false'
        else:
            i, j = random.randint(1, 100), random.randint(1, 100)
            return ('a' * i + 'b' * j) * 2, 'true'


if __name__ == '__main__':
    Test.parse_args()

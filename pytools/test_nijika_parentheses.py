import random

from tester import Testcase


class Test(Testcase):
    UpperBound = 512

    def wrap(self, s: str) -> str:
        return '(' + s + ')'

    def concat(self, s: str) -> str:
        if random.randint(0, 1):
            return s + '()'
        else:
            return '()' + s

    def rand_testcase(self) -> tuple:
        n_actions = random.randint(1, self.UpperBound)
        actions = [self.wrap, self.concat]
        s = ''
        for _ in range(n_actions):
            s = random.choice(actions)(s)
        if random.randint(0, 1):
            fail = random.randint(0, len(s) - 1)
            return s[:fail] + s[fail + 1:], ':('
        else:
            return s, ':)'


if __name__ == '__main__':
    Test.parse_args()

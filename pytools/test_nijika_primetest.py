from tester import Testcase


class Test(Testcase):
    UpperBound = 256

    def is_prime(self, x: int) -> bool:
        if x == 2:
            return True
        if x < 2 or x % 2 == 0:
            return False
        for i in range(3, int(x ** 0.5) + 1, 2):
            if x % i == 0:
                return False
        return True

    def rand_testcase(self) -> tuple:
        import random

        x = random.randint(2, self.UpperBound)
        bin_x = bin(x)[2:]
        if self.is_prime(x):
            output = bin_x + '_is_prime!'
        else:
            output = bin_x + '_is_not_prime.'
        return bin_x, output


if __name__ == '__main__':
    Test.parse_args()

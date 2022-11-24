from tester import Testcase
from random import choice, randint, uniform
from string import ascii_letters


class Test(Testcase):
    Alphabet = ascii_letters + '+-'
    Parenthesis = ['()', '[]', '<>']

    @staticmethod
    def rand_str(alphabet: str, low: int = 2, high: int = 5) -> str:
        return ''.join(choice(alphabet) for _ in range(randint(low, high)))

    @staticmethod
    def add_parenthesis(s: str, type_: str = '()') -> str:
        return type_[0] + s + type_[1]

    @staticmethod
    def add_normal(s: str) -> str:
        head = Test.rand_str(Test.Alphabet, 0, 2)
        tail = Test.rand_str(Test.Alphabet, 1, 3)
        return head + s + tail

    def rand_testcase(self) -> tuple:
        steps = randint(3, 10)
        s = Test.rand_str(Test.Alphabet, 1, 3)
        for _ in range(steps):
            method = choice([Test.add_parenthesis, Test.add_normal])
            if method == Test.add_parenthesis:
                s = method(s, choice(Test.Parenthesis))
            else:
                s = method(s)
        if uniform(0, 1) < 0.5:
            return s, 'true'
        else:
            # corrupt some parenthesis
            left = s.count('(') + s.count('[') + s.count('<')
            if not left:
                return s, 'true'

            repl = randint(1, left)
            for _ in range(repl):
                if '(' in s:
                    s = s.replace('(', '', 1)
                elif '[' in s:
                    s = s.replace('[', '', 1)
                elif '<' in s:
                    s = s.replace('<', '', 1)
            return s, 'false'


Test.parse_args()
